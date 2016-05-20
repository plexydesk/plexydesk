/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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

#include "ck_extension_manager.h"
#include "ck_data_plugin_interface.h"
#include "ck_desktop_plugin_interface.h"
#include "ck_dialog_plugin_interface.h"

namespace cherry_kit {
extension_manager *extension_manager::mInstance = 0;

class extension_manager::PrivateExtManager {
public:
  typedef QHash<QString, data_plugin_interface *> EnginePlugins;
  typedef QHash<QString, desktop_plugin_interface *> ControllerPlugins;
  typedef QHash<QString, dialog_plugin_interface *> ActivityPlugins;
  typedef QHash<QString, style_interface *> StylePlugins;

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

extension_manager::extension_manager(QObject *parent)
    : QObject(parent), o_extension_manager(new PrivateExtManager) {}

extension_manager::~extension_manager() {
  o_extension_manager->mStyles.clear();

  foreach(QPluginLoader * _loader, o_extension_manager->m_plugin_loader_list) {
    _loader->unload();
    delete _loader;
		qDebug() << Q_FUNC_INFO << "Deleted plugin";
  }

  o_extension_manager->m_plugin_loader_list.clear();

  delete o_extension_manager;
}

extension_manager *extension_manager::instance(const QString &a_desktopPrefix,
                                               const QString &a_prefix) {
  if (!mInstance) {
    mInstance = new extension_manager();
    mInstance->set_plugin_prefix(a_prefix);
    mInstance->set_plugin_info_prefix(a_desktopPrefix);
    mInstance->scan_for_plugins();
  }
  return mInstance;
}

extension_manager *extension_manager::init(const QString &a_desktopPrefix,
                                           const QString &a_prefix) {
  return instance(a_desktopPrefix, a_prefix);
}

extension_manager *extension_manager::instance() {
  if (!mInstance) {
    mInstance = new extension_manager();
  }
  return mInstance;
}

void extension_manager::destroy_instance() {
  if (mInstance) {
     delete mInstance;
  }

  mInstance = 0;
}

QStringList extension_manager::extension_list(const QString &a_types) {
  return o_extension_manager->mDict[a_types];
}

DataSourcePtr extension_manager::data_engine(const QString &a_name) {
  if (o_extension_manager->m_engine_plugins[a_name]) {
    return o_extension_manager->m_engine_plugins[a_name]->model();
  } else {
    load("Engine", a_name);
    if (o_extension_manager->m_engine_plugins[a_name]) {
      return o_extension_manager->m_engine_plugins[a_name]->model();
    }
  }

  return QSharedPointer<data_source>();
}

desktop_controller_ref extension_manager::controller(const QString &a_name) {
  if (o_extension_manager->mControllers[a_name]) {
    return o_extension_manager->mControllers[a_name]->controller();
  } else {
    load("Widget", a_name);
    if (o_extension_manager->mControllers[a_name]) {
      return o_extension_manager->mControllers[a_name]->controller();
    }
  }

  return desktop_controller_ref();
}

desktop_dialog_ref extension_manager::activity(const QString &a_name) {

  if (o_extension_manager->mActivities[a_name]) {
    return o_extension_manager->mActivities[a_name]->activity();
  } else {
    load("Activity", a_name);
    if (o_extension_manager->mActivities[a_name]) {
      return o_extension_manager->mActivities[a_name]->activity();
    }
  }

  return std::shared_ptr<desktop_dialog>();
}

style_ref extension_manager::style(const QString &a_name) {
  if (o_extension_manager->mStyles[a_name]) {
    return o_extension_manager->mStyles[a_name]->style();
  } else {
    load("Style", a_name);
    if (o_extension_manager->mStyles[a_name]) {
      return o_extension_manager->mStyles[a_name]->style();
    }
  }

  return style_ref();
}

QString extension_manager::desktop_controller_extension_info(
    const QString &a_key) const {
  return o_extension_manager->mPluginNames[a_key];
}

void extension_manager::load(const QString &a_interface,
                             const QString &a_plugin_name) {
#ifdef Q_OS_MAC
  QPluginLoader *loader = new QPluginLoader(
      o_extension_manager->mPluginPrefix + QLatin1String("lib") +
      a_plugin_name + QLatin1String(".dylib"));
#endif

#ifdef Q_OS_LINUX
  QPluginLoader *loader =
      new QPluginLoader(o_extension_manager->mPluginPrefix +
                        QLatin1String("lib") + a_plugin_name + ".so");
#endif

#ifdef Q_OS_WIN32
  QPluginLoader *loader = new QPluginLoader(o_extension_manager->mPluginPrefix +
                                            a_plugin_name + ".dll");
#endif

#ifdef Q_OS_QPA
  QPluginLoader *loader =
      new QPluginLoader(o_extension_manager->mPluginPrefix +
                        QLatin1String("lib") + a_plugin_name + ".so");
#endif

  if (a_interface.toLower() == "engine") {
    QObject *plugin = loader->instance();

    if (plugin) {
      data_plugin_interface *Iface = 0;
      Iface = dynamic_cast<data_plugin_interface *>(plugin);
      o_extension_manager->m_engine_plugins[a_plugin_name] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << ":" << a_interface << ":"
                 << "pluginName: " << a_plugin_name
                 << "Error: " << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (a_interface.toLower() == "widget") {

    QObject *plugin = loader->instance();

    if (plugin) {
      desktop_plugin_interface *Iface = 0;
      Iface = qobject_cast<desktop_plugin_interface *>(plugin);
      o_extension_manager->mControllers[a_plugin_name] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (a_interface.toLower() == "activity") {

    QObject *plugin = loader->instance();

    if (plugin) {
      dialog_plugin_interface *Iface = 0;
      Iface = qobject_cast<dialog_plugin_interface *>(plugin);
      o_extension_manager->mActivities[a_plugin_name] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  if (a_interface.toLower() == "style") {

    QObject *plugin = loader->instance();

    if (plugin) {
      style_interface *Iface = 0;
      Iface = qobject_cast<style_interface *>(plugin);
      o_extension_manager->mStyles[a_plugin_name] = Iface;
    } else {
      qWarning() << Q_FUNC_INFO << loader->errorString();
      loader->unload();
      delete loader;
      return;
    }
  }

  o_extension_manager->m_plugin_loader_list.append(loader);
}

void extension_manager::scan_for_plugins() {
  qDebug() << Q_FUNC_INFO << o_extension_manager->mPluginInfoPrefix << " : "
           << o_extension_manager->mPluginPrefix;
  if (o_extension_manager->mPluginInfoPrefix.isEmpty() ||
      o_extension_manager->mPluginInfoPrefix.isNull()) {
    qWarning() << Q_FUNC_INFO << "Prefix undefined"
               << " try running PluginLoader::getInstanceWithPrefix with the "
                  "correct path first";
  }

  QDir dir(o_extension_manager->mPluginInfoPrefix);
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    load_desktop(o_extension_manager->mPluginInfoPrefix + fileInfo.fileName());
  }
}

void extension_manager::set_plugin_prefix(const QString &a_path) {
  o_extension_manager->mPluginPrefix = a_path;
}

void extension_manager::set_plugin_info_prefix(const QString &a_path) {
  o_extension_manager->mPluginInfoPrefix = a_path;
}

QString extension_manager::plugin_info_prefix() const {
  return o_extension_manager->mPluginInfoPrefix;
}

QString extension_manager::plugin_prefix() const {
  return o_extension_manager->mPluginPrefix;
}

void extension_manager::load_desktop(const QString &a_path) {
  QSettings desktopFile(a_path, QSettings::IniFormat, this);
  desktopFile.beginGroup("Desktop Entry");
  o_extension_manager->addToDict(
      desktopFile.value("Type").toString(),
      desktopFile.value("X-PLEXYDESK-Library").toString());
  o_extension_manager->mPluginNames
      [desktopFile.value("X-PLEXYDESK-Library").toString()] =
      desktopFile.value("Name").toString();
  desktopFile.endGroup();
}

void
extension_manager::PrivateExtManager::addToDict(const QString &interface,
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
