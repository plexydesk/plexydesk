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
#ifndef PLEXY_WIDGET_CLOCK_H
#define PLEXY_WIDGET_CLOCK_H

#include <functional>

#include <ck_desktop_controller_interface.h>
#include <ck_data_source.h>
#include <QTimer>
#include "ck_clock_view.h"

class time_controller : public cherry_kit::desktop_controller_interface {
  Q_OBJECT

public:
  time_controller(QObject *a_parent_ptr = 0);
  virtual ~time_controller();

  void init();

  void set_view_rect(const QRectF &rect);
  void session_data_ready(const cherry_kit::sync_object &a_session_root);
  virtual void submit_session_data(cherry_kit::sync_object *a_obj);

  bool remove_widget(cherry_kit::widget *widget);

  cherry_kit::ActionList actions() const;
  void request_action(const QString &actionName, const QVariantMap &args);

  cherry_kit::ui_action task();

  QAction *createAction(int id, const QString &action_name,
                        const QString &icon_name);
public
Q_SLOTS:
  void onDataUpdated(const QVariantMap &data);

private:
  class PrivateClockController;
  PrivateClockController *const priv;
};

#endif
