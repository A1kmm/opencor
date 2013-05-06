//==============================================================================
// Single cell view simulation
//==============================================================================

#include "cellmlfileruntime.h"
#include "singlecellviewcontentswidget.h"
#include "singlecellviewinformationsimulationwidget.h"
#include "singlecellviewinformationwidget.h"
#include "singlecellviewsimulation.h"
#include "singlecellviewwidget.h"

//==============================================================================

#include <QFile>
#include <QTextStream>

//==============================================================================

#include <qmath.h>

//==============================================================================

#include "qwt_slider.h"

//==============================================================================

namespace OpenCOR {
namespace SingleCellView {

//==============================================================================

SingleCellViewSimulationData::SingleCellViewSimulationData(CellMLSupport::CellMLFileRuntime *pRuntime) :
    mRuntime(pRuntime),
    mDelay(0),
    mStartingPoint(0.0),
    mEndingPoint(1000.0),
    mPointInterval(1.0),
    mSolverName(QString()),
    mSolverProperties(Properties())
{
    // Create our various arrays, if possible
    mConstants = mStates = mRates = mAlgebraic = mCondVar =
        mInitialConstants = mInitialStates = NULL;
    allocateStorageArrays();
}

//==============================================================================

SingleCellViewSimulationData::~SingleCellViewSimulationData()
{
    // Delete some internal objects

    delete[] mConstants;
    delete[] mStates;
    delete[] mRates;
    delete[] mAlgebraic;
    delete[] mCondVar;

    delete[] mInitialConstants;
    delete[] mInitialStates;
}

//==============================================================================

void SingleCellViewSimulationData::allocateStorageArrays()
{
    if (mRuntime) {
        bool isDAEType = isDAETypeSolver();
        iface::cellml_services::CellMLCompiledModel*
            compModel(isDAEType ?
                      static_cast<iface::cellml_services::CellMLCompiledModel*>
                      (mRuntime->daeCompiledModel()) :
                      static_cast<iface::cellml_services::CellMLCompiledModel*>
                      (mRuntime->odeCompiledModel()));
        ObjRef<iface::cellml_services::CodeInformation> codeInfo
            (compModel->codeInformation());
        mConstants = new double[codeInfo->constantIndexCount()];
        mRates = new double[codeInfo->rateIndexCount()];
        mStates = new double[codeInfo->rateIndexCount()];
        mAlgebraic = new double[codeInfo->algebraicIndexCount()];
        ObjRef<iface::cellml_services::IDACodeInformation> idaCodeInfo
            (QueryInterface(codeInfo));
        if (idaCodeInfo)
            mCondVar = new double[idaCodeInfo->conditionVariableCount()];
        mInitialConstants = new double[codeInfo->constantIndexCount()];
        mInitialStates = new double[codeInfo->rateIndexCount()];
    }
}

void SingleCellViewSimulationData::freeStorageArrays()
{
    if (mConstants)
        delete[] mConstants;
    if (mStates)
        delete[] mStates;
    if (mRates)
        delete[] mRates;
    if (mAlgebraic)
        delete[] mAlgebraic;
    if (mCondVar)
        delete[] mCondVar;
    if (mInitialConstants)
        delete[] mInitialConstants;
    if (mInitialStates)
        delete[] mInitialStates;
}

//==============================================================================

double * SingleCellViewSimulationData::constants() const
{
    // Return our constants array

    return mConstants;
}

//==============================================================================

double * SingleCellViewSimulationData::states() const
{
    // Return our states array

    return mStates;
}

//==============================================================================

double * SingleCellViewSimulationData::rates() const
{
    // Return our rates array

    return mRates;
}

//==============================================================================

double * SingleCellViewSimulationData::algebraic() const
{
    // Return our algebraic array

    return mAlgebraic;
}

//==============================================================================

double * SingleCellViewSimulationData::condVar() const
{
    // Return our condVar array

    return mCondVar;
}

//==============================================================================

int SingleCellViewSimulationData::delay() const
{
    // Return our delay

    return mDelay;
}

//==============================================================================

void SingleCellViewSimulationData::setDelay(const int &pDelay)
{
    // Set our delay

    mDelay = pDelay;
}

//==============================================================================

double SingleCellViewSimulationData::startingPoint() const
{
    // Return our starting point

    return mStartingPoint;
}

//==============================================================================

void SingleCellViewSimulationData::setStartingPoint(const double &pStartingPoint,
                                                    const bool &pRecompute)
{
    // Set our starting point

    mStartingPoint = pStartingPoint;

    // Recompute our 'computed constants' and 'variables'

    if (pRecompute)
        recomputeComputedConstantsAndVariables();
}

//==============================================================================

double SingleCellViewSimulationData::endingPoint() const
{
    // Return our ending point

    return mEndingPoint;
}

//==============================================================================

void SingleCellViewSimulationData::setEndingPoint(const double &pEndingPoint)
{
    // Set our ending point

    mEndingPoint = pEndingPoint;
}

//==============================================================================

double SingleCellViewSimulationData::pointInterval() const
{
    // Return our point interval

    return mPointInterval;
}

//==============================================================================

void SingleCellViewSimulationData::setPointInterval(const double &pPointInterval)
{
    // Set our point interval

    mPointInterval = pPointInterval;
}

//==============================================================================

QString SingleCellViewSimulationData::solverName() const
{
    // Return our ODE solver name

    return mSolverName;
}

//==============================================================================

void SingleCellViewSimulationData::setSolverName(const QString &pSolverName)
{
    // Set our ODE solver name
    mSolverName = pSolverName;

    // Reallocate the arrays in case we changed their sizes.
    freeStorageArrays();
    allocateStorageArrays();
}

//==============================================================================

bool SingleCellViewSimulationData::isDAETypeSolver() const
{
    return (mSolverName == "IDA");
}

//==============================================================================

Properties SingleCellViewSimulationData::solverProperties() const
{
    // Return our ODE solver's properties

    return mSolverProperties;
}

//==============================================================================

void SingleCellViewSimulationData::addSolverProperty(const QString &pName,
                                                     const QVariant &pValue)
{
    // Add an ODE solver property
    mSolverProperties.insert(pName, pValue);
}

//==============================================================================

static class GrabInitialValueListener 
    : public iface::cellml_services::IntegrationProgressObserver {
    
    void computedConstants(const std::vector<double>& constValues) throw() {
    }
    void results(const std::vector<double>& aState) throw(std::exception&) {
    }
    void done() throw() {}
    void failed() throw() {}
};

void SingleCellViewSimulationData::reset()
{
    // Reset our model parameter values which means both initialising our
    // 'constants' and computing our 'computed constants' and 'variables'
    // Note #1: we must check whether our runtime needs NLA solver and, if so,
    //          then retrieve an instance of our NLA solver since some of the
    //          resetting may require solving one or several NLA systems...
    // Note #2: recomputeComputedConstantsAndVariables() will let people know
    //          that our data has changed...

    // Reset our model parameter values

    static const int SizeOfDouble = sizeof(double);

    bool isDAEType = isDAETypeSolver();
    iface::cellml_services::CellMLCompiledModel*
        compModel(isDAEType ?
                  static_cast<iface::cellml_services::CellMLCompiledModel*>
                  (mRuntime->daeCompiledModel()) :
                  static_cast<iface::cellml_services::CellMLCompiledModel*>
                  (mRuntime->odeCompiledModel()));
    ObjRef<iface::cellml_services::CodeInformation> codeInfo
        (compModel->codeInformation());

    memset(mConstants, 0, codeInfo->constantIndexCount()*SizeOfDouble);
    memset(mStates, 0, codeInfo->rateIndexCount()*SizeOfDouble);
    memset(mRates, 0, codeInfo->rateIndexCount()*SizeOfDouble);
    memset(mAlgebraic, 0, codeInfo->algebraicIndexCount()*SizeOfDouble);
    ObjRef<iface::cellml_services::IDACodeInformation> idaCodeInfo
        (QueryInterface(codeInfo));
    if (idaCodeInfo)
        memset(mCondVar, 0, idaCodeInfo->conditionVariableCount()*SizeOfDouble);

    mRuntime->initializeConstants()(mConstants, mRates, mStates);
    recomputeComputedConstantsAndVariables();

    // Keep track of our various initial values
    memcpy(mInitialConstants, mConstants, mRuntime->constantsCount()*SizeOfDouble);
    memcpy(mInitialStates, mStates, mRuntime->statesCount()*SizeOfDouble);

    // Let people know that our data is 'cleaned', i.e. not modified
    emit modified(false);
}

//==============================================================================

void SingleCellViewSimulationData::recomputeComputedConstantsAndVariables()
{
    // Recompute our 'computed constants' and 'variables', if possible

    if (mRuntime && mRuntime->isValid()) {
        mRuntime->computeComputedConstants()(mConstants, mRates, mStates);
        mRuntime->computeVariables()(mStartingPoint, mConstants, mRates, mStates, mAlgebraic);

        // Let people know that our data has been updated

        emit updated();
    }
}

//==============================================================================

void SingleCellViewSimulationData::recomputeVariables(const double &pCurrentPoint,
                                                      const bool &pEmitSignal)
{
    // Recompute our 'variables'

    mRuntime->computeVariables()(pCurrentPoint, mConstants, mRates, mStates, mAlgebraic);

    // Let people know that our data has been updated, if requested
    // Note: recomputeVariables() will normally be called many times when
    //       running a simulation to ensure that all of our 'variables' are
    //       up-to-date and to emit loads of signals wouldn't be a good idea,
    //       hence the caller can decide whether to emit a signal or not...

    if (pEmitSignal)
        emit updated();
}

//==============================================================================

void SingleCellViewSimulationData::checkForModifications()
{
    // Check whether any of our constants or states has been modified

    foreach (CellMLSupport::CellMLFileRuntimeModelParameter *modelParameter, mRuntime->modelParameters())
        switch (modelParameter->type()) {
        case CellMLSupport::CellMLFileRuntimeModelParameter::Constant:
            if (mConstants[modelParameter->index()] != mInitialConstants[modelParameter->index()]) {
                emit modified(true);

                return;
            }

            break;
        case CellMLSupport::CellMLFileRuntimeModelParameter::State:
            if (mStates[modelParameter->index()] != mInitialStates[modelParameter->index()]) {
                emit modified(true);

                return;
            }

            break;
        default:
            // Either Voi, ComputedConstant, Rate, Algebraic or Undefined, so...

            ;
        }

    // Let people know that no data has been modified

    emit modified(false);
}

//==============================================================================

SingleCellViewSimulationResults::SingleCellViewSimulationResults(CellMLSupport::CellMLFileRuntime *pRuntime,
                                                                 SingleCellViewSimulation *pSimulation) :
    mRuntime(pRuntime),
    mSimulation(pSimulation),
    mSize(0),
    mPoints(0),
    mConstants(0),
    mStates(0),
    mRates(0),
    mAlgebraic(0)
{
}

//==============================================================================

SingleCellViewSimulationResults::~SingleCellViewSimulationResults()
{
    // Delete some internal objects

    deleteArrays();
}

//==============================================================================

bool SingleCellViewSimulationResults::createArrays()
{
    static const int SizeOfDoublePointer = sizeof(double *);

    // Retrieve the size of our data and make sure that it is valid

    qulonglong simulationSize = qulonglong(mSimulation->size());

    if (!simulationSize)
        return true;

    // Create our points array

    try {
        mPoints = new double[simulationSize];
    } catch(...) {
        return false;
    }

    // Create our constants arrays

    try {
        mConstants = new double*[mRuntime->constantsCount()];

        memset(mConstants, 0, mRuntime->constantsCount()*SizeOfDoublePointer);
    } catch(...) {
        deleteArrays();

        return false;
    }

    for (int i = 0, iMax = mRuntime->constantsCount(); i < iMax; ++i)
        try {
            mConstants[i] = new double[simulationSize];
        } catch(...) {
            deleteArrays();

            return false;
        }

    // Create our states arrays

    try {
        mStates = new double*[mRuntime->statesCount()];

        memset(mStates, 0, mRuntime->statesCount()*SizeOfDoublePointer);
    } catch(...) {
        deleteArrays();

        return false;
    }

    for (int i = 0, iMax = mRuntime->statesCount(); i < iMax; ++i)
        try {
            mStates[i] = new double[simulationSize];
        } catch(...) {
            deleteArrays();

            return false;
        }

    // Create our rates arrays

    try {
        mRates = new double*[mRuntime->ratesCount()];

        memset(mRates, 0, mRuntime->ratesCount()*SizeOfDoublePointer);
    } catch(...) {
        deleteArrays();

        return false;
    }

    for (int i = 0, iMax = mRuntime->ratesCount(); i < iMax; ++i)
        try {
            mRates[i] = new double[simulationSize];
        } catch(...) {
            deleteArrays();

            return false;
        }

    // Create our algebraic arrays

    try {
        mAlgebraic = new double*[mRuntime->algebraicCount()];

        memset(mAlgebraic, 0, mRuntime->algebraicCount()*SizeOfDoublePointer);
    } catch(...) {
        deleteArrays();

        return false;
    }

    for (int i = 0, iMax = mRuntime->algebraicCount(); i < iMax; ++i)
        try {
            mAlgebraic[i] = new double[simulationSize];
        } catch(...) {
            deleteArrays();

            return false;
        }

    // We could allocate all of our required memory, so...

    return true;
}

//==============================================================================

void SingleCellViewSimulationResults::deleteArrays()
{
    // Delete our points array

    delete[] mPoints;

    mPoints = 0;

    // Delete our constants arrays

    if (mConstants)
        for (int i = 0, iMax = mRuntime->constantsCount(); i < iMax; ++i)
            delete[] mConstants[i];

    delete mConstants;

    mConstants = 0;

    // Delete our states arrays

    if (mStates)
        for (int i = 0, iMax = mRuntime->statesCount(); i < iMax; ++i)
            delete[] mStates[i];

    delete mStates;

    mStates = 0;

    // Delete our rates arrays

    if (mRates)
        for (int i = 0, iMax = mRuntime->ratesCount(); i < iMax; ++i)
            delete[] mRates[i];

    delete mRates;

    mRates = 0;

    // Delete our algebraic arrays

    if (mAlgebraic)
        for (int i = 0, iMax = mRuntime->algebraicCount(); i < iMax; ++i)
            delete[] mAlgebraic[i];

    delete mAlgebraic;

    mAlgebraic = 0;
}

//==============================================================================

bool SingleCellViewSimulationResults::reset(const bool &pCreateArrays)
{
    // Reset our size

    mSize = 0;

    // Reset our arrays

    deleteArrays();

    return pCreateArrays?createArrays():true;
}

//==============================================================================

void SingleCellViewSimulationResults::addPoint(const double &pPoint)
{
    // Add the data to our different arrays

    mPoints[mSize] = pPoint;

    for (int i = 0, iMax = mRuntime->constantsCount(); i < iMax; ++i)
        mConstants[i][mSize] = mSimulation->data()->constants()[i];

    for (int i = 0, iMax = mRuntime->statesCount(); i < iMax; ++i)
        mStates[i][mSize] = mSimulation->data()->states()[i];

    for (int i = 0, iMax = mRuntime->ratesCount(); i < iMax; ++i)
        mRates[i][mSize] = mSimulation->data()->rates()[i];

    for (int i = 0, iMax = mRuntime->algebraicCount(); i < iMax; ++i)
        mAlgebraic[i][mSize] = mSimulation->data()->algebraic()[i];

    // Increase our size

    ++mSize;
}

//==============================================================================

qulonglong SingleCellViewSimulationResults::size() const
{
    // Return our size

    return mSize;
}

//==============================================================================

double * SingleCellViewSimulationResults::points() const
{
    // Return our points

    return mPoints;
}

//==============================================================================

double ** SingleCellViewSimulationResults::constants() const
{
    // Return our constants array

    return mConstants;
}

//==============================================================================

double ** SingleCellViewSimulationResults::states() const
{
    // Return our states array

    return mStates;
}

//==============================================================================

double ** SingleCellViewSimulationResults::rates() const
{
    // Return our rates array

    return mRates;
}

//==============================================================================

double ** SingleCellViewSimulationResults::algebraic() const
{
    // Return our algebraic array

    return mAlgebraic;
}

//==============================================================================

bool SingleCellViewSimulationResults::exportToCsv(const QString &pFileName) const
{
    // Export of all of our data to a CSV file

    QFile file(pFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // The file can't be opened, so...

        file.remove();

        return false;
    }

    // Write out the contents of the CellML file to the file

    QTextStream out(&file);

    // Header

    static const QString Header = "%1 | %2 (%3)";

    out << Header.arg(mRuntime->variableOfIntegration()->component(),
                      mRuntime->variableOfIntegration()->name(),
                      mRuntime->variableOfIntegration()->unit());

    for (int i = 0, iMax = mRuntime->modelParameters().count(); i < iMax; ++i) {
        CellMLSupport::CellMLFileRuntimeModelParameter *modelParameter = mRuntime->modelParameters()[i];

        out << "," << Header.arg(modelParameter->component(),
                                 modelParameter->name()+QString(modelParameter->degree(), '\''),
                                 modelParameter->unit());
    }

    out << "\n";

    // Data itself

    for (qulonglong j = 0; j < mSize; ++j) {
        out << mPoints[j];

        for (int i = 0, iMax = mRuntime->modelParameters().count(); i < iMax; ++i) {
            CellMLSupport::CellMLFileRuntimeModelParameter *modelParameter = mRuntime->modelParameters()[i];

            switch (modelParameter->type()) {
            case CellMLSupport::CellMLFileRuntimeModelParameter::Constant:
            case CellMLSupport::CellMLFileRuntimeModelParameter::ComputedConstant:
                out << "," << mConstants[modelParameter->index()][j];

                break;
            case CellMLSupport::CellMLFileRuntimeModelParameter::State:
                out << "," << mStates[modelParameter->index()][j];

                break;
            case CellMLSupport::CellMLFileRuntimeModelParameter::Rate:
                out << "," << mRates[modelParameter->index()][j];

                break;
            case CellMLSupport::CellMLFileRuntimeModelParameter::Algebraic:
                out << "," << mAlgebraic[modelParameter->index()][j];

                break;
            default:
                // Either Voi or Undefined, so...

                ;
            }
        }

        out << "\n";
    }

    // We are done, so close our file

    file.close();

    // Everything went fine, so...

    return true;
}

//==============================================================================

SingleCellViewSimulation::SingleCellViewSimulation(const QString &pFileName,
                                                   CellMLSupport::CellMLFileRuntime *pRuntime) :
    mWorker(0),
    mFileName(pFileName),
    mRuntime(pRuntime),
    mData(new SingleCellViewSimulationData(pRuntime)),
    mResults(new SingleCellViewSimulationResults(pRuntime, this))
{
    // Keep track of any error occurring in our data

    connect(mData, SIGNAL(error(const QString &)),
            this, SIGNAL(error(const QString &)));
}

//==============================================================================

SingleCellViewSimulation::~SingleCellViewSimulation()
{
    // Stop our worker (just in case...)

    stop();

    // Delete some internal objects

    delete mResults;
    delete mData;
}

//==============================================================================

QString SingleCellViewSimulation::fileName() const
{
    // Retrieve and return our file name

    return mFileName;
}

//==============================================================================

SingleCellViewSimulationData * SingleCellViewSimulation::data() const
{
    // Retrieve and return our data

    return mData;
}

//==============================================================================

SingleCellViewSimulationResults * SingleCellViewSimulation::results() const
{
    // Return our results

    return mResults;
}

//==============================================================================

bool SingleCellViewSimulation::isRunning() const
{
    // Return whether we are running

    return mWorker?mWorker->isRunning():false;
}

//==============================================================================

bool SingleCellViewSimulation::isPaused() const
{
    // Return whether we are paused

    return mWorker?mWorker->isPaused():false;
}

//==============================================================================

double SingleCellViewSimulation::progress() const
{
    // Return our progress

    return mWorker?mWorker->progress():0.0;
}

//==============================================================================

void SingleCellViewSimulation::setDelay(const int &pDelay)
{
    // Set our delay

    mData->setDelay(pDelay);
}

//==============================================================================

double SingleCellViewSimulation::requiredMemory()
{
    // Determine and return the amount of required memory to run our simulation
    // Note: we return the amount as a double rather than a qulonglong (as we do
    //       when retrieving the total/free amount of memory available; see
    //       [OpenCOR]/src/plugins/misc/Core/src/coreutils.cpp) in case a
    //       simulation requires an insane amount of memory...

    static const int SizeOfDouble = sizeof(double);

    return  size()
           *( 1
             +mRuntime->constantsCount()
             +mRuntime->statesCount()
             +mRuntime->ratesCount()
             +mRuntime->algebraicCount())
           *SizeOfDouble;
}

//==============================================================================

bool SingleCellViewSimulation::simulationSettingsOk(const bool &pEmitError)
{
    // Check and return whether our simulation settings are sound

    if (mData->startingPoint() == mData->endingPoint()) {
        if (pEmitError)
            emit error(tr("the starting and ending points cannot have the same value"));

        return false;
    } else if (mData->pointInterval() == 0) {
        if (pEmitError)
            emit error(tr("the point interval cannot be equal to zero"));

        return false;
    } else if (   (mData->startingPoint() < mData->endingPoint())
             && (mData->pointInterval() < 0)) {
        if (pEmitError)
            emit error(tr("the ending point is greater than the starting point, so the point interval should be greater than zero"));

        return false;
    } else if (   (mData->startingPoint() > mData->endingPoint())
             && (mData->pointInterval() > 0)) {
        if (pEmitError)
            emit error(tr("the ending point is smaller than the starting point, so the point interval should be smaller than zero"));

        return false;
    } else {
        return true;
    }
}

//==============================================================================

double SingleCellViewSimulation::size()
{
    // Return the size of our simulation (i.e. the number of data points which
    // should be generated)
    // Note: we return a double rather than a qulonglong in case the simulation
    //       requires an insane amount of memory...

    if (simulationSettingsOk(false))
        // Our simulation settings are fine, so...

        return ceil((mData->endingPoint()-mData->startingPoint())/mData->pointInterval())+1.0;
    else
        // Something wrong with our simulation settings, so...

        return 0.0;
}

//==============================================================================

void SingleCellViewSimulation::run()
{
    // Initialise our worker, if not active

    if (!mWorker) {
        // First, check that our simulation settings we were given are sound

        if (!simulationSettingsOk())
            // Something wrong with our simulation settings, so...

            return;

        // Create our worker

        mWorker = new SingleCellViewSimulationWorker(mSolverInterfaces, mRuntime, this, &mWorker);

        if (!mWorker) {
            emit error(tr("the simulation worker could not be created"));

            return;
        }

        // Create a few connections

        connect(mWorker, SIGNAL(running(const bool &)),
                this, SIGNAL(running(const bool &)));
        connect(mWorker, SIGNAL(paused()),
                this, SIGNAL(paused()));

        connect(mWorker, SIGNAL(finished(const int &)),
                this, SIGNAL(stopped(const int &)));

        connect(mWorker, SIGNAL(error(const QString &)),
                this, SIGNAL(error(const QString &)));

        // Start our worker

        mWorker->run();
    }
}

//==============================================================================

void SingleCellViewSimulation::pause()
{
    // Ask our worker to pause, if active

    if (mWorker)
        mWorker->pause();
}

//==============================================================================

void SingleCellViewSimulation::resume()
{
    // Ask our worker to resume, if active

    if (mWorker)
        mWorker->resume();
}

//==============================================================================

void SingleCellViewSimulation::stop()
{
    // Ask our worker to stop, if active

    if (mWorker)
        mWorker->stop();
}

//==============================================================================

void SingleCellViewSimulation::resetWorker()
{
    // Ask our worker to reset itself

    if (mWorker)
        mWorker->reset();
}

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
