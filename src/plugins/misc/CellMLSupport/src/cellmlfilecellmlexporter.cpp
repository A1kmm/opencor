//==============================================================================
// CellML file CellML exporter
//==============================================================================

#include "cellmlfilecellmlexporter.h"

//==============================================================================

//#include "cellml-api-cxx-support.hpp"

#include "CellMLBootstrap.hpp"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellMLFileCellMLExporter::CellMLFileCellMLExporter(iface::cellml_api::Model *pModel,
                                                   const std::wstring &pVersion) :
    CellMLFileExporter(),
    mModel(pModel)
{
    // Create an empty CellML model of the required version

    ObjRef<iface::cellml_api::CellMLBootstrap> cellmlBootstrap = CreateCellMLBootstrap();

    mExportedModel = cellmlBootstrap->createModel(pVersion);
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
