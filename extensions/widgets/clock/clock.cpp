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
// internal
#include "clock.h"
#include "clockwidget.h"

#include <QDebug>

// uikit
#include <view_controller.h>
#include <extensionmanager.h>
#include <toolbar.h>

// datakit
#include <clockwidget.h>
#include <datasync.h>
#include <disksyncengine.h>

// Qt
#include <QAction>
#include <label.h>

//
#include "session_sync.h"
#include "clockui.h"

class Clock::PrivateClockController {
public:
  PrivateClockController() : m_clock_activity_count(0) {}
  ~PrivateClockController() {}

  void _new_session(Clock *a_controller);
  void _restore_session(Clock *a_controller, const QVariantMap &a_data);
  void _end_session(int a_id);
  void _save_session();

  UIKit::ActionList m_supported_action_list;
  int m_clock_activity_count;
  QString m_session_database_name;

  QList<SessionSync *> m_session_list;
  QList<ClockUI *> m_clock_ui_list;
};

Clock::Clock(QObject *parent)
    : UIKit::ViewController(parent), d(new PrivateClockController) {}

Clock::~Clock() {
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete d;
}

QAction *Clock::createAction(int id, const QString &action_name,
                             const QString &icon_name) {
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 0);

  return _add_clock_action;
}

void Clock::init() {
  d->m_supported_action_list
      << createAction(1, tr("Clock"), "pd_clock_frame_icon.png");
  d->m_supported_action_list
      << createAction(2, tr("Timer"), "pd_clock_frame_icon.png");
  d->m_supported_action_list
      << createAction(3, tr("Alarm"), "pd_clock_frame_icon.png");
}

void Clock::set_view_rect(const QRectF &rect) {}

void Clock::session_data_available(
    const QuetzalKit::SyncObject &a_session_root) {

  QString session_name = a_session_root.attributeValue("name").toString()
      + "_org.clock.data";
  d->m_session_database_name = session_name;

  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(session_name.toStdString());
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      // create a new clock instance by requesting a new clock.
      //request_action("Clock", QVariantMap());
        QVariantMap session_data;
        session_data["x"] = a_object.attributeValue("x");
        session_data["y"] = a_object.attributeValue("y");
        session_data["zone_id"] = a_object.attributeValue("zone_id");
        session_data["clock_id"] = a_object.attributeValue("clock_id");
        d->_restore_session(this, session_data);
    }
  });

  sync->find("Clock", "", "");
  delete sync;
}

void Clock::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  if (!a_obj)
    return;

  a_obj->setObjectAttribute("count", d->m_session_list.count());

  QString session_name = a_obj->attributeValue("name").toString()
      + "_org.clock.data";

  foreach (SessionSync *session_ref, d->m_session_list) {
    if (session_ref->is_purged())
      continue;

    QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(session_name.toStdString());
    QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    QuetzalKit::SyncObject clock_session_obj;

    clock_session_obj.setName("Clock");
    clock_session_obj.setObjectAttribute("current_zone", "Asia/South");
    clock_session_obj.setObjectAttribute("clock_id", session_ref->session_id());
    clock_session_obj.setObjectAttribute("zone_id",
                                         session_ref->session_data(
                                           "zone_id").toString());

    clock_session_obj.setObjectAttribute("x",
                                         session_ref->session_data(
                                           "x").toString());
    clock_session_obj.setObjectAttribute("y",
                                         session_ref->session_data(
                                           "y").toString());


    sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
       if (!a_found) {
           sync->add_object(clock_session_obj);
       } else {
           sync->save_object(clock_session_obj);
       }
    });

    sync->find("Clock", "clock_id",
               QString("%1").arg(session_ref->session_id()).toStdString());
    delete sync;
  }

  a_obj->sync();
}

bool Clock::remove_widget(UIKit::Widget *widget) {
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  return 1;
}

UIKit::ActionList Clock::actions() const { return d->m_supported_action_list; }

void Clock::sync_session() {
  if (viewport()) {
    viewport()->update_session_value(controller_name(), "", "");
    qDebug() << Q_FUNC_INFO;
  }
}

void Clock::request_action(const QString &actionName, const QVariantMap &args) {
  if (actionName == tr("Clock")) {
    // todo create a new clock widget.
    d->_new_session(this);
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data) {}

void Clock::PrivateClockController::_new_session(Clock *a_controller) {

  QPointF window_location;

  if (a_controller && a_controller->viewport()) {
      const UIKit::Space *viewport = a_controller->viewport();

      window_location = viewport->center(QRectF(0, 0, 240, 240 + 48));;
  }

  QVariantMap session_args;

  session_args["x"] = window_location.x();
  session_args["y"] = window_location.y();
  session_args["database_name"] = m_session_database_name;

  SessionSync *session_ref = new SessionSync("Clock", session_args);
  session_ref->set_session_id(m_session_list.count());

  ClockUI *clock = new ClockUI(session_ref, a_controller);
  //session_ref->set_session_data("database_name", m_session_database_name);
  m_session_list << (session_ref);
  m_clock_ui_list << clock;

  if (a_controller->viewport()) {
      a_controller->viewport()->update_session_value(
            a_controller->controller_name(), "", "");
    }
}

void Clock::PrivateClockController::_restore_session(Clock *a_controller,
                                                     const QVariantMap &a_data) {
  QVariantMap session_args;

  session_args["x"] = a_data["x"];
  session_args["y"] = a_data["y"];
  session_args["zone_id"] = a_data["zone_id"];
  session_args["database_name"] = m_session_database_name;

  SessionSync *session_ref = new SessionSync("Clock", session_args);
  session_ref->set_session_id(a_data["clock_id"].toInt());

  ClockUI *clock = new ClockUI(session_ref, a_controller);

  m_session_list << session_ref;
  m_clock_ui_list << clock;
}

void Clock::PrivateClockController::_end_session(int a_id) {
  foreach (SessionSync *session_ref, m_session_list) {
    if (session_ref->session_id() == a_id) {
      session_ref->purge();
      qDebug() << Q_FUNC_INFO << " Delete from Session !";
    }
  }
}

void Clock::PrivateClockController::_save_session() {}

