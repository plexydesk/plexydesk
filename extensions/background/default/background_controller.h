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
#ifndef PLEXY_BACKGROUND_CONTROLLER_H
#define PLEXY_BACKGROUND_CONTROLLER_H

#include <config.h>
#include <ck_desktop_controller_interface.h>
#define QT_SHAREDPOINTER_TRACK_POINTERS 1
#include <QSharedPointer>

// self
#include "classicbackgroundrender.h"
#include "desktopwindow.h"

#include <ck_ui_action.h>

class desktop_controller_impl
    : public cherry_kit::desktop_controller_interface {
  Q_OBJECT
public:
  desktop_controller_impl(QObject *object = 0);
  virtual ~desktop_controller_impl();

  void init();

  void session_data_ready(const cherry_kit::sync_object &a_session_root);
  void submit_session_data(cherry_kit::sync_object *a_object);

  cherry_kit::ui_action task() const;

  void set_view_rect(const QRectF &rect);

  void prepare_removal();
  void create_task_group() const;

private Q_SLOTS:
  void image_locally_available();
  void on_image_data_available();

protected:
  void handle_drop_event(cherry_kit::widget *widget, QDropEvent *event);
  void expose_platform_desktop() const;

private:
  void download_image_from_url(QUrl fileUrl);
  void revoke_session(const QVariantMap &args);

  void sync_image_data_to_disk(const QByteArray &data, const QString &source,
                               bool a_local_file = false);

  void sync_image_data_to_disk(const QImage &data, const QString &source,
                               bool saveLocally = false);

  void
  set_desktop_scale_type(desktop_window::DesktopScalingMode a_desktop_mode);
  void set_desktop_scale_type(const QString &a_action);

  void sync_session_data(const QString &key, const QVariant &value);

  class PrivateBackgroundController;
  PrivateBackgroundController *const o_ctr;
};

#endif // PLEXY_BACKGROUND_CONTROLLER_H
