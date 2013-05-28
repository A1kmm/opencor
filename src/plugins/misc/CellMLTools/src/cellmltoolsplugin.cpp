//==============================================================================
// CellMLTools plugin
//==============================================================================

#include "cellmlfilemanager.h"
#include "cellmltoolsplugin.h"

//==============================================================================

#include <QAction>
#include <QMenu>

//==============================================================================

namespace OpenCOR {
namespace CellMLTools {

//==============================================================================

PLUGININFO_FUNC CellMLToolsPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("A plugin which gives access to various <a href=\"http://www.cellml.org/\">CellML</a>-related tools"));
    descriptions.insert("fr", QString::fromUtf8("Une extension qui donne accès à divers outils en rapport avec <a href=\"http://www.cellml.org/\">CellML</a>"));

    return new PluginInfo(PluginInfo::InterfaceVersion001,
                          PluginInfo::General,
                          PluginInfo::Miscellaneous,
                          true,
                          QStringList() << "CellMLSupport",
                          descriptions);
}

//==============================================================================

void CellMLToolsPlugin::initialize()
{
    // Create our Tools | Export To menu

    mCellmlExportToMenu = newMenu(mMainWindow, "CellmlExportTo");

    // Create our different Tools | Export To actions, and add them to our
    // Tools | Export To menu

    mExportToCellml10Action = newAction(mMainWindow);
    mExportToCellml11Action = newAction(mMainWindow);

    mCellmlExportToMenu->addAction(mExportToCellml10Action);
    mCellmlExportToMenu->addAction(mExportToCellml11Action);

    // Some connections to handle our different Tools | Export To actions

    connect(mExportToCellml10Action, SIGNAL(triggered(bool)),
            this, SLOT(exportToCellml10()));
    connect(mExportToCellml11Action, SIGNAL(triggered(bool)),
            this, SLOT(exportToCellml11()));

    // Set our settings

    mGuiSettings->addMenuAction(GuiMenuActionSettings::Tools, mCellmlExportToMenu->menuAction());
    mGuiSettings->addMenuAction(GuiMenuActionSettings::Tools);
}

//==============================================================================

void CellMLToolsPlugin::updateGui(Plugin *pViewPlugin, const QString &pFileName)
{
    // Enable/disable and show/hide our tools in case we are dealing with a
    // CellML-based view plugin

    bool toolsVisible = pViewPlugin?
                            pViewPlugin->info()->fullDependencies().contains("CellMLSupport"):
                            false;
    CellMLSupport::CellMLFile *cellmlFile = CellMLSupport::CellMLFileManager::instance()->cellmlFile(pFileName);

    mCellmlExportToMenu->menuAction()->setEnabled(toolsVisible);
    mCellmlExportToMenu->menuAction()->setVisible(toolsVisible);

    mExportToCellml10Action->setEnabled(   toolsVisible && cellmlFile
                                        && QString::fromStdWString(cellmlFile->model()->cellmlVersion()).compare(CellMLSupport::CellML_1_0));
    mExportToCellml10Action->setVisible(toolsVisible);

    mExportToCellml11Action->setEnabled(   toolsVisible && cellmlFile
                                        && QString::fromStdWString(cellmlFile->model()->cellmlVersion()).compare(CellMLSupport::CellML_1_1));
    mExportToCellml11Action->setVisible(toolsVisible);
}

//==============================================================================

void CellMLToolsPlugin::retranslateUi()
{
    // Retranslate our different Tools actions

    retranslateMenu(mCellmlExportToMenu, tr("CellML Export To"));

    retranslateAction(mExportToCellml10Action, tr("CellML 1.0..."), tr("Export the CellML file to CellML 1.0"));
    retranslateAction(mExportToCellml11Action, tr("CellML 1.1..."), tr("Export the CellML file to CellML 1.1"));
}

//==============================================================================

void CellMLToolsPlugin::exportToCellml10()
{
//---GRY--- TO BE DONE...
}

//==============================================================================

void CellMLToolsPlugin::exportToCellml11()
{
//---GRY--- TO BE DONE...
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
