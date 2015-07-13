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

class BackgroundController : public cherry_kit::desktop_controller_interface {
  Q_OBJECT
public:
  BackgroundController(QObject *object = 0);
  virtual ~BackgroundController();

  void init();

  void session_data_available(const cherry::sync_object &a_session_root);
  void submit_session_data(cherry::sync_object *a_object);

  cherry_kit::ActionList actions() const;
  void request_action(const QString &actionName, const QVariantMap &data);

  void set_view_rect(const QRectF &rect);

  QString icon() const;
  QString label() const;

  void prepare_removal();

private
Q_SLOTS:
  void image_locally_available();
  void on_image_data_available();

protected:
  void handle_drop_event(cherry_kit::widget *widget, QDropEvent *event);
  void expose_platform_desktop();

private:
  void download_image_from_url(QUrl fileUrl);
  void revoke_session(const QVariantMap &args);

  void sync_image_data_to_disk(const QByteArray &data, const QString &source,
                               bool a_local_file = false);

  void sync_image_data_to_disk(const QImage &data, const QString &source,
                               bool saveLocally = false);

  void set_desktop_scale_type(DesktopWindow::DesktopScalingMode a_desktop_mode);
  void set_desktop_scale_type(const QString &a_action);

  void sync_session_data(const QString &key, const QVariant &value);

  class PrivateBackgroundController;
  PrivateBackgroundController *const o_ctr;
};

#endif // PLEXY_BACKGROUND_CONTROLLER_H
