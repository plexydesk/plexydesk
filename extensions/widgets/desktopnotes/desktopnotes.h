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

class DesktopNotesControllerImpl : public UIKit::ViewController
{
  Q_OBJECT

public:
  DesktopNotesControllerImpl(QObject *object = 0);
  virtual ~DesktopNotesControllerImpl();

  void init();

  virtual void revoke_session(const QVariantMap &args);
  virtual void set_view_rect(const QRectF &rect);

  UIKit::ActionList actions() const;

  virtual void request_action(const QString &actionName, const QVariantMap &args);
  virtual void handle_drop_event(UIKit::Widget *widget, QDropEvent *event);

  QString icon() const;

public Q_SLOTS:
  void onDataUpdated(const QVariantMap &data);

private:
  void createNoteUI();
  void createReminderUI();

  class PrivateDesktopNotes;
  PrivateDesktopNotes *const d;
};

#endif
