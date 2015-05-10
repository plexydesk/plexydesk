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

// datakit
#include <clockwidget.h>
#include <datasync.h>
#include <disksyncengine.h>

// Qt
#include <QAction>

class _clock_session {
public:
  _clock_session(Clock *a_controller);
  ~_clock_session() {}

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void mark();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();

private:
  void delete_session_data(const QString &a_session_name);

  int m_session_id;
  QVariantMap m_session_data;
  QString m_session_database;
  bool m_purged;

  Clock *m_controller;
  UIKit::Window *m_clock_session_window;
  UIKit::Widget *m_content_view;
  UIKit::ClockWidget *m_clock_widget;
};

class Clock::PrivateClockController {
public:
  PrivateClockController() : m_clock_activity_count(0) {}
  ~PrivateClockController() {}

  void _new_session(Clock *a_controller);
  void _restore_session(Clock *a_controller);
  void _end_session(int a_id);
  void _save_session();

  UIKit::ActionList m_supported_action_list;
  int m_clock_activity_count;
  QString m_session_database_name;

  QList<_clock_session *> m_session_list;
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
      d->_restore_session(this);
    }
  });

  sync->find("Clock", "", "");
  delete sync;
}

void Clock::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  if (!a_obj)
    return;

  qDebug() << Q_FUNC_INFO << "Save session data for clock";

  a_obj->setObjectAttribute("count", d->m_session_list.count());

  QString session_name = a_obj->attributeValue("name").toString()
      + "_org.clock.data";

  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(session_name.toStdString());
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  foreach (_clock_session *session_ref, d->m_session_list) {
    qDebug() << Q_FUNC_INFO << "Check Session List";
    if (session_ref->is_purged())
      continue;

    QuetzalKit::SyncObject clock_session_obj;

    clock_session_obj.setName("Clock");
    clock_session_obj.setObjectAttribute("zone_id", "Asia/South");
    clock_session_obj.setObjectAttribute("current_zone", "Asia/South");
    clock_session_obj.setObjectAttribute("clock_id", session_ref->session_id());

    sync->add_object(clock_session_obj);
  }

  a_obj->sync();
  delete sync;
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
  _clock_session *session_ref = new _clock_session(a_controller);
  session_ref->set_session_id(m_session_list.count());
  session_ref->set_session_data("database_name", m_session_database_name);
  m_session_list << (session_ref);

  qDebug() << Q_FUNC_INFO << "New Session ID :" << m_session_list.count();

  if (a_controller->viewport()) {
      a_controller->viewport()->update_session_value(
            a_controller->controller_name(), "", "");
    }
}

void Clock::PrivateClockController::_restore_session(Clock *a_controller)
{
  _clock_session *session_ref = new _clock_session(a_controller);
  session_ref->set_session_id(m_session_list.count());
  session_ref->set_session_data("database_name", m_session_database_name);
  m_session_list << session_ref;

  qDebug() << Q_FUNC_INFO << "New Session ID :" << m_session_list.count();
  qDebug() << Q_FUNC_INFO << "New Session db name :" << m_session_database_name;

}

void Clock::PrivateClockController::_end_session(int a_id) {
  foreach (_clock_session *session_ref, m_session_list) {
    if (session_ref->session_id() == a_id) {
      session_ref->mark();
      qDebug() << Q_FUNC_INFO << " Delete from Session !";
    }
  }
}

void Clock::PrivateClockController::_save_session() {}

_clock_session::_clock_session(Clock *a_controller) :
  m_purged(0) {
  m_clock_session_window = new UIKit::Window();
  m_content_view = new UIKit::Widget(m_clock_session_window);
  m_clock_widget = new UIKit::ClockWidget(m_content_view);

  m_content_view->setGeometry(QRectF(0, 0, 240, 240 + 48));
  m_clock_widget->setGeometry(QRectF(0, 0, 240, 240));
  m_clock_session_window->set_window_title("Clock");
  m_session_id = -1;

  m_clock_session_window->set_window_content(m_content_view);

  m_clock_session_window->on_window_closed([&](const UIKit::Window *a_window) {
     QString db_name = session_data("database_name").toString();

     mark();

     if (db_name.isNull() || db_name.isEmpty()) {
         qDebug() << Q_FUNC_INFO << "Null session database name";
         return;
     }

     this->delete_session_data(db_name);
  });


  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
  }

  qDebug() << Q_FUNC_INFO << "Create a new clock";
}

void _clock_session::set_session_data(const QString &a_key,
                                      const QVariant &a_data) {
  m_session_data[a_key] = a_data;
}

QVariant _clock_session::session_data(const QString &a_key) const {
  if (!m_session_data.contains(a_key)) {
      return QVariant("");
  }
  return m_session_data[a_key];
}

QList<QString> _clock_session::session_keys() const {
  return m_session_data.keys();
}

void _clock_session::mark() { m_purged = true; }

bool _clock_session::is_purged() { return m_purged; }

void _clock_session::set_session_id(int a_id) { m_session_id = a_id; }

int _clock_session::session_id() { return m_session_id; }

void _clock_session::delete_session_data(const QString &a_session_name)
{
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());

  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->remove_object("Clock", "clock_id",
             QString("%1").arg(m_session_id).toStdString());
  delete sync;
}
