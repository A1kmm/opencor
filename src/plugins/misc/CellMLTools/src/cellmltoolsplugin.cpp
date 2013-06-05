//==============================================================================
// CellMLTools plugin
//==============================================================================

#include "cellmlfilemanager.h"
#include "cellmltoolsplugin.h"
#include "coreutils.h"

//==============================================================================

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>

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

    mCellMLFileExportToMenu = newMenu(mMainWindow, "CellMLFileExportTo");

    // Create our different Tools | Export To actions, and add them to our
    // Tools | Export To menu

    mExportToCellML10Action = newAction(mMainWindow);
    mExportToCellML11Action = newAction(mMainWindow);

    mCellMLFileExportToMenu->addAction(mExportToCellML10Action);
    mCellMLFileExportToMenu->addAction(mExportToCellML11Action);

    // Some connections to handle our different Tools | Export To actions

    connect(mExportToCellML10Action, SIGNAL(triggered(bool)),
            this, SLOT(exportToCellML10()));
    connect(mExportToCellML11Action, SIGNAL(triggered(bool)),
            this, SLOT(exportToCellML11()));

    // Set our settings

    mGuiSettings->addMenuAction(GuiMenuActionSettings::Tools, mCellMLFileExportToMenu->menuAction());
    mGuiSettings->addMenuAction(GuiMenuActionSettings::Tools);
}

//==============================================================================

void CellMLToolsPlugin::initializationsDone(const Plugins &pLoadedPlugins)
{
    // Retrieve the file types supported by the CellMLSupport plugin

    mCellMLFileTypes = FileTypes();

    foreach (Plugin *loadedPlugin, pLoadedPlugins) {
        FileInterface *fileInterface = qobject_cast<FileInterface *>(loadedPlugin->instance());

        if (!loadedPlugin->name().compare("CellMLSupport") && fileInterface) {
            // This is the CellMLSupport plugin and, as expected, it implements
            // the file interface, so retrieve the file types it supports

            mCellMLFileTypes = fileInterface->fileTypes();

            break;
        }
    }
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

    mCellMLFileExportToMenu->menuAction()->setEnabled(toolsVisible);
    mCellMLFileExportToMenu->menuAction()->setVisible(toolsVisible);

    mExportToCellML10Action->setEnabled(   toolsVisible && cellmlFile
                                        && QString::fromStdWString(cellmlFile->model()->cellmlVersion()).compare(CellMLSupport::CellML_1_0));
    mExportToCellML10Action->setVisible(toolsVisible);

/*---GRY---
    mExportToCellML11Action->setEnabled(   toolsVisible && cellmlFile
                                        && QString::fromStdWString(cellmlFile->model()->cellmlVersion()).compare(CellMLSupport::CellML_1_1));
    mExportToCellML11Action->setVisible(toolsVisible);
*/
//---GRY--- BEGIN
// THIS IS UNTIL EXPORTING TO CellML 1.1 IS FULLY SUPPORTED...
mExportToCellML11Action->setEnabled(false);
mExportToCellML11Action->setVisible(false);
//---GRY--- END

    // Keep track of the file name

    mFileName = pFileName;
}

//==============================================================================

void CellMLToolsPlugin::retranslateUi()
{
    // Retranslate our different Tools actions

    retranslateMenu(mCellMLFileExportToMenu, tr("CellML File Export To"));

    retranslateAction(mExportToCellML10Action, tr("CellML 1.0..."), tr("Export the CellML file to CellML 1.0"));
    retranslateAction(mExportToCellML11Action, tr("CellML 1.1..."), tr("Export the CellML file to CellML 1.1"));
}

//==============================================================================

void CellMLToolsPlugin::exportTo(const CellMLSupport::CellMLFile::Format &pFormat)
{
    // Ask for the name of the file which will contain the export

    QString format;
    QString fileTypes;

    switch (pFormat) {
    case CellMLSupport::CellMLFile::CellML_1_1:
    default:   // CellMLSupport::CellMLFile::CellML_1_0
        if (pFormat == CellMLSupport::CellMLFile::CellML_1_0)
            format = "CellML 1.0";
        else
            format = "CellML 1.1";

        foreach (const FileType &fileType, mCellMLFileTypes) {
            if (!fileTypes.isEmpty())
                fileTypes += ";;";

            fileTypes +=  fileType.description()+" (*."+fileType.fileExtension()+")";
        }
    }

    QString fileName = Core::getSaveFileName(tr("CellML file export to %1").arg(format), mFileName, fileTypes);

    // Make sure that we have a file name or leave, if not

    if (fileName.isEmpty())
        return;

    // Now that we have a file name, we can do the eport itself

    CellMLSupport::CellMLFile *cellmlFile = CellMLSupport::CellMLFileManager::instance()->cellmlFile(mFileName);

    if (!cellmlFile->exportTo(fileName, pFormat)) {
        CellMLSupport::CellMLFileIssues issues = cellmlFile->issues();
        QString errorMessage = QString();

        if (issues.count())
            errorMessage = " ("+issues.first().message()+")";
            // Note: if there are 'issues', then there can be only one of them
            //       following a CellML export...

        QMessageBox::warning(mMainWindow, tr("CellML file export to %1").arg(format),
                             tr("Sorry, but <strong>%1</strong> could not be exported to <strong>%2</strong>%3.").arg(fileName, format, errorMessage));
    }
}

//==============================================================================

void CellMLToolsPlugin::exportToCellML10()
{
    // Export the current file to CellML 1.0

    exportTo(CellMLSupport::CellMLFile::CellML_1_0);
}

//==============================================================================

void CellMLToolsPlugin::exportToCellML11()
{
    // Export the current file to CellML 1.1

    exportTo(CellMLSupport::CellMLFile::CellML_1_1);
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
