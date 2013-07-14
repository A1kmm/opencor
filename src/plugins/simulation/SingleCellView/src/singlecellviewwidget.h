//==============================================================================
// Single cell view widget
//==============================================================================

#ifndef SINGLECELLVIEWWIDGET_H
#define SINGLECELLVIEWWIDGET_H

//==============================================================================

#include "solverinterface.h"
#include "viewwidget.h"
#include "qwt_series_data.h"

//==============================================================================

class QFrame;
class QLabel;
class QProgressBar;
class QScrollBar;
class QSettings;
class QSplitter;
class QTextEdit;

//==============================================================================

class QwtWheel;

//==============================================================================

namespace Ui {
    class SingleCellViewWidget;
}

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class Property;
    class ProgressBarWidget;
    class ToolBarWidget;
    class UserMessageWidget;
}   // namespace Core

//==============================================================================

namespace CellMLSupport {
    class CellMLFileRuntimeParameter;
    class CellMLFileRuntimeCompiledModelParameter;
}   // namespace CellMLSupport

//==============================================================================

namespace SingleCellView {

//==============================================================================

class SingleCellViewContentsWidget;
class SingleCellViewGraphPanelPlotCurve;
class SingleCellViewGraphPanelWidget;
class SingleCellViewPlugin;
class SingleCellViewSimulation;
class SingleCellViewSimulationData;
class SingleCellViewSimulationResults;
class SingleCellViewGraphPanelPlotWidget;

//==============================================================================

//==============================================================================

/**
 * This class represents a record of a variable that can be plotted on a graph.
 * There is only one object of this class for each variable; due to the repeats
 * feature, there may be multiple traces associated with this data (each represented
 * by a SingleCellViewGraphPanelPlotCurve.
 */
class SingleCellViewWidgetCurveData
{
public:
    SingleCellViewWidgetCurveData(const QString &pFileName,
                                  SingleCellViewSimulation *pSimulation,
                                  QSharedPointer<CellMLSupport::CellMLFileRuntimeParameter>
                                    pModelParameter);

    QString fileName() const;

    QSharedPointer<CellMLSupport::CellMLFileRuntimeCompiledModelParameter> parameter() const;

    const QList<QSharedPointer<SingleCellViewGraphPanelPlotCurve> > curves() const;
    QList<QSharedPointer<SingleCellViewGraphPanelPlotCurve> > curves();

    /**
     * Set up the right number of SingleCellViewGraphPanelPlotCurve objects to
     * match the number of repeats (reusing existing objects where possible),
     * and attaches or detaches them from a plot, depending on isAttached().
     * If pActive is false, always detach.
     */
    void updateCurves(SingleCellViewGraphPanelPlotWidget* pPlot, bool pActive);

    qulonglong plottedCurve() const { return mPlottedCurve; }
    qulonglong plottedPoint() const { return mPlottedPoint; }
    void plottedCurve(qulonglong pValue) { mPlottedCurve = pValue; }
    void plottedPoint(qulonglong pValue) { mPlottedPoint = pValue; }

    bool isAttached() const;
    void setAttached(const bool &pAttached);

    QSharedPointer<CellMLSupport::CellMLFileRuntimeCompiledModelParameter> modelParameterY()
    {
        return mParameterY;
    }

private:
    QString mFileName;
    SingleCellViewSimulation *mSimulation;
    QSharedPointer<CellMLSupport::CellMLFileRuntimeCompiledModelParameter> mParameterY;
    QList<QSharedPointer<SingleCellViewGraphPanelPlotCurve> > mCurves;
    bool mAttached;
    qulonglong mPlottedCurve, mPlottedPoint;
};

class SingleCellViewQwtCurveDataAdaptor
    : public QwtSeriesData<QPointF>
{
public:
    SingleCellViewQwtCurveDataAdaptor(SingleCellViewSimulation* pSimulation,
                                      int pWhichRepeat,
                                      SingleCellViewWidgetCurveData* pCurveData);

    virtual QRectF boundingRect() const;
    void updateSize();
    virtual size_t size() const;
    virtual QPointF sample(size_t i) const;

private:
    double sampleBvar(size_t i) const;
    double sampleStateY(size_t i) const;
    double sampleRateY(size_t i) const;
    double sampleAlgebraicY(size_t i) const;
    double sampleConstantY(size_t i) const;

    int mConstantYIndex;
    int mSampleYIndex;

    SingleCellViewSimulationResults* mSimulationResults;
    double (SingleCellViewQwtCurveDataAdaptor::* mSampleY)(size_t i) const;
    int mWhichRepeat;
    size_t mSize;
};

//==============================================================================

class SingleCellViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit SingleCellViewWidget(SingleCellViewPlugin *pPluginParent,
                                  QWidget *pParent = 0);
    ~SingleCellViewWidget();

    virtual void retranslateUi();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    void initialize(const QString &pFileName);
    bool isManaged(const QString &pFileName) const;
    void finalize(const QString &pFileName);

    QIcon fileTabIcon(const QString &pFileName) const;

    SingleCellViewSimulation * simulation() const;

    SingleCellViewContentsWidget * contentsWidget() const;

private:
    enum ErrorType {
        General,
        InvalidCellMLFile,
        InvalidSimulationEnvironment
    };

    Ui::SingleCellViewWidget *mGui;

    SingleCellViewPlugin *mPluginParent;

    SingleCellViewSimulation *mSimulation;
    QMap<QString, SingleCellViewSimulation *> mSimulations;

    QList<QPointer<SingleCellViewSimulation> > mStoppedSimulations;

    Core::ProgressBarWidget *mProgressBarWidget;

    QMap<QString, int> mProgresses;

    QMap<QString, bool> mResets;
    QMap<QString, int> mDelays;

    struct AxisSettings {
        double minX;
        double maxX;
        double minY;
        double maxY;

        double localMinX;
        double localMaxX;
        double localMinY;
        double localMaxY;
    };

    QMap<QString, AxisSettings> mAxesSettings;

    Core::ToolBarWidget *mToolBarWidget;

    QFrame *mTopSeparator;
    QFrame *mBottomSeparator;

    QwtWheel *mDelayWidget;
    QLabel *mDelayValueWidget;

    QSplitter *mSplitterWidget;
    QList<int> mSplitterWidgetSizes;

    SingleCellViewContentsWidget *mContentsWidget;

    bool mRunActionEnabled;

    Core::UserMessageWidget *mInvalidModelMessageWidget;

    QTextEdit *mOutputWidget;

    ErrorType mErrorType;

    SingleCellViewGraphPanelWidget *mActiveGraphPanel;

    QMap<QString, SingleCellViewWidgetCurveData *> mCurvesData;

    QMap<SingleCellViewSimulation*, QPair<qulonglong, qulonglong> > mOldSimulationResultsSizes;

    QList<QPointer<SingleCellViewSimulation> > mCheckResultsSimulations;

    void setDelayValue(const int &pDelayValue);

    void output(const QString &pMessage);

    void updateSimulationMode();

    int tabBarIconSize() const;

    void updateRunPauseAction(const bool &pRunActionEnabled);

    void updateInvalidModelMessageWidget();

    void updateResults(SingleCellViewSimulation *pSimulation,
                       const qulonglong &pRepeatSize,
                       const qulonglong &pSize,
                       const bool &pReplot = false);
    void checkResults(SingleCellViewSimulation *pSimulation);

    QString parameterKey(const QString pFileName,
                              QSharedPointer<CellMLSupport::CellMLFileRuntimeCompiledModelParameter>
                              pModelParameter);

private Q_SLOTS:
    void on_actionRunPauseResume_triggered();
    void on_actionStop_triggered();

    void on_actionReset_triggered();

    void on_actionDebugMode_triggered();

    void on_actionAdd_triggered();
    void on_actionRemove_triggered();

    void on_actionCsvExport_triggered();

    void updateDelayValue(const double &pDelayValue);

    void simulationRunning(QPointer<OpenCOR::SingleCellView::SingleCellViewSimulation> pSimulation,
                           bool pIsResuming);
    void simulationPaused(QPointer<OpenCOR::SingleCellView::SingleCellViewSimulation> pSimulation);
    void simulationStopped(QPointer<OpenCOR::SingleCellView::SingleCellViewSimulation>,
                           int pElapsedTime);

    void resetProgressBar();
    void resetFileTabIcon();

    void simulationError(QPointer<OpenCOR::SingleCellView::SingleCellViewSimulation> pSimulation,
                         const QString &pMessage,
                         const ErrorType &pErrorType = General);

    void simulationDataModified(QPointer<OpenCOR::SingleCellView::SingleCellViewSimulationData> pSimulationData,
                                const bool& pIsModified);

    void splitterWidgetMoved();

    void simulationPropertyChanged(Core::Property *pProperty, bool pNeedReset = true);
    void solversPropertyChanged(Core::Property *pProperty);

    void showParameter(const QString &pFileName,
                            QSharedPointer<CellMLSupport::CellMLFileRuntimeParameter> pParameter,
                       const bool &pShow);

    void callCheckResults();
};

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
