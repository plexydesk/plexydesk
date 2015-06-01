/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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
#ifndef TEMPLATE_DATA_H
#define TEMPLATE_DATA_H

#include <QtCore>
#include <plexy.h>
#include <datasource.h>
#include <view_controller.h>
#include <QtNetwork>

class TemplateControllerImpl : public UIKit::ViewController {
  Q_OBJECT

public:
  TemplateControllerImpl(QObject *object = 0);
  virtual ~TemplateControllerImpl();

  void init();

  void session_data_available(const QuetzalKit::SyncObject &a_session_root);
  virtual void submit_session_data(QuetzalKit::SyncObject *a_obj);

  void set_view_rect(const QRectF &rect);

  bool remove_widget(UIKit::Widget *widget);

  UIKit::ActionList actions() const;
  void request_action(const QString &actionName,
                      const QVariantMap &args);

  QString icon() const;
public
Q_SLOTS:
  void onDataUpdated(const QVariantMap &data);

private:
  class PrivateTemplate;
  PrivateTemplate *const d;
};

#endif
