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
#ifndef EXTENSION_MANAGER_H
#define EXTENSION_MANAGER_H

#include <QHash>
#include <QSharedPointer>

#include <ck_data_source.h>
#include <ck_desktop_controller_interface.h>
#include <ck_style_interface.h>

#include <ck_desktop_dialog.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
/**
    * @brief PlexyDesk Plugin Manager Class
    *
    * Supports loading and handling various plexydesk
    * extensions supported by the sytem
    */
class DECL_UI_KIT_EXPORT extension_manager : public QObject {
  Q_OBJECT
public:
  /**
      * @brief
      *
      * @param parent
      */
  extension_manager(QObject *a_parent_ptr = 0);
  /**
      * @brief
      *
      */
  virtual ~extension_manager();
  /**
      * @brief
      *
      * @param desktopPrefix
      * @param libPrefix
      * @return ExtensionManager
      */
  static extension_manager *instance(const QString &a_desktopPrefix,
                                     const QString &a_prefix);

  static extension_manager *init(const QString &a_desktopPrefix,
                                 const QString &a_prefix);

  /**
      * @brief
      *
      * @return ExtensionManager
      */
  static extension_manager *instance();

  static void destroy_instance();
  /**
      * @brief
      *
      * @param types
      * @return QStringList
      */
  QStringList extension_list(const QString &a_types);
  /**
      * @brief
      *
      * @param name
      * @return DataSourcePtr
      */
  DataSourcePtr data_engine(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return ControllerPtr
      */
  desktop_controller_ref controller(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return DesktopActivityPtr
      */
  cherry_kit::desktop_dialog_ref activity(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return StylePtr
      */
  style_ref style(const QString &a_name);
  /**
    * @brief
    *
    * @param key
    * @return QString
    */
  QString desktop_controller_extension_info(const QString &a_key) const;

private:
  /**
      * @brief
      *
      */
  void scan_for_plugins();
  /**
      * @brief
      *
      * @param path
      */
  void load_desktop(const QString &a_path);
  /**
      * @brief
      *
      * @param _interface
      * @param plugin
      */
  void load(const QString &a_interface, const QString &a_plugin_name);
  /**
      * @brief
      *
      * @param path
      */
  void set_plugin_prefix(const QString &a_path);
  /**
      * @brief
      *
      * @param path
      */
  void set_plugin_info_prefix(const QString &a_path);
  /**
      * @brief
      *
      * @return QString
      */
  QString plugin_info_prefix() const;
  /**
      * @brief
      *
      * @return QString
      */
  QString plugin_prefix() const;

  class PrivateExtManager;
  PrivateExtManager *const o_extension_manager; /**< TODO */

#ifdef Q_OS_WIN32
  static extension_manager *mInstance;
#else
  static DECL_UI_KIT_EXPORT extension_manager *mInstance; /**< TODO */
#endif
};
} // namespace PlexDesk
#endif
