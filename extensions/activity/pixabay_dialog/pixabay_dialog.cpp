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
#include "pixabay_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_fixed_layout.h>

#include "pixabay_service.h"

#include <QDebug>
#include <ck_button.h>
#include <ck_image_view.h>
#include <ck_item_view.h>
#include <ck_line_edit.h>
#include <ck_progress_bar.h>

#include <atomic>

class pixabay_dialog::Privatepixabay {
public:
  Privatepixabay() : m_count(0) {}
  ~Privatepixabay() {}

  cherry_kit::window *m_main_window;
  cherry_kit::fixed_layout *m_layout;
  pixabay_service *m_service;
  cherry_kit::item_view *m_grid_view;

  cherry_kit::window *m_progress_window;
  cherry_kit::progress_bar *m_progress_widget;

  std::atomic<int> m_count;

  std::vector<cherry_kit::image_view *> m_pool;

  void create_pool() {
    for (int i = 0; i < 8; i++) {
      cherry_kit::image_view *item = new cherry_kit::image_view(m_grid_view);
      cherry_kit::model_view_item *ck_preview_item =
          new cherry_kit::model_view_item();
      
      int width = 128;
      int height = 128;

      item->set_size(QSizeF(width, height));
      item->set_contents_geometry(0, 0, width, height);

      ck_preview_item->set_view(item);

      m_grid_view->insert(ck_preview_item);

      m_pool.push_back(item);
    }
  }

  cherry_kit::image_view *get() {
    std::cout << "get item : " << m_count << std::endl;

    cherry_kit::image_view *rv = m_pool.at(m_count);
    m_count++;

    if (m_count >= 8)
      m_count = 0;

    return rv;
  }
};

pixabay_dialog::pixabay_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object), priv(new Privatepixabay) {}

pixabay_dialog::~pixabay_dialog() { delete priv; }

void pixabay_dialog::create_window(const QRectF &window_geometry,
                                   const QString &window_title,
                                   const QPointF &window_pos) {
  priv->m_main_window = new cherry_kit::window();

  set_geometry(window_geometry);
  update_content_geometry(priv->m_main_window);

  /* progress window */
  priv->m_progress_window = new cherry_kit::window(priv->m_main_window);
  priv->m_progress_window->set_window_type(
      cherry_kit::window::kNotificationWindow);
  priv->m_progress_window->set_geometry(QRectF(0, 0, 320, 240));
  priv->m_progress_widget =
      new cherry_kit::progress_bar(priv->m_progress_window);
  priv->m_progress_widget->set_range(1, 100);
  priv->m_progress_widget->set_size(QSizeF(240, 48));
  priv->m_progress_window->set_window_content(priv->m_progress_widget);
  priv->m_progress_window->raise();
  priv->m_progress_widget->set_value(0);
  priv->m_progress_window->setPos((window_geometry.width() / 2) - 120,
                                  (window_geometry.height() / 2) - 24);
  priv->m_progress_window->set_window_title("Search Powered By Pixabay");

  priv->m_progress_window->show();

  /* Layout */
  priv->m_layout = new cherry_kit::fixed_layout(priv->m_main_window);
  priv->m_layout->set_content_margin(4, 4, 4, 4);
  priv->m_layout->set_geometry(0, 0, priv->m_main_window->geometry().width(),
                               priv->m_main_window->geometry().height());
  priv->m_layout->add_rows(3);
  priv->m_layout->add_segments(0, 2);
  priv->m_layout->add_segments(1, 1);
  priv->m_layout->add_segments(2, 3);

  priv->m_layout->set_row_height(0, "10%");
  priv->m_layout->set_row_height(1, "80%");
  priv->m_layout->set_row_height(2, "10%");

  priv->m_layout->set_segment_width(0, 0, "80%");
  priv->m_layout->set_segment_width(0, 1, "20%");

  cherry_kit::widget_properties_t ui_data;

  /* navigation buttons */
  ui_data["icon"] = "toolbar/ck_arrow-left.png";
  priv->m_layout->add_widget(2, 0, "image_button", ui_data, [=]() {});
  ui_data["icon"] = "branding/ck_pixabay_logo.png";
  priv->m_layout->add_widget(2, 1, "image_button", ui_data, [=]() {});
  ui_data["icon"] = "toolbar/ck_arrow-right.png";
  priv->m_layout->add_widget(2, 2, "image_button", ui_data, [=]() {});

  /*insert widgets to layout */

  ui_data["text"] = "";
  cherry_kit::line_edit *editor = dynamic_cast<cherry_kit::line_edit *>(
      priv->m_layout->add_widget(0, 0, "line_edit", ui_data, [=]() {}));

  ui_data["label"] = "Search";

  cherry_kit::button *ck_search_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(0, 1, "button", ui_data, [=]() {
        if (!editor) {
          qDebug() << Q_FUNC_INFO << "Invalid Editor";
          return;
        }

        if (!priv->m_progress_window || !priv->m_progress_widget)
          return;

        priv->m_progress_window->show();
        priv->m_progress_window->raise();
        priv->m_service->search(editor->text().toStdString());
        priv->m_progress_window->set_window_title("Search : " + editor->text());
        priv->m_progress_window->setZValue(10000);
      }));

  /* grid view */
  cherry_kit::widget *_base_widget =
      priv->m_layout->add_widget(1, 0, "widget", ui_data, [=]() {});

  if (!_base_widget)
    return;
  priv->m_grid_view = new cherry_kit::item_view(
      _base_widget, cherry_kit::item_view::kGridModel);

  _base_widget->on_geometry_changed([&](const QRectF &a_rect) {
    priv->m_grid_view->set_geometry(a_rect);
  });

  /* the reset of the things */
  priv->m_main_window->set_window_content(priv->m_layout->viewport());
  exec(window_pos);

  priv->m_service = new pixabay_service();

  priv->m_grid_view->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  priv->m_grid_view->set_view_geometry(_base_widget->geometry());
  priv->m_grid_view->set_coordinates(0, 0);
  priv->m_grid_view->set_content_size(128, 128);
  priv->m_grid_view->set_column_count(4);
  priv->m_grid_view->set_enable_scrollbars(false);
  priv->m_grid_view->set_content_spacing(0);

  priv->m_service->on_progress([=](int a_value) {
    if (!priv->m_progress_window)
      return;

    if (!priv->m_progress_widget)
      return;

    if (a_value > 80) {
      priv->m_progress_window->hide();
      return;
    }

    priv->m_progress_window->show();
    priv->m_progress_widget->set_value(a_value);
  });

  priv->m_service->on_ready([=](const pixabay_service *a_service) {
    if (!a_service)
      return;
    service_result_list list = a_service->get();

    std::for_each(std::begin(list), std::end(list),
                  [=](pixabay_service_hit_result *a_hit) {
      if (!a_hit)
        return;

      if (!priv->m_grid_view) {
        qDebug() << Q_FUNC_INFO << "Invalid Grid Model"
                 << a_hit->preview_url().c_str();
        return;
      }

      cherry_kit::io_surface *ck_surface_ref = a_hit->preview_image();

      if (!ck_surface_ref) {
        qDebug() << Q_FUNC_INFO << "Invalid Surface"
                 << a_hit->preview_url().c_str();
        return;
      }

      QImage image_buffer(ck_surface_ref->copy(), ck_surface_ref->width,
                          ck_surface_ref->height, QImage::Format_ARGB32);
      image_buffer = image_buffer.scaledToHeight(128, Qt::SmoothTransformation);

      cherry_kit::image_view *ck_view = priv->get();

      ck_view->set_image(image_buffer);
      ck_view->reset_click_event();
      ck_view->on_click([=]() {
        notify_message("url", a_hit->web_format_url());
      });
    });
  });

  priv->m_progress_window->raise();
  priv->m_progress_widget->show();

  priv->create_pool();
  priv->m_service->search("green leaf");
}

void pixabay_dialog::update_attribute(const QString &name,
                                      const QVariant &data) {}

cherry_kit::window *pixabay_dialog::dialog_window() const {
  return priv->m_main_window;
}

void pixabay_dialog::purge() {
  if (priv->m_main_window) {
    delete priv->m_main_window;
  }
  priv->m_main_window = 0;
}
