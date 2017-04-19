#include "event_browser_ui.h"

#include <ck_button.h>
#include <ck_fixed_layout.h>
#include <ck_icon_button.h>
#include <ck_label.h>
#include <ck_timer.h>
#include <ck_window.h>
#include <ck_calendar_view.h>

#include <iostream>
#include <string>
#include <vector>

#include "date_controller.h"
#include "time_event.h"
#include "time_segment.h"


class event_browser_ui::evb_context {
public:
  evb_context(cherry_kit::session_sync *a_session, date_controller *a_ctr)
      : m_ctr(a_ctr), m_session(a_session), m_event_id(0) {}
  ~evb_context() { std::cout << __FUNCTION__ << std::endl; }

  void update_date(int a_year, int a_month, int a_day) {
    m_current_year = a_year;
    m_current_month = a_month;
    m_current_day = a_day;

    update_time_line_date();
  }

  void update_time_line_date() {
     std::for_each(std::begin(m_time_segment_list),
                  std::end(m_time_segment_list),
                  [=](time_segment *a_time_seg_ref) {
      if (!a_time_seg_ref)
        return;

      a_time_seg_ref->set_day(m_current_day);
      a_time_seg_ref->set_month(m_current_month);
      a_time_seg_ref->set_year(m_current_year);
     });
  }

  void create_time_segments(date_controller *a_ctr,
                            cherry_kit::session_sync *a_session,
                            cherry_kit::item_view *ck_model_view,
                            cherry_kit::window *window);

  time_segment *insert_time_element(cherry_kit::session_sync *a_session,
                                    cherry_kit::item_view *a_view, int a_value,
                                    int a_type,
                                    cherry_kit::window *a_window_ref,
                                    const date_controller *a_controller_ref);

  cherry_kit::session_sync *m_session;
  date_controller *m_ctr;
  cherry_kit::widget *add_action_button(cherry_kit::fixed_layout *ui, int a_row,
                                        int a_col, const std::string &a_label,
                                        const std::string &a_icon);
  int m_event_id;
  time_segment_list_t m_time_segment_list;

  int m_current_year;
  int m_current_month;
  int m_current_day;
};

void event_browser_ui::evb_context::create_time_segments(
    date_controller *a_ctr, cherry_kit::session_sync *a_session,
    cherry_kit::item_view *ck_model_view, cherry_kit::window *window) {
  m_time_segment_list.push_back(insert_time_element(
      a_session, ck_model_view, 12, time_segment::kAMTime, window, a_ctr));

  for (int i = 1; i <= 11; i++) {
    m_time_segment_list.push_back(insert_time_element(
        a_session, ck_model_view, i, time_segment::kAMTime, window, a_ctr));
  }

  m_time_segment_list.push_back(insert_time_element(
      a_session, ck_model_view, 12, time_segment::kNoonTime, window, a_ctr));

  for (int i = 1; i <= 11; i++) {
    m_time_segment_list.push_back(insert_time_element(
        a_session, ck_model_view, i, time_segment::kPMTime, window, a_ctr));
  }
}

std::function<void()> event_browser_ui::update_time_line() {
  std::function<void()> ck_timeout_func = ([this]() {
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

    std::for_each(std::begin(ctx->m_time_segment_list),
                  std::end(ctx->m_time_segment_list),
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

  return ck_timeout_func;
}

event_browser_ui::event_browser_ui(cherry_kit::session_sync *a_session,
                                   date_controller *a_ctr)
    : ctx(new evb_context(a_session, a_ctr)) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *ui = new cherry_kit::fixed_layout(window);
  cherry_kit::item_view *ck_model_view = 0;
  cherry_kit::timer *ck_timer = new cherry_kit::timer(1000 * 60);
  cherry_kit::icon_button *ck_add_button = 0;
  cherry_kit::calendar_view *ck_calendar = 0;

  ctx->m_event_id = a_session->session_data("calendar_id").toInt();

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

  ck_calendar = dynamic_cast<cherry_kit::calendar_view *>(
      ui->add_widget(0, 0, "calendar", ui_data, [=]() {}));

  ck_model_view = dynamic_cast<cherry_kit::item_view *>(
      ui->add_widget(1, 0, "model_view", ui_data, [=]() {}));
  ck_model_view->set_content_size(320, 64);

  ck_model_view->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  /* set the current date */
  ck_calendar->on_date_change([=]() {
    ctx->m_time_segment_list.clear();
    ck_model_view->clear();
    ctx->create_time_segments(a_ctr, a_session, ck_model_view, window);
    update_time_line()();

    ctx->update_date(ck_calendar->selected_date().year(),
                     ck_calendar->selected_date().month(),
                     ck_calendar->selected_date().day());
  });

  /* update segments */
  ctx->create_time_segments(a_ctr, a_session, ck_model_view, window);
  ctx->update_date(ck_calendar->selected_date().year(),
                   ck_calendar->selected_date().month(),
                   ck_calendar->selected_date().day());


  ck_add_button = dynamic_cast<cherry_kit::icon_button *>(
      ctx->add_action_button(ui, 2, 0, "", "ck_plus"));

  window->set_window_title("Appointments");
  window->set_window_content(ui->viewport());

  ck_add_button->on_click([=]() {
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

    std::for_each(std::begin(ctx->m_time_segment_list),
                  std::end(ctx->m_time_segment_list),
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

        a_time_seg_ref->create_new([=](cherry_kit::window *ck_app_window) {
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
              a_ctr->viewport()->center(sub_window_geometry, window_geometry,
                                        cherry_kit::space::kCenterOnWindow));

          a_ctr->insert(ck_app_window);
        });
      }
    });
  });
  a_session->bind_to_window(window);

  update_time_line()();
  //ck_timer->start_once(1000, ck_timeout_func);
  ck_timer->on_timeout(update_time_line());
  ck_timer->start();

  window->on_window_discarded([=](cherry_kit::window *aWindow) {
    a_session->unbind_window(aWindow);
    ck_model_view->clear();
    if (ck_timer->is_active())
      ck_timer->stop();
    delete ck_timer;

    delete ck_model_view;
    delete ui;
    delete aWindow;
  });

  if (a_ctr->viewport()) {
    a_ctr->insert(window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}

event_browser_ui::~event_browser_ui() { delete ctx; }

void event_browser_ui::insert_event() {}

int event_browser_ui::event_id() const { return ctx->m_event_id; }

time_segment *event_browser_ui::evb_context::insert_time_element(
    cherry_kit::session_sync *a_session, cherry_kit::item_view *a_view,
    int a_value, int a_type, cherry_kit::window *a_window_ref,
    const date_controller *a_controller_ref) {
  cherry_kit::model_view_item *ck_model_itm = new cherry_kit::model_view_item();
  time_segment *ck_time_seg = new time_segment(m_ctr, a_view);
  ck_time_seg->set_time_value(a_value);
  ck_time_seg->set_time_type((time_segment::segment_t)a_type);

  cherry_kit::label *ck_item_lbl = new cherry_kit::label(ck_time_seg);
  cherry_kit::icon_button *ck_button = new cherry_kit::icon_button(ck_time_seg);

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

  ck_time_seg->set_contents_geometry(0, 0, a_view->contents_geometry().width(),
                                     32);
  ck_item_lbl->set_alignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ck_item_lbl->set_text(time_str);
  ck_item_lbl->set_size(QSize(64, 32));

  ck_model_itm->on_activated([=](cherry_kit::model_view_item *a_item) {
    ck_time_seg->create_new([=](cherry_kit::window *ck_app_window) {

      ck_app_window->setPos(a_window_ref->pos());
      QPointF window_pos(a_window_ref->mapToScene(QPointF()));
      QRectF window_geometry(window_pos.x(), window_pos.y(),
                             a_window_ref->geometry().width(),
                             a_window_ref->geometry().height());

      QPointF sub_window_pos(a_window_ref->mapToScene(QPointF()));
      QRectF sub_window_geometry(sub_window_pos.x(), sub_window_pos.y(),
                                 ck_app_window->geometry().width(),
                                 ck_app_window->geometry().height());

      ck_app_window->setPos(
          m_ctr->viewport()->center(sub_window_geometry, window_geometry,
                                    cherry_kit::space::kCenterOnWindow));

      m_ctr->insert(ck_app_window);
    });
  });

  ck_model_itm->set_view(ck_time_seg);
  a_view->insert(ck_model_itm);

  // event handlers.
  ck_model_itm->on_view_removed([=](cherry_kit::model_view_item *a_item) {
    if (a_item && a_item->view()) {
      cherry_kit::widget *view = a_item->view();
      if (view)
        delete view;
    }
  });

  return ck_time_seg;
}

cherry_kit::widget *event_browser_ui::evb_context::add_action_button(
    cherry_kit::fixed_layout *ui, int a_row, int a_col,
    const std::string &a_label, const std::string &a_icon) {
  cherry_kit::widget_properties_t ui_data;
  ui_data["label"] = a_label;
  ui_data["icon"] = "toolbar/" + a_icon + ".png";
  return ui->add_widget(a_row, a_col, "image_button", ui_data, [=]() {});
}
