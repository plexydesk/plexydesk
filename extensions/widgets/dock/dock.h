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
#ifndef DOCK_DATA_H
#define DOCK_DATA_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <view_controller.h>
#include <widget.h>
#include <QtNetwork>
#include <abstractcellcomponent.h>
#include <viewbuilder.h>

using namespace CherryKit;

class DockControllerImpl : public CherryKit::ViewController {
  Q_OBJECT

public:
  DockControllerImpl(QObject *object = 0);

  virtual ~DockControllerImpl();

  void init();

  void session_data_available(const cherry::sync_object &a_sesion_root);
  virtual void submit_session_data(cherry::sync_object *a_obj);

  void set_view_rect(const QRectF &rect);

  CherryKit::ActionList actions() const;

  void request_action(const QString &actionName, const QVariantMap &args);

  QString icon() const;

  void createActionForController(const QString &name, const QPointF &pos);

  void createActivityForController(const QString &name);

  void previousSpace();

  void nextSpace();

  void toggleSeamless();

  void prepare_removal();

  void create_dock_action(CherryKit::HybridLayout *build, int row, int column,
                          const std::string &icon,
                          std::function<void()> a_button_action_func);
public
Q_SLOTS:

  void toggleDesktopPanel();

  void loadControllerActions(const QString &name);

  void onActivityAnimationFinished();

  void onActivityFinished();

  void removeSpace();

  void exec_dock_action(const QString &action);

  void onAddSpaceButtonClicked();

protected:
  void updatePreview();

private:
  class PrivateDock;
  PrivateDock *const o_view_controller;

  CherryKit::DesktopActivityPtr createActivity(const QString &controller_name,
                                               const QString &activity,
                                               const QString &title,
                                               const QPoint &pos,
                                               const QVariantMap &dataItem);
  QAction *createAction(int id, const QString &action_name,
                        const QString &icon_name);
};

#endif
