#include "session_sync.h"

#include <datasync.h>
#include <disksyncengine.h>

#include <QDebug>

SessionSync::SessionSync(UIKit::ViewController *a_controller,
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

void SessionSync::set_session_data(const QString &a_key,
                                      const QVariant &a_data) {
  m_session_data[a_key] = a_data;
}

QVariant SessionSync::session_data(const QString &a_key) const {
  if (!m_session_data.contains(a_key)) {
      return QVariant("");
  }
  return m_session_data[a_key];
}

QList<QString> SessionSync::session_keys() const {
  return m_session_data.keys();
}

void SessionSync::mark() { m_purged = true; }

bool SessionSync::is_purged() { return m_purged; }

void SessionSync::set_session_id(int a_id) { m_session_id = a_id; }

int SessionSync::session_id() { return m_session_id; }

void SessionSync::update_session()
{
  m_session_data["x"] = m_clock_session_window->pos().x();
  m_session_data["y"] = m_clock_session_window->pos().y();
}

void SessionSync::delete_session_data(const QString &a_session_name)
{
  QuetzalKit::DataSync *sync =
      new QuetzalKit::DataSync(a_session_name.toStdString());

  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();
  sync->set_sync_engine(engine);

  sync->remove_object("Clock", "clock_id",
             QString("%1").arg(m_session_id).toStdString());
  delete sync;
}

void SessionSync::save_session_attribute(const QString &a_session_name,
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
