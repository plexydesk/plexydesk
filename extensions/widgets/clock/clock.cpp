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

class _clock_session {
public:
  _clock_session(Clock *a_controller,
                 const QVariantMap &a_data = QVariantMap());
  ~_clock_session() {}

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void mark();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();

  void update_session();
private:
  void delete_session_data(const QString &a_session_name);
  void save_session_attribute(const QString &a_session_name,
                              const QString &a_key,
                              const QString &a_value);

  int m_session_id;
  QVariantMap m_session_data;
  QString m_session_database;
  bool m_purged;

  Clock *m_controller;
  UIKit::Window *m_clock_session_window;
  UIKit::Widget *m_content_view;
  UIKit::ClockWidget *m_clock_widget;
  UIKit::ToolBar *m_toolbar;
  UIKit::Label *m_timezone_label;
};

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

  foreach (_clock_session *session_ref, d->m_session_list) {
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

  _clock_session *session_ref = new _clock_session(a_controller, session_args);
  session_ref->set_session_id(m_session_list.count());
  //session_ref->set_session_data("database_name", m_session_database_name);
  m_session_list << (session_ref);

  if (a_controller->viewport()) {
      a_controller->viewport()->update_session_value(
            a_controller->controller_name(), "", "");
    }
}

void Clock::PrivateClockController::_restore_session(Clock *a_controller,
                                                     const QVariantMap &a_data)
{
  QVariantMap session_args;

  session_args["x"] = a_data["x"];
  session_args["y"] = a_data["y"];
  session_args["zone_id"] = a_data["zone_id"];
  session_args["database_name"] = m_session_database_name;

  _clock_session *session_ref = new _clock_session(a_controller, session_args);

  session_ref->set_session_id(m_session_list.count());

  //session_ref->set_session_data("database_name", m_session_database_name);
  //session_ref->set_session_data("x", a_data["x"]);
  //session_ref->set_session_data("y", a_data["y"]);
  m_session_list << session_ref;
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

_clock_session::_clock_session(Clock *a_controller,
                               const QVariantMap &a_data) :
  m_purged(0) {
  m_controller = a_controller;
  m_session_data = a_data;
  float window_width = 248;
  m_clock_session_window = new UIKit::Window();
  m_content_view = new UIKit::Widget(m_clock_session_window);
  m_clock_widget = new UIKit::ClockWidget(m_content_view);
  m_toolbar = new UIKit::ToolBar(m_content_view);
  m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  if (a_data.contains("zone_id")) {
      m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
      m_timezone_label->set_label(a_data["zone_id"].toString());
  }

  m_content_view->setGeometry(QRectF(0, 0, window_width, window_width + 48));
  m_clock_widget->setGeometry(QRectF(0, 0, window_width, window_width));
  m_clock_session_window->set_window_title("Clock");
  m_session_id = -1;

  //toolbar placement.
  m_toolbar->set_icon_resolution("hdpi");
  m_toolbar->set_icon_size(QSize(32, 32));

  m_toolbar->add_action("TimeZone", "pd_add_new_icon", false);
  m_toolbar->insert_widget(m_timezone_label);

  m_toolbar->setGeometry(m_toolbar->frame_geometry());
  m_toolbar->show();
  m_toolbar->setPos(0, window_width);

  m_clock_session_window->set_window_content(m_content_view);

  m_toolbar->on_item_activated([&](const QString &a_action){
      if (a_action == "TimeZone") {
          if (m_controller && m_controller->viewport()) {
              UIKit::Space *viewport = m_controller->viewport();
              UIKit::DesktopActivityPtr activity = viewport->create_activity(
                                                     "timezone", "TimeZone",
                                                     viewport->cursor_pos(),
                                                     QRectF(0, 0, 240, 320.0),
                                                     QVariantMap());

              activity->on_action_completed([&](const QVariantMap &a_data) {
                  m_clock_widget->set_timezone_id(
                        a_data["zone_id"].toByteArray());
                  m_timezone_label->set_label(
                        a_data["zone_id"].toString());

                  QString db_name = session_data("database_name").toString();


                  if (db_name.isNull() || db_name.isEmpty()) {
                      qDebug() << Q_FUNC_INFO << "Failed to save timezone";
                      return;
                  }

                  save_session_attribute(db_name, "zone_id",
                                         a_data["zone_id"].toString());
              });
          }
      }
  });

  m_clock_session_window->on_window_closed([&](const UIKit::Window *a_window) {
     QString db_name = session_data("database_name").toString();

     mark();
     if (db_name.isNull() || db_name.isEmpty()) {
         qDebug() << Q_FUNC_INFO << "Null session database name";
         return;
     }

     this->delete_session_data(db_name);
  });

  m_clock_session_window->on_window_moved([&](const QPointF &a_pos) {
      update_session();
      QString db_name = session_data("database_name").toString();


      if (db_name.isNull() || db_name.isEmpty()) {
         qDebug() << Q_FUNC_INFO << "Null session database name";
         return;
      }

      save_session_attribute(db_name, "x", session_data("x").toString());
      save_session_attribute(db_name, "y", session_data("y").toString());
  });

  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
    QPointF window_location;

    window_location.setX(session_data("x").toFloat());
    window_location.setY(session_data("y").toFloat());

    m_clock_session_window->setPos(window_location);
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

void _clock_session::update_session()
{
  m_session_data["x"] = m_clock_session_window->pos().x();
  m_session_data["y"] = m_clock_session_window->pos().y();
}

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

void _clock_session::save_session_attribute(const QString &a_session_name,
                                            const QString &a_key,
                                            const QString &a_value)
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync(
        a_session_name.toStdString());
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
        /*
        a_object.setObjectAttribute("x", session_data("x").toString());
        a_object.setObjectAttribute("y", session_data("y").toString());
        a_object.setObjectAttribute("zone_id", session_data("zone_id").t);
        */

        a_object.setObjectAttribute(a_key, a_value);

        qDebug() << Q_FUNC_INFO << a_object.attributeValue("clock_id");
        qDebug() << Q_FUNC_INFO << "saving timezone :" << a_value;

        sync->save_object(a_object);
    }
  });

  qDebug() << Q_FUNC_INFO << "Lookup "
           << m_session_id << " : "
           << QString("%1").arg(m_session_id);

  sync->find("Clock", "clock_id",
             QString("%1").arg(m_session_id).toStdString());

  delete sync;
}
