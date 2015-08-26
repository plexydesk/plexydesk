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
#ifndef DESKTOPNOTES_DATA_H
#define DESKTOPNOTES_DATA_H

#include <QtCore>

#include <ck_data_source.h>
#include <ck_desktop_controller_interface.h>
#include <ck_widget.h>
#include <QtNetwork>

class desktop_task_controller_impl
    : public cherry_kit::desktop_controller_interface {
  Q_OBJECT

public:
  desktop_task_controller_impl(QObject *object = 0);
  virtual ~desktop_task_controller_impl();

  void init();

  void session_data_ready(const cherry_kit::sync_object &a_sesion_root);
  virtual void submit_session_data(cherry_kit::sync_object *a_obj);

  virtual void set_view_rect(const QRectF &rect);

  /*
  cherry_kit::ActionList actions() const;
  virtual void request_action(const QString &actionName,
                              const QVariantMap &args);
                              */

  cherry_kit::ui_action task();

  virtual void handle_drop_event(cherry_kit::widget *widget, QDropEvent *event);

public
Q_SLOTS:
  void onDataUpdated(const QVariantMap &data);

private:
  void createNoteUI(cherry_kit::session_sync *a_session);
  void createReminderUI(cherry_kit::session_sync *a_session);

  class PrivateDesktopNotes;
  PrivateDesktopNotes *const priv;
};

#endif
