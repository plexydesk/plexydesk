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
#include <space.h>

// datakit
#include <clockwidget.h>
#include <datasync.h>
#include <disksyncengine.h>

// Qt
#include <QAction>
#include <label.h>

//
#include "session_sync.h"

class Clock::PrivateClockController {
 public:
  PrivateClockController() : m_clock_activity_count(0) {}
  ~PrivateClockController() {
    qDeleteAll(m_session_list);
    m_session_list.clear();
  }

  void _start_clock_session(Clock* a_controller,
                            const QVariantMap& a_data,
                            bool a_restore = false);
  void _create_clock_ui(Clock* a_controller, SessionSync* a_session);

  UIKit::ActionList m_supported_action_list;
  int m_clock_activity_count;
  QString m_session_database_name;

  QList<SessionSync*> m_session_list;
};

Clock::Clock(QObject* parent)
    : UIKit::ViewController(parent), d(new PrivateClockController) {}

Clock::~Clock() {
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete d;
}

QAction* Clock::createAction(int id,
                             const QString& action_name,
                             const QString& icon_name) {
  QAction* _add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 0);

  return _add_clock_action;
}

void Clock::init() {
  d->m_supported_action_list << createAction(
                                    1, tr("Clock"), "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(
                                    2, tr("Timer"), "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(
                                    3, tr("Alarm"), "pd_clock_frame_icon.png");
}

void Clock::set_view_rect(const QRectF& rect) {}

QString Clock::session_database_name() {
  UIKit::Space* view = viewport();
  QString session_name = view->session_name_for_controller(controller_name()) +
                         "_org." + controller_name() + ".data";

  return session_name;
}

void Clock::revoke_previous_session(
    const std::string& a_session_object_name,
    std::function<void(const QVariantMap&)> a_callback) {

  QuetzalKit::DataSync* sync =
      new QuetzalKit::DataSync(session_database_name().toStdString());
  QuetzalKit::DiskSyncEngine* engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject& a_object,
                            const std::string& a_app_name,
                            bool a_found) {
    if (a_found) {
      QVariantMap session_data;

      Q_FOREACH(const QString & a_key, a_object.attributes()) {
        session_data[a_key] = a_object.attributeValue(a_key);
      }

      if (a_callback)
        a_callback(session_data);
    }
  });

  sync->find(a_session_object_name, "", "");
  delete sync;
}

void Clock::session_data_available(
    const QuetzalKit::SyncObject& a_session_root) {
  revoke_previous_session("Clock", [this](const QVariantMap& a_session_data) {
    d->_start_clock_session(this, a_session_data, true);
  });
}

void Clock::submit_session_data(QuetzalKit::SyncObject* a_obj) {
  if (!a_obj)
    return;

  a_obj->setObjectAttribute("count", d->m_session_list.count());

  QString session_name = session_database_name();

  foreach(SessionSync * session_ref, d->m_session_list) {
    if (session_ref->is_purged())
      continue;

    QuetzalKit::DataSync* sync =
        new QuetzalKit::DataSync(session_name.toStdString());
    QuetzalKit::DiskSyncEngine* engine = new QuetzalKit::DiskSyncEngine();
    sync->set_sync_engine(engine);

    session_ref->update_session();
    QuetzalKit::SyncObject clock_session_obj;

    clock_session_obj.setName("Clock");
    Q_FOREACH(const QString & a_key, session_ref->session_keys()) {
      clock_session_obj.setObjectAttribute(a_key,
                                           session_ref->session_data(a_key));
    }

    sync->on_object_found([&](QuetzalKit::SyncObject& a_object,
                              const std::string& a_app_name,
                              bool a_found) {
      if (!a_found) {
        sync->add_object(clock_session_obj);
      } else {
        sync->save_object(clock_session_obj);
      }
    });

    sync->find(
        "Clock", "clock_id", session_ref->session_id_to_string().toStdString());
    delete sync;
  }

  a_obj->sync();
}

bool Clock::remove_widget(UIKit::Widget* widget) {
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

void Clock::request_action(const QString& actionName, const QVariantMap& args) {
  if (actionName == tr("Clock")) {
    QPointF window_location;

    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
      ;
    }

    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["clock_id"] = d->m_session_list.count();
    session_args["database_name"] = session_database_name();

    d->_start_clock_session(this, session_args, false);
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap& data) {}

void Clock::PrivateClockController::_start_clock_session(
    Clock* a_controller,
    const QVariantMap& a_data,
    bool a_restore) {
  SessionSync* session_ref = new SessionSync("Clock", a_data);
  session_ref->set_session_id(a_data["clock_id"].toInt());

  std::function<void()> bind_func =
      std::bind(&Clock::PrivateClockController::_create_clock_ui,
                this,
                a_controller,
                session_ref);
  session_ref->on_session_init(bind_func);

  m_session_list << (session_ref);
  session_ref->session_init();

  if (a_controller->viewport() && a_restore == false) {
    a_controller->viewport()->update_session_value(
        a_controller->controller_name(), "", "");
  }
}

void Clock::PrivateClockController::_create_clock_ui(Clock* a_controller,
                                                     SessionSync* a_session) {
  float window_width = 248;

  UIKit::Window* m_clock_session_window = new UIKit::Window();
  UIKit::Widget* m_content_view = new UIKit::Widget(m_clock_session_window);
  UIKit::ClockWidget* m_clock_widget = new UIKit::ClockWidget(m_content_view);
  UIKit::ToolBar* m_toolbar = new UIKit::ToolBar(m_content_view);
  UIKit::Label* m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  if (a_session->session_keys().contains("zone_id")) {
    m_clock_widget->set_timezone_id(
        a_session->session_data("zone_id").toByteArray());
    m_timezone_label->set_label(a_session->session_data("zone_id").toString());
  }

  m_content_view->setGeometry(QRectF(0, 0, window_width, window_width + 48));
  m_clock_widget->setGeometry(QRectF(0, 0, window_width, window_width));
  m_clock_session_window->set_window_title("Clock");

  // toolbar placement.
  m_toolbar->set_icon_resolution("hdpi");
  m_toolbar->set_icon_size(QSize(24, 24));

  m_toolbar->add_action("TimeZone", "actions/pd_location", false);
  m_toolbar->insert_widget(m_timezone_label);

  m_toolbar->setGeometry(m_toolbar->frame_geometry());
  m_toolbar->show();
  m_toolbar->setPos(0, window_width);

  m_clock_session_window->set_window_content(m_content_view);

  a_session->bind_to_window(m_clock_session_window);

  m_toolbar->on_item_activated([=](const QString& a_action) {
    if (a_action == "TimeZone") {
      if (a_controller && a_controller->viewport()) {
        UIKit::Space* viewport = a_controller->viewport();
        UIKit::DesktopActivityPtr activity =
            viewport->create_activity("timezone",
                                      "TimeZone",
                                      viewport->cursor_pos(),
                                      QRectF(0, 0, 240, 320.0),
                                      QVariantMap());

        activity->on_action_completed([=](const QVariantMap& a_data) {
          m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
          m_timezone_label->set_label(a_data["zone_id"].toString());

          QString db_name = a_session->session_data("database_name").toString();

          if (db_name.isNull() || db_name.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "Failed to save timezone";
            return;
          }

          a_session->save_session_attribute(
              db_name,
              "Clock",
              "clock_id",
              QString("%1").arg(a_session->session_id()),
              "zone_id",
              a_data["zone_id"].toString());
        });
      }
    }
  });

  if (a_controller && a_controller->viewport()) {
    a_controller->insert(m_clock_session_window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    m_clock_session_window->setPos(window_location);
  }
}
