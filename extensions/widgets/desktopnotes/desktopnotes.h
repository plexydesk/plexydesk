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
#ifndef DESKTOPNOTES_DATA_H
#define DESKTOPNOTES_DATA_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <view_controller.h>
#include <widget.h>
#include <QtNetwork>

class DesktopNotesControllerImpl : public CherryKit::ViewController {
  Q_OBJECT

public:
  DesktopNotesControllerImpl(QObject *object = 0);
  virtual ~DesktopNotesControllerImpl();

  void init();

  void session_data_available(const QuetzalKit::SyncObject &a_sesion_root);
  virtual void submit_session_data(QuetzalKit::SyncObject *a_obj);

  virtual void set_view_rect(const QRectF &rect);

  CherryKit::ActionList actions() const;
  virtual void request_action(const QString &actionName,
                              const QVariantMap &args);
  virtual void handle_drop_event(CherryKit::Widget *widget, QDropEvent *event);

  QString icon() const;

public
Q_SLOTS:
  void onDataUpdated(const QVariantMap &data);

private:
  void createNoteUI(CherryKit::SessionSync *a_session);
  void createReminderUI(CherryKit::SessionSync *a_session);

  class PrivateDesktopNotes;
  PrivateDesktopNotes *const o_view_controller;
};

#endif
