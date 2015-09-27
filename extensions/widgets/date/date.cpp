/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "date.h"
#include <QGraphicsLinearLayout>
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

class time_segment : public cherry_kit::widget {
public:
  typedef enum {
    kAMTime,
    kPMTime,
    kNoonTime
  } segment_t;
  time_segment(cherry_kit::widget *a_parent_ref = 0)
      : cherry_kit::widget(a_parent_ref), m_heighlight(0), m_duration(0) {}
  virtual ~time_segment() {}

  void set_time_value(int time_value);
  int time_value() const;

  segment_t time_type() const;
  void set_time_type(const segment_t &time_type);

  bool heighlight() const;
  void set_heighlight(bool a_is_enabled);

  cherry_kit::window *create_new();

protected:
  virtual void paint_view(QPainter *a_ctx, const QRectF &a_rect);

private:
  int m_time_value;
  int m_duration;
  segment_t m_time_type;
  bool m_heighlight;
};

typedef std::vector<time_segment *> time_segment_list_t;

class date_controller::PrivateDate {
public:
  PrivateDate() {}
  ~PrivateDate() {}
};

date_controller::date_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object), priv(new PrivateDate) {}

date_controller::~date_controller() { delete priv; }

void date_controller::init() {}

void date_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {
  revoke_previous_session(
      "Calendar", [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
        create_ui_calendar_ui(a_session);
      });
}

void date_controller::submit_session_data(cherry_kit::sync_object *a_obj) {
  write_session_data("Calendar");
}

void date_controller::set_view_rect(const QRectF &a_rect) {}

bool date_controller::remove_widget(cherry_kit::widget *a_widget_ptr) {
  return false;
}

cherry_kit::ui_action date_controller::task() {
  cherry_kit::ui_action task;
  task.set_name("Organize");
  task.set_visible(1);
  task.set_controller(controller_name().toStdString());
  task.set_icon("panel/ck_add.png");

  cherry_kit::ui_action cal_task;
  cal_task.set_name("Calendar");
  cal_task.set_visible(1);
  cal_task.set_icon("panel/ck_add.png");
  cal_task.set_controller(controller_name().toStdString());
  cal_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                           const cherry_kit::ui_task_data_t &a_data) {
    QPointF window_location;
    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
    }
    QVariantMap session_args;

    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["calendar_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("calendar"));

    start_session("Calendar", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      create_ui_calendar_ui(a_session);
    });
  });

  task.add_action(cal_task);

  return task;
}

cherry_kit::widget *
date_controller::add_action_button(cherry_kit::fixed_layout *ui, int a_row,
                                   int a_col, const std::string &a_label,
                                   const std::string &a_icon) {
  cherry_kit::widget_properties_t ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "toolbar/" + a_icon + ".png";
  return ui->add_widget(a_row, a_col, "image_button", ui_data);
}

time_segment *
date_controller::insert_time_element(cherry_kit::item_view *a_view, int a_value,
                                     int a_type) {
  cherry_kit::model_view_item *ck_model_itm = new cherry_kit::model_view_item();
  time_segment *ck_base_view = new time_segment(a_view);
  ck_base_view->set_time_value(a_value);
  ck_base_view->set_time_type((time_segment::segment_t)a_type);

  cherry_kit::label *ck_item_lbl = new cherry_kit::label(ck_base_view);
  cherry_kit::icon_button *ck_button =
      new cherry_kit::icon_button(ck_base_view);

  QString time_str = "AM";
  switch (a_type) {
  case time_segment::kAMTime:
    time_str = QString("%1 AM").arg(a_value);
    break;
  case time_segment::kPMTime:
    time_str = QString("%1 PM").arg(a_value);
    break;
  case time_segment::kNoonTime:
    time_str = "Noon";
    break;
  };

  ck_base_view->setMinimumSize(a_view->boundingRect().width(), 32);
  ck_item_lbl->set_alignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ck_item_lbl->set_text(time_str);
  ck_item_lbl->set_size(QSize(64, 32));

  ck_model_itm->set_view(ck_base_view);
  a_view->insert(ck_model_itm);

  // event handlers.
  ck_model_itm->on_view_removed([=](cherry_kit::model_view_item *a_item) {
    if (a_item && a_item->view()) {
      cherry_kit::widget *view = a_item->view();
      if (view)
        delete view;
    }
  });

  return ck_base_view;
}

void
date_controller::create_ui_calendar_ui(cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *ui = new cherry_kit::fixed_layout(window);
  cherry_kit::item_view *ck_model_view = 0;
  cherry_kit::timer *ck_timer = new cherry_kit::timer(1000 * 60);
  cherry_kit::icon_button *ck_add_button = 0;

  ui->set_content_margin(5, 5, 5, 5);
  ui->set_geometry(0, 0, 320, 480);
  ui->add_rows(3);
  ui->add_segments(0, 1);
  ui->add_segments(1, 1);
  ui->add_segments(2, 1);
  ui->set_row_height(0, "50%");
  ui->set_row_height(1, "45%");
  ui->set_row_height(2, "5%");

  cherry_kit::widget_properties_t ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data);
  ck_model_view = dynamic_cast<cherry_kit::item_view *>(
      ui->add_widget(1, 0, "model_view", ui_data));

  time_segment_list_t time_segment_list;

  time_segment_list.push_back(
      insert_time_element(ck_model_view, 12, time_segment::kAMTime));

  for (int i = 1; i <= 11; i++) {
    time_segment_list.push_back(
        insert_time_element(ck_model_view, i, time_segment::kAMTime));
  }

  time_segment_list.push_back(
      insert_time_element(ck_model_view, 12, time_segment::kNoonTime));

  for (int i = 1; i <= 11; i++) {
    time_segment_list.push_back(
        insert_time_element(ck_model_view, i, time_segment::kPMTime));
  }

  ck_add_button = dynamic_cast<cherry_kit::icon_button *>(
      add_action_button(ui, 2, 0, "", "ck_person_add"));

  window->set_window_title("Appointments");
  window->set_window_content(ui->viewport());

  ck_add_button->on_click([=]() {
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();

    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    time_segment::segment_t type;

    qDebug() << Q_FUNC_INFO << local_tm.tm_hour;
    if (local_tm.tm_hour == 12) {
      type = time_segment::kNoonTime;
    } else if (local_tm.tm_hour > 12) {
      type = time_segment::kPMTime;
    } else {
      type = time_segment::kAMTime;
    }

    std::for_each(std::begin(time_segment_list), std::end(time_segment_list),
                  [=](time_segment *a_time_seg_ref) {
      if (!a_time_seg_ref)
        return;
      int current_time = local_tm.tm_hour;

      if (current_time == 0)
        current_time = 12;

      if (type == time_segment::kPMTime)
        current_time = (local_tm.tm_hour - 12);

      if (a_time_seg_ref->time_value() == current_time &&
          a_time_seg_ref->time_type() == type) {
        cherry_kit::window *ck_app_window = a_time_seg_ref->create_new();

        ck_app_window->setPos(window->pos());
        QPointF window_pos(window->mapToScene(QPointF()));
        QRectF window_geometry(window_pos.x(), window_pos.y(),
                               window->geometry().width(),
                               window->geometry().height());

        QPointF sub_window_pos(window->mapToScene(QPointF()));
        QRectF sub_window_geometry(sub_window_pos.x(), sub_window_pos.y(),
                                   ck_app_window->geometry().width(),
                                   ck_app_window->geometry().height());

        ck_app_window->setPos(
            viewport()->center(sub_window_geometry, window_geometry,
                               cherry_kit::space::kCenterOnWindow));

        insert(ck_app_window);
      }
    });
  });
  a_session->bind_to_window(window);

  std::function<void()> ck_timeout_func = ([=]() {
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();

    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    time_segment::segment_t type;

    if (local_tm.tm_hour == 12) {
      type = time_segment::kNoonTime;
    } else if (local_tm.tm_hour > 12) {
      type = time_segment::kPMTime;
    } else {
      type = time_segment::kAMTime;
    }

    std::for_each(std::begin(time_segment_list), std::end(time_segment_list),
                  [=](time_segment *a_time_seg_ref) {
      if (!a_time_seg_ref)
        return;
      int current_time = local_tm.tm_hour;

      if (current_time == 0)
        current_time = 12;

      if (type == time_segment::kPMTime)
        current_time = (local_tm.tm_hour - 12);

      if (a_time_seg_ref->time_value() == current_time &&
          a_time_seg_ref->time_type() == type) {
        a_time_seg_ref->set_heighlight(1);
      } else {
        a_time_seg_ref->set_heighlight(0);
      }
    });
  });

  ck_timer->start_once(1000, ck_timeout_func);
  ck_timer->on_timeout(ck_timeout_func);
  ck_timer->start();

  window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(aWindow);
    ck_model_view->clear();
    ck_timer->stop();
    delete ck_timer;
    delete aWindow;
  });

  if (viewport()) {
    insert(window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}

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

  a_ctx->restore();
}

bool time_segment::heighlight() const { return m_heighlight; }

void time_segment::set_heighlight(bool a_is_enabled) {
  m_heighlight = a_is_enabled;
  update();
}

cherry_kit::window *time_segment::create_new() {
  cherry_kit::window *ck_window = new cherry_kit::window();
  cherry_kit::fixed_layout *ck_layout = new cherry_kit::fixed_layout(ck_window);
  cherry_kit::line_edit *ck_duration_editor = 0;

  ck_layout->set_verticle_spacing(5);
  ck_layout->set_content_margin(5, 5, 5, 5);
  ck_layout->set_geometry(0, 0, 240, 148);
  ck_layout->add_rows(3);
  ck_layout->add_segments(0, 4);
  ck_layout->add_segments(1, 1);
  ck_layout->add_segments(2, 3);

  ck_layout->set_row_height(0, "20%");
  ck_layout->set_row_height(1, "60%");
  ck_layout->set_row_height(2, "20%");

  ck_layout->set_segment_width(0, 0, "25%");
  ck_layout->set_segment_width(0, 1, "55%");
  ck_layout->set_segment_width(0, 2, "10%");
  ck_layout->set_segment_width(0, 3, "10%");

  cherry_kit::widget_properties_t ui_data;

  ui_data["label"] = "Duration";
  ck_layout->add_widget(0, 0, "label", ui_data);

  ui_data["text"] = "00 Minutes";

  ck_duration_editor =
      dynamic_cast<cherry_kit::line_edit *>(ck_layout->add_widget(
          0, 1, "line_edit", ui_data));
  ck_duration_editor->set_readonly(true);

  ui_data["label"] = "+";
  ck_layout->add_widget(0, 2, "button", ui_data,[=]() {
      m_duration++;

      if (m_duration >= 60)
          m_duration = 60;

      QString time_value = QString("%1").arg(m_duration);
      if (m_duration <= 9) {
          time_value = QString("0%1").arg(m_duration);
      }

      ck_duration_editor->set_text(QString("%1 Minutes").arg(time_value));
  });

  ui_data["label"] = "-";
  ck_layout->add_widget(0, 3, "button", ui_data,[=]() {
      m_duration--;
      QString time_value = QString("%1").arg(m_duration);

      if (m_duration <= 0)
          m_duration = 0;

      if (m_duration <= 9) {
          time_value = QString("0%1").arg(m_duration);
      }

      ck_duration_editor->set_text(QString("%1 Minutes").arg(time_value));
  });

  ui_data["text"] = "";
  ck_layout->add_widget(1, 0, "text_edit", ui_data);

  ui_data["label"] = "Add";
  cherry_kit::button *ck_add_btn = dynamic_cast<cherry_kit::button *>(
      ck_layout->add_widget(2, 1, "button", ui_data));

  ck_add_btn->on_click([=]() { ck_window->close(); });

  ck_window->set_window_content(ck_layout->viewport());
  ck_window->on_window_discarded([=](cherry_kit::window *a_window) {
    delete ck_window;
  });

  return ck_window;
}

void time_segment::set_time_value(int time_value) { m_time_value = time_value; }
