/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QSettings>

#include "extensionmanager.h"
#include "dataplugininterface.h"
#include "controllerplugininterface.h"
#include "activity_interface.h"

namespace PlexyDesk {
ExtensionManager *ExtensionManager::mInstance = 0;

class ExtensionManager::PrivateExtManager {
public:
  typedef QHash<QString, DataPluginInterface *> EnginePlugins;
  typedef QHash<QString, ControllerPluginInterface *> ControllerPlugins;
  typedef QHash<QString, ActivityInterface *> ActivityPlugins;
  typedef QHash<QString, WidgetStyleInterface *> StylePlugins;

  PrivateExtManager() {}
  ~PrivateExtManager() { mDict.clear(); }

  void addToDict(const QString &interface, const QString &pluginName);

  EnginePlugins m_engine_plugins;
  ControllerPlugins mControllers;
  ActivityPlugins mActivities;
  StylePlugins mStyles;

  QString mPluginPrefix;
  QString mPluginInfoPrefix;
  QHash<QString, QStringList> mDict;
  QHash<QString, QString> mPluginNames;

  QList<QPluginLoader *> m_plugin_loader_list;
};

ExtensionManager::ExtensionManager(QObject *parent)
    : QObject(parent), d(new PrivateExtManager) {}

ExtensionManager::~ExtensionManager() {
  foreach(QPluginLoader * _loader, d->m_plugin_loader_list) {
    _loader->unload();
    delete _loader;
  }

  d->m_plugin_loader_list.clear();

  delete d;
}

ExtensionManager *ExtensionManager::instance(const QString &desktopPrefix,
                                             const QString &prefix) {
  if (!mInstance) {
    mInstance = new ExtensionManager();
    mInstance->setPluginPrefix(prefix);
    mInstance->setPluginInfoPrefix(desktopPrefix);
    mInstance->scanForPlugins();
  }
  return mInstance;
}

ExtensionManager *ExtensionManager::instance() {
  if (!mInstance) {
    mInstance = new ExtensionManager();
  }
  return mInstance;
}

void ExtensionManager::destroyInstance() {
  if (mInstance)
    mInstance->deleteLater();

  mInstance = 0;
}

QStringList ExtensionManager::extensionList(const QString &types) {
  return d->mDict[types];
}

DataSourcePtr ExtensionManager::dataEngine(const QString &name) {
  if (d->m_engine_plugins[name]) {
    return d->m_engine_plugins[name]->model();
  } else {
    load("Engine", name);
    if (d->m_engine_plugins[name]) {
      return d->m_engine_plugins[name]->model();
    }
  }

  return QSharedPointer<DataSource>();
}

ControllerPtr ExtensionManager::controller(const QString &name) {
  if (d->mControllers[name]) {
    return d->mControllers[name]->controller();
  } else {
    load("Widget", name);
    if (d->mControllers[name]) {
      return d->mControllers[name]->controller();
    }
  }

  return QSharedPointer<ViewController>();
}

DesktopActivityPtr ExtensionManager::activity(const QString &name) {

  if (d->mActivities[name]) {
    return d->mActivities[name]->activity();
  } else {
    load("Activity", name);
    if (d->mActivities[name]) {
      return d->mActivities[name]->activity();
    }
  }

  return QSharedPointer<DesktopActivity>();
}

StylePtr ExtensionManager::style(const QString &name) {
  if (d->mStyles[name]) {
    return d->mStyles[name]->style();
  } else {
    load("Style", name);
    if (d->mStyles[name]) {
      return d->mStyles[name]->style();
    }
  }

  return QSharedPointer<Style>();
}

QString ExtensionManager::desktopControllerExtensionInfo(
    const QString &key) const {
  return d->mPluginNames[key];
}

void ExtensionManager::load(const QString &interface,
                            const QString &pluginName) {
#ifdef Q_OS_MAC
  QPluginLoader *loader =
      new QPluginLoader(d->mPluginPrefix + QLatin1String("lib") + pluginName +
                        QLatin1String(".dylib"));
#endif

#ifdef Q_OS_LINUX
  QPluginLoader *loader = new QPluginLoader(
      d->mPluginPrefix + QLatin1String("lib") + pluginName + ".so");
#endif

#ifdef Q_OS_WIN
  QPluginLoader *loader =
      new QPluginLoader(d->mPluginPrefix + pluginName + ".dll");
#endif

#ifdef Q_OS_QPA
  QPluginLoader *loader = new QPluginLoader(
      d->mPluginPrefix + QLatin1String("lib") + pluginName + ".so");
#endif

  if (interface.toLower() == "engine") {
    QObject *plugin = loader->instance();

    if (plugin) {
      DataPluginInterface *Iface = 0;
      Iface = qobject_cast<DataPluginInterface *>(plugin);
      d->m_engine_plugins[pluginName] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << ":" << interface << ":"
                 << "pluginName: " << pluginName
                 << "Error: " << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (interface.toLower() == "widget") {

    QObject *plugin = loader->instance();

    if (plugin) {
      ControllerPluginInterface *Iface = 0;
      Iface = qobject_cast<ControllerPluginInterface *>(plugin);
      d->mControllers[pluginName] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (interface.toLower() == "activity") {

    QObject *plugin = loader->instance();

    if (plugin) {
      ActivityInterface *Iface = 0;
      Iface = qobject_cast<ActivityInterface *>(plugin);
      d->mActivities[pluginName] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (interface.toLower() == "style") {

    QObject *plugin = loader->instance();

    if (plugin) {
      WidgetStyleInterface *Iface = 0;
      Iface = qobject_cast<WidgetStyleInterface *>(plugin);
      d->mStyles[pluginName] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  d->m_plugin_loader_list.append(loader);
}

void ExtensionManager::scanForPlugins() {
    qDebug() << Q_FUNC_INFO << d->mPluginInfoPrefix << " : "
             << d->mPluginPrefix;
  if (d->mPluginInfoPrefix.isEmpty() || d->mPluginInfoPrefix.isNull()) {
    qWarning() << Q_FUNC_INFO << "Prefix undefined"
               << " try running PluginLoader::getInstanceWithPrefix with the "
                  "correct path first";
  }

  QDir dir(d->mPluginInfoPrefix);
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    loadDesktop(d->mPluginInfoPrefix + fileInfo.fileName());
  }
}

void ExtensionManager::setPluginPrefix(const QString &path) {
  d->mPluginPrefix = path;
}

void ExtensionManager::setPluginInfoPrefix(const QString &path) {
  d->mPluginInfoPrefix = path;
}

QString ExtensionManager::pluginInforPrefix() const {
  return d->mPluginInfoPrefix;
}

QString ExtensionManager::pluginPrefix() const { return d->mPluginPrefix; }

void ExtensionManager::loadDesktop(const QString &path) {
  QSettings desktopFile(path, QSettings::IniFormat, this);
  desktopFile.beginGroup("Desktop Entry");
  d->addToDict(desktopFile.value("Type").toString(),
               desktopFile.value("X-PLEXYDESK-Library").toString());
  d->mPluginNames[desktopFile.value("X-PLEXYDESK-Library").toString()] =
      desktopFile.value("Name").toString();
  desktopFile.endGroup();
}

void ExtensionManager::PrivateExtManager::addToDict(const QString &interface,
                                                    const QString &pluginName) {
  const QStringList dictKeys = mDict.keys();
  if (!dictKeys.contains(interface)) {
    QStringList list;
    list << pluginName;
    mDict[interface] = list;
  } else {
    QStringList list;
    list = mDict[interface];
    list << pluginName;
    mDict[interface] = list;
  }
}
}
