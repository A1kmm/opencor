//==============================================================================
// CellML file exporter
//==============================================================================

#ifndef CELLMLFILEEXPORTER_H
#define CELLMLFILEEXPORTER_H

//==============================================================================

#include <QString>

//==============================================================================

#include "IfaceCellML_APISPEC.hxx"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

class CellMLFileExporter
{
public:
    explicit CellMLFileExporter();

    bool result() const;

    QString errorMessage() const;

protected:
    bool mResult;

    QString mErrorMessage;

    bool saveModel(iface::cellml_api::Model *pModel, const QString &pFileName);
};

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
