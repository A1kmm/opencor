//==============================================================================
// CellML file exporter
//==============================================================================

#include "cellmlfileexporter.h"

//==============================================================================

#include <QFile>
#include <QTextStream>

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellMLFileExporter::CellMLFileExporter() :
    mResult(false),
    mErrorMessage(QString())
{
}

//==============================================================================

bool CellMLFileExporter::result() const
{
    // Return the result of the conversion

    return mResult;
}

//==============================================================================

QString CellMLFileExporter::errorMessage() const
{
    // Return the error message following the conversion

    return mErrorMessage;
}

//==============================================================================

bool CellMLFileExporter::saveModel(iface::cellml_api::Model *pModel,
                                   const QString &pFileName)
{
    // Save the given model

    QFile file(pFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.remove();

        return false;
    } else {
        QTextStream out(&file);

        out << QString::fromStdWString(pModel->serialisedText());

        file.close();

        return true;
    }
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
