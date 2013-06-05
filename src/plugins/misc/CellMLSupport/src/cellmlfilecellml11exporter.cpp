//==============================================================================
// CellML file CellML 1.1 exporter
//==============================================================================

#include "cellmlfilecellml11exporter.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellMLFileCellML11Exporter::CellMLFileCellML11Exporter(iface::cellml_api::Model *pModel,
                                                       const QString &pFileName) :
    CellMLFileCellMLExporter(pModel, L"1.1")
{
    // Save our exported model

    mResult = saveModel(mExportedModel, pFileName);
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
