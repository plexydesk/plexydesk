#include "time_segment.h"

#include <ck_calendar_view.h>
#include <ck_item_view.h>
#include <ck_ToolBar.h>
#include <ck_label.h>
#include <ck_config.h>
#include <ck_session_sync.h>
#include <ck_icon_button.h>
#include <ck_resource_manager.h>
#include <ck_window.h>
#include <ck_fixed_layout.h>
#include <ck_model_view_item.h>
#include <ck_timer.h>

#include <ctime>
#include <chrono>
#include <ck_button.h>
#include <ck_line_edit.h>
#include <ck_text_editor.h>

#include "date_controller.h"

#include "time_event.h"

int time_segment::time_value() const { return m_time_value; }
time_segment::segment_t time_segment::time_type() const { return m_time_type; }

void time_segment::set_time_type(const segment_t &time_type) {
  m_time_type = time_type;
}

void time_segment::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  // a_ctx->fillRect(a_rect, QColor("#ffffff"));
  // todo:
  // this should be moved to style class.
  a_ctx->save();
  a_ctx->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                            QPainter::SmoothPixmapTransform |
                            QPainter::HighQualityAntialiasing,
                        true);

  if (heighlight()) {
    QPainterPath dot_path;
    dot_path.addEllipse(QPointF(64, a_rect.height() / 2), 5, 5);
    a_ctx->fillPath(dot_path, QColor(255, 0, 0));

    a_ctx->setPen(
        QPen(QColor(255, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  } else {
    a_ctx->setPen(
        QPen(QColor("#9E9E9E"), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
  }

  a_ctx->drawLine(QPointF(64, a_rect.height() / 2),
                  QPointF(a_rect.width(), a_rect.height() / 2));

  // draw segments.
  int unit_length = (a_rect.width() - 64) / 60;

  std::for_each(std::begin(m_events), std::end(m_events),
                [&](time_event a_event) {
    a_ctx->save();
    a_ctx->setOpacity(0.5);
    // a_ctx->fillRect(a_rect, QColor(0, 0, 255));
    // qDebug() << Q_FUNC_INFO << "Size :" << a_event.duration() * unit_length;
    a_ctx->setPen(
        QPen(QColor(0, 0, 255), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    a_ctx->drawLine(
        QPointF(64, a_rect.height() / 2),
        QPointF((a_event.duration() * unit_length) + 64, a_rect.height() / 2));
    a_ctx->restore();
  });
  a_ctx->restore();
}

int time_segment::day() const
{
    return m_day;
}

void time_segment::set_day(int day)
{
    m_day = day;
}

int time_segment::month() const
{
    return m_month;
}

void time_segment::set_month(int month)
{
    m_month = month;
}

int time_segment::year() const
{
    return m_year;
}

void time_segment::set_year(int year)
{
    m_year = year;
}

bool time_segment::heighlight() const { return m_heighlight; }

void time_segment::set_heighlight(bool a_is_enabled) {
  m_heighlight = a_is_enabled;
  update();
}

void time_segment::add_event(const time_event &a_event) {
  m_events.push_back(a_event);
  update();
}

event_list_t time_segment::events() const { return m_events; }

void time_segment::create_new(new_event_callback_t a_callback) {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_layout = new cherry_kit::fixed_layout(ck_window);
  cherry_kit::line_edit *ck_duration_editor = 0;
  cherry_kit::line_edit *ck_end_time_editor = 0;
  cherry_kit::text_editor *ck_note_editor = 0;

  ck_window->set_window_title("Add Appointment");

  ck_layout->set_verticle_spacing(5);
  ck_layout->set_content_margin(5, 5, 5, 5);
  ck_layout->set_geometry(0, 0, 320, 240);

  ck_layout->add_rows(4);

  ck_layout->add_segments(0, 4);
  ck_layout->add_segments(1, 4);
  ck_layout->add_segments(2, 1);
  ck_layout->add_segments(3, 3);

  ck_layout->set_row_height(0, "10%");
  ck_layout->set_row_height(1, "10%");
  ck_layout->set_row_height(2, "70%");
  ck_layout->set_row_height(3, "10%");

  ck_layout->set_segment_width(0, 0, "25%");
  ck_layout->set_segment_width(0, 1, "55%");
  ck_layout->set_segment_width(0, 2, "10%");
  ck_layout->set_segment_width(0, 3, "10%");

  ck_layout->set_segment_width(1, 0, "25%");
  ck_layout->set_segment_width(1, 1, "55%");
  ck_layout->set_segment_width(1, 2, "10%");
  ck_layout->set_segment_width(1, 3, "10%");

  time_event *event = new time_event();

  cherry_kit::widget_properties_t ui_data;

  ui_data["text"] = "";
  ck_note_editor = dynamic_cast<cherry_kit::text_editor *>(
      ck_layout->add_widget(2, 0, "text_edit", ui_data, [=]() {}));

  /* start adding first row of the layout */
  ui_data["label"] = "Start";
  ck_layout->add_widget(0, 0, "label", ui_data, [=]() {});

  ui_data["text"] = std::to_string(m_time_value) + ":00";

  ck_duration_editor = dynamic_cast<cherry_kit::line_edit *>(
      ck_layout->add_widget(0, 1, "line_edit", ui_data, [=]() {}));
  ck_duration_editor->set_readonly(true);

  ui_data["label"] = "+";
  ck_layout->add_widget(0, 2, "button", ui_data, [=]() {
    m_duration++;

    if (m_duration >= 60)
      m_duration = 60;

    event->set_duration(m_duration);
    QString time_value = QString("%1").arg(m_duration);
    if (m_duration <= 9) {
      time_value = QString("0%1").arg(m_duration);
    }

    ck_duration_editor->set_text(
        QString("%1:%2").arg(m_time_value).arg(time_value));
  });

  ui_data["label"] = "-";
  ck_layout->add_widget(0, 3, "button", ui_data, [=]() {
    m_duration--;
    QString time_value = QString("%1").arg(m_duration);

    if (m_duration <= 0)
      m_duration = 0;

    event->set_duration(m_duration);
    if (m_duration <= 9) {
      time_value = QString("0%1").arg(m_duration);
    }

    ck_duration_editor->set_text(
        QString("%1:%2").arg(m_time_value).arg(time_value));
  });

  /* end adding the first row */
  ui_data["label"] = "End";
  ck_layout->add_widget(1, 0, "label", ui_data, [=]() {});

  ui_data["text"] = std::to_string(m_time_value) + ":00";

  ck_end_time_editor = dynamic_cast<cherry_kit::line_edit *>(
      ck_layout->add_widget(1, 1, "line_edit", ui_data, [=]() {}));
  ck_end_time_editor->set_readonly(true);

  ui_data["label"] = "+";
  ck_layout->add_widget(1, 2, "button", ui_data, [=]() {
    m_duration++;

    if (m_duration >= 60)
      m_duration = 60;

    event->set_duration(m_duration);
    QString time_value = QString("%1").arg(m_duration);
    if (m_duration <= 9) {
      time_value = QString("0%1").arg(m_duration);
    }

    ck_end_time_editor->set_text(
        QString("%1:%2").arg(m_time_value).arg(time_value));
  });

  ui_data["label"] = "-";
  ck_layout->add_widget(1, 3, "button", ui_data, [=]() {
    m_duration--;
    QString time_value = QString("%1").arg(m_duration);

    if (m_duration <= 0)
      m_duration = 0;

    event->set_duration(m_duration);
    if (m_duration <= 9) {
      time_value = QString("0%1").arg(m_duration);
    }

    ck_end_time_editor->set_text(
        QString("%1:%2").arg(m_time_value).arg(time_value));
  });

  //////
  ui_data["label"] = "Add";
  cherry_kit::button *ck_add_btn = dynamic_cast<cherry_kit::button *>(
      ck_layout->add_widget(3, 1, "button", ui_data, [=]() {}));

  ck_add_btn->on_click([=]() {
    event->set_note(ck_note_editor->text().toStdString());
    event->set_name("None");

    // save event to disk
    int session_id = m_controller->event_count();
    m_controller->new_event_store(
        "timeline", "true", session_id,
        [=](cherry_kit::desktop_controller_interface *x,
            cherry_kit::session_sync *a_session) {

          int session_id = m_controller->event_count();

          m_controller->save_to_store(a_session, "name", event->name(),
                                      session_id);
          m_controller->save_to_store(a_session, "note", event->note(),
                                      session_id);
          m_controller->save_to_store(a_session, "start",
                                      std::to_string(m_time_value), session_id);
          m_controller->save_to_store(a_session, "duration",
                                      std::to_string(m_duration), session_id);
          m_controller->save_to_store(a_session, "year",
                                      std::to_string(m_year), session_id);
          m_controller->save_to_store(a_session, "month",
                                      std::to_string(m_month), session_id);
          m_controller->save_to_store(a_session, "day",
                                      std::to_string(m_day), session_id);

          add_event(*event);
          delete event;

          m_duration = 0;
          ck_window->close();

          m_controller->update_event_count();

          delete a_session;
        });
  });

  ck_window->set_window_content(ck_layout->viewport());
  ck_window->on_window_discarded([=](cherry_kit::window *a_window) {
    delete ck_window;
  });

  if (a_callback)
    a_callback(ck_window);
}

void time_segment::set_time_value(int time_value) { m_time_value = time_value; }
