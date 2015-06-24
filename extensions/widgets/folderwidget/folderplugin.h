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
#ifndef PLEXY_WIDGET_IMAGEPILE_H
#define PLEXY_WIDGET_IMAGEPILE_H

#include <plexy.h>

#include <view_controller.h>
#include <resource_manager.h>
#include "iconwidgetview.h"

class QLineEdit;
class QWidget;

class DirectoryController : public UIKit::ViewController {
  Q_OBJECT

public:
  DirectoryController(QObject *object = 0);
  virtual ~DirectoryController();

  void init();

  void session_data_available(const QuetzalKit::SyncObject &a_session_root);
  virtual void submit_session_data(QuetzalKit::SyncObject *a_obj);

  UIKit::ActionList actions() const;
  void request_action(const QString &actionName, const QVariantMap &args);
  void handle_drop_event(UIKit::Widget *widget, QDropEvent *event);
  void set_view_rect(const QRectF &rect);

  QString icon() const;

private:
  //UIKit::Theme *mThemePack;
  //QList<UIKit::Widget *> mFolderViewList;
  UIKit::ActionList m_supported_action_list;
};

#endif
