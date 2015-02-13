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
#ifndef EXTENSION_MANAGER_H
#define EXTENSION_MANAGER_H

#include <plexy.h>

#include <QHash>
#include <QSharedPointer>

#include <datasource.h>
#include <view_controller.h>
#include <widgetstyleinterface.h>

#include <desktopactivity.h>
#include <plexydesk_ui_exports.h>

namespace UIKit
{
/**
    * @brief PlexyDesk Plugin Manager Class
    *
    * Supports loading and handling various plexydesk
    * extensions supported by the sytem
    */
class DECL_UI_KIT_EXPORT ExtensionManager : public QObject
{
  Q_OBJECT
public:
  /**
      * @brief
      *
      * @param parent
      */
  ExtensionManager(QObject *parent = 0);
  /**
      * @brief
      *
      */
  virtual ~ExtensionManager();
  /**
      * @brief
      *
      * @param desktopPrefix
      * @param libPrefix
      * @return ExtensionManager
      */
  static ExtensionManager *instance(const QString &desktopPrefix,
                                    const QString &prefix);


  static ExtensionManager *init(const QString &desktopPrefix,
                                const QString &prefix);

  /**
      * @brief
      *
      * @return ExtensionManager
      */
  static ExtensionManager *instance();

  static void destroyInstance();
  /**
      * @brief
      *
      * @param types
      * @return QStringList
      */
  QStringList extensionList(const QString &types);
  /**
      * @brief
      *
      * @param name
      * @return DataSourcePtr
      */
  DataSourcePtr dataEngine(const QString &name);
  /**
      * @brief
      *
      * @param name
      * @return ControllerPtr
      */
  ViewControllerPtr controller(const QString &name);
  /**
      * @brief
      *
      * @param name
      * @return DesktopActivityPtr
      */
  UIKit::DesktopActivityPtr activity(const QString &name);
  /**
      * @brief
      *
      * @param name
      * @return StylePtr
      */
  StylePtr style(const QString &name);
  /**
    * @brief
    *
    * @param key
    * @return QString
    */
  QString desktopControllerExtensionInfo(const QString &key) const;

private:
  /**
      * @brief
      *
      */
  void scanForPlugins();
  /**
      * @brief
      *
      * @param path
      */
  void loadDesktop(const QString &path);
  /**
      * @brief
      *
      * @param _interface
      * @param plugin
      */
  void load(const QString &_interface, const QString &plugin);
  /**
      * @brief
      *
      * @param path
      */
  void setPluginPrefix(const QString &path);
  /**
      * @brief
      *
      * @param path
      */
  void setPluginInfoPrefix(const QString &path);
  /**
      * @brief
      *
      * @return QString
      */
  QString pluginInforPrefix() const;
  /**
      * @brief
      *
      * @return QString
      */
  QString pluginPrefix() const;

  class PrivateExtManager;
  PrivateExtManager *const d; /**< TODO */

#ifdef Q_OS_WIN
  static ExtensionManager *mInstance;
#else
  static PLEXYDESKCORE_EXPORT ExtensionManager *mInstance; /**< TODO */
#endif
};

} // namespace PlexDesk
#endif
