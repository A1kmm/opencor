PROJECT(CellMLToolsPlugin)

# Retrieve some CellML API settings

RETRIEVE_CELLML_API_SETTINGS()

# Add the plugin

ADD_PLUGIN(CellMLTools
    SOURCES
        ../../coreinterface.cpp
        ../../guiinterface.cpp
        ../../i18ninterface.cpp
        ../../interface.cpp
        ../../fileinterface.cpp
        ../../plugin.cpp
        ../../plugininfo.cpp
        ../../pluginmanager.cpp

        src/cellmltoolsplugin.cpp
    HEADERS_MOC
        ../../plugin.h
        ../../pluginmanager.h

        src/cellmltoolsplugin.h
    INCLUDE_DIRS
        src
    PLUGIN_DEPENDENCIES
        CellMLAPI
        CellMLSupport
        Core
    QT_MODULES
        Widgets
    QT_DEPENDENCIES
        QtCore
        QtGui
        QtNetwork
        QtWidgets
    EXTERNAL_BINARY_DEPENDENCIES
        ${CELLML_API_EXTERNAL_BINARY_DEPENDENCIES}
)
