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
#ifndef NLP_DATA_H
#define NLP_DATA_H

#include <QtCore>
#include <plexy.h>
#include <datasource.h>
#include <view_controller.h>
#include <QtNetwork>

class NLPControllerImpl : public CherryKit::ViewController {
  Q_OBJECT

public:
  NLPControllerImpl(QObject *object = 0);
  virtual ~NLPControllerImpl();

  void init();

  void session_data_available(const ck::SyncObject &a_session_root);
  virtual void submit_session_data(ck::SyncObject *a_obj);

  void set_view_rect(const QRectF &rect);

  bool remove_widget(CherryKit::Widget *widget);

  CherryKit::ActionList actions() const;
  void request_action(const QString &actionName, const QVariantMap &args);

  QString icon() const;

private:
  class PrivateNLP;
  PrivateNLP *const o_view_controller;
};

#endif
