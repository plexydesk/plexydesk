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

#include "session_sync.h"

#include <datasync.h>
#include <disksyncengine.h>

#include <QDebug>


class SessionSync::PrivSessionSync {
public:
  PrivSessionSync() : m_purged(0) {}
  ~PrivSessionSync() {}

  int m_session_id;
  QVariantMap m_session_data;
  QString m_session_database;
  bool m_purged;

  UIKit::ViewController *m_controller;
  UIKit::Window *m_clock_session_window;
  UIKit::Widget *m_content_view;
  UIKit::ClockWidget *m_clock_widget;
  UIKit::ToolBar *m_toolbar;
  UIKit::Label *m_timezone_label;
};

SessionSync::SessionSync(UIKit::ViewController *a_controller,
                         const QVariantMap &a_data)
    : d(new PrivSessionSync) {

  d->m_controller = a_controller;
  d->m_session_data = a_data;

  float window_width = 248;
  d->m_clock_session_window = new UIKit::Window();
  d->m_content_view = new UIKit::Widget(d->m_clock_session_window);
  d->m_clock_widget = new UIKit::ClockWidget(d->m_content_view);
  d->m_toolbar = new UIKit::ToolBar(d->m_content_view);
  d->m_timezone_label = new UIKit::Label(d->m_toolbar);
  d->m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  if (a_data.contains("zone_id")) {
    d->m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
    d->m_timezone_label->set_label(a_data["zone_id"].toString());
  }

  d->m_content_view->setGeometry(QRectF(0, 0, window_width, window_width + 48));
  d->m_clock_widget->setGeometry(QRectF(0, 0, window_width, window_width));
  d->m_clock_session_window->set_window_title("Clock");
  d->m_session_id = -1;

  // toolbar placement.
  d->m_toolbar->set_icon_resolution("hdpi");
  d->m_toolbar->set_icon_size(QSize(32, 32));

  d->m_toolbar->add_action("TimeZone", "pd_add_new_icon", false);
  d->m_toolbar->insert_widget(d->m_timezone_label);

  d->m_toolbar->setGeometry(d->m_toolbar->frame_geometry());
  d->m_toolbar->show();
  d->m_toolbar->setPos(0, window_width);

  d->m_clock_session_window->set_window_content(d->m_content_view);

  d->m_toolbar->on_item_activated([&](const QString &a_action) {
    if (a_action == "TimeZone") {
      if (d->m_controller && d->m_controller->viewport()) {
        UIKit::Space *viewport = d->m_controller->viewport();
        UIKit::DesktopActivityPtr activity = viewport->create_activity(
            "timezone", "TimeZone", viewport->cursor_pos(),
            QRectF(0, 0, 240, 320.0), QVariantMap());

        activity->on_action_completed([&](const QVariantMap &a_data) {
          d->m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
          d->m_timezone_label->set_label(a_data["zone_id"].toString());

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

  d->m_clock_session_window->on_window_closed([&](const UIKit::Window *a_window) {
    QString db_name = session_data("database_name").toString();

    mark();
    if (db_name.isNull() || db_name.isEmpty()) {
      qDebug() << Q_FUNC_INFO << "Null session database name";
      return;
    }

    this->delete_session_data(db_name);
  });

  d->m_clock_session_window->on_window_moved([&](const QPointF &a_pos) {
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
    a_controller->insert(d->m_clock_session_window);
    QPointF window_location;

    window_location.setX(session_data("x").toFloat());
    window_location.setY(session_data("y").toFloat());

    d->m_clock_session_window->setPos(window_location);
  }

  qDebug() << Q_FUNC_INFO << "Create a new clock";
}

void SessionSync::session_init()
{

}

void SessionSync::set_session_data(const QString &a_key,
                                   const QVariant &a_data) {
  d->m_session_data[a_key] = a_data;
}

QVariant SessionSync::session_data(const QString &a_key) const {
  if (!d->m_session_data.contains(a_key)) {
    return QVariant("");
  }
  return d->m_session_data[a_key];
}

QList<QString> SessionSync::session_keys() const {
  return d->m_session_data.keys();
}

void SessionSync::mark() { d->m_purged = true; }

bool SessionSync::is_purged() { return d->m_purged; }

void SessionSync::set_session_id(int a_id) { d->m_session_id = a_id; }

int SessionSync::session_id() { return d->m_session_id; }

void SessionSync::update_session() {
  d->m_session_data["x"] = d->m_clock_session_window->pos().x();
  d->m_session_data["y"] = d->m_clock_session_window->pos().y();
}

void SessionSync::delete_session_data(const QString &a_session_name) {
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());

  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->remove_object("Clock", "clock_id",
                      QString("%1").arg(d->m_session_id).toStdString());
  delete sync;
}

void SessionSync::save_session_attribute(const QString &a_session_name,
                                         const QString &a_key,
                                         const QString &a_value) {
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      a_object.setObjectAttribute(a_key, a_value);
      qDebug() << Q_FUNC_INFO << a_object.attributeValue("clock_id");
      qDebug() << Q_FUNC_INFO << "saving timezone :" << a_value;

      sync->save_object(a_object);
    }
  });

  qDebug() << Q_FUNC_INFO << "Lookup " << d->m_session_id << " : "
           << QString("%1").arg(d->m_session_id);

  sync->find("Clock", "clock_id",
             QString("%1").arg(d->m_session_id).toStdString());

  delete sync;
}
