/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "timepicker.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>

class time_dialog::PrivateTimePicker {
public:
  PrivateTimePicker() {}
  ~PrivateTimePicker() {}
};

time_dialog::time_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateTimePicker) {}

time_dialog::~time_dialog() { delete o_desktop_dialog; }

void time_dialog::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  // todo: invoke UI
}

QRectF time_dialog::geometry() const { return QRectF(); }

QVariantMap time_dialog::result() const { return QVariantMap(); }

cherry_kit::window *time_dialog::activity_window() const { return 0; }

void time_dialog::cleanup() {}
