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
#include "clockui.h"

#include <QDebug>

ClockUI::ClockUI(SessionSync *a_sync, UIKit::ViewController *a_ctr)
    : m_session(a_sync) {
  m_controller = a_ctr;
  float window_width = 248;
  m_clock_session_window = new UIKit::Window();
  m_content_view = new UIKit::Widget(m_clock_session_window);
  m_clock_widget = new UIKit::ClockWidget(m_content_view);
  m_toolbar = new UIKit::ToolBar(m_content_view);
  m_timezone_label = new UIKit::Label(m_toolbar);
  m_timezone_label->set_size(QSizeF(window_width - 72, 32));

  if (m_session->session_keys().contains("zone_id")) {
    m_clock_widget->set_timezone_id(
        m_session->session_data("zone_id").toByteArray());
    m_timezone_label->set_label(m_session->session_data("zone_id").toString());
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

  m_session->bind_to_window(m_clock_session_window);

  m_toolbar->on_item_activated([&](const QString &a_action) {
    if (a_action == "TimeZone") {
      if (m_controller && m_controller->viewport()) {
        UIKit::Space *viewport = m_controller->viewport();
        UIKit::DesktopActivityPtr activity = viewport->create_activity(
            "timezone", "TimeZone", viewport->cursor_pos(),
            QRectF(0, 0, 240, 320.0), QVariantMap());

        activity->on_action_completed([&](const QVariantMap &a_data) {
          m_clock_widget->set_timezone_id(a_data["zone_id"].toByteArray());
          m_timezone_label->set_label(a_data["zone_id"].toString());

          QString db_name = m_session->session_data("database_name").toString();

          if (db_name.isNull() || db_name.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "Failed to save timezone";
            return;
          }

          m_session->save_session_attribute(db_name,
                                            "Clock",
                                            "clock_id",
                                            QString("%1").arg(m_session->session_id()),
                                            "zone_id",
                                            a_data["zone_id"].toString());
        });
      }
    }
  });

  if (m_controller && m_controller->viewport()) {
    m_controller->insert(m_clock_session_window);
    QPointF window_location;
    window_location.setX(m_session->session_data("x").toFloat());
    window_location.setY(m_session->session_data("y").toFloat());

    m_clock_session_window->setPos(window_location);
  }
}

ClockUI::~ClockUI() {}
