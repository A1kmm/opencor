#ifndef QJSONPLUGIN_H
#define QJSONPLUGIN_H

#include "plugininfo.h"

namespace OpenCOR {
namespace QJson {

PLUGININFO_FUNC QJsonPluginInfo();

class QJsonPlugin : public QObject
{
    Q_OBJECT
};

} }

#endif