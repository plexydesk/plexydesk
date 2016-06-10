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
#include <memory>
#include <thread>
#include <future>

class pixabay_dialog::Privatepixabay {
public:
  Privatepixabay() : m_count(0), m_in_progress(0), m_current_page(0) {}
  ~Privatepixabay() {}

  cherry_kit::window *m_main_window;
  cherry_kit::fixed_layout *m_layout;
  cherry_kit::item_view *m_grid_view;

  cherry_kit::window *m_progress_window;
  cherry_kit::progress_bar *m_progress_widget;

  std::atomic<int> m_count;
  std::atomic<bool> m_in_progress;
  std::future<bool> m_busy_wait;

  int m_current_page;

  cherry_kit::line_edit *m_editor;

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
    std::cout << "Pool Size : " << m_pool.size() << std::endl;
    std::cout << "get item : " << m_count << std::endl;

    if (m_pool.size() <= 0)
      return 0;

    cherry_kit::image_view *rv = m_pool.at(m_count);
    m_count++;

    if (m_count >= 8)
      m_count = 0;

    return rv;
  }

  void release_pool() {
    m_pool.erase(m_pool.begin(), m_pool.end());
    m_pool.clear();
  }
};

pixabay_dialog::pixabay_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object), priv(new Privatepixabay) {}

pixabay_dialog::~pixabay_dialog() {
    std::cout << __FUNCTION__ << std::endl;
    delete priv;
}

void pixabay_dialog::create_window() {
  const QRectF window_geometry(0, 0, 520, 340);
  priv->m_main_window = new cherry_kit::window();
  priv->m_main_window->set_window_title("Pixabay Search");

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
  priv->m_layout->add_widget(2, 0, "image_button", ui_data, [=]() {
    if (priv->m_current_page == 0 || priv->m_in_progress || !priv->m_editor)
      return;

    priv->m_current_page -= 1;

    if (priv->m_current_page < 0)
      priv->m_current_page = 0;

    priv->m_progress_window->show();
    priv->m_progress_window->raise();
    priv->m_progress_window->setZValue(1000);
    priv->m_progress_window->set_window_title(
        QString("Loading Page %1").arg(priv->m_current_page));

    priv->m_in_progress = true;
    search(priv->m_editor->text().toStdString(),
                            priv->m_current_page);
  });

  ui_data["icon"] = "branding/ck_pixabay_logo.png";
  priv->m_layout->add_widget(2, 1, "image_button", ui_data, [=]() {});
  ui_data["icon"] = "toolbar/ck_arrow-right.png";
  priv->m_layout->add_widget(2, 2, "image_button", ui_data, [=]() {
    if (priv->m_in_progress || !priv->m_editor)
      return;

    priv->m_current_page += 1;

    priv->m_progress_window->show();
    priv->m_progress_window->raise();
    priv->m_progress_window->setZValue(1000);
    priv->m_progress_window->set_window_title(
        QString("Loading Page %1").arg(priv->m_current_page));

    priv->m_in_progress = true;
    search(priv->m_editor->text().toStdString(),
                            priv->m_current_page);
  });

  /*insert widgets to layout */

  ui_data["text"] = "";
  priv->m_editor = dynamic_cast<cherry_kit::line_edit *>(
      priv->m_layout->add_widget(0, 0, "line_edit", ui_data, [=]() {}));

  ui_data["label"] = "Search";

  cherry_kit::button *ck_search_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(0, 1, "button", ui_data, [=]() {
        if (!priv->m_editor) {
          qDebug() << Q_FUNC_INFO << "Invalid Editor";
          return;
        }

        if (!priv->m_progress_window || !priv->m_progress_widget)
          return;

        priv->m_progress_window->show();
        priv->m_progress_window->raise();
        priv->m_current_page = 0;
        search(priv->m_editor->text().toStdString(),
               priv->m_current_page);
        priv->m_progress_window->set_window_title("Search : " +
                                                  priv->m_editor->text());
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

  priv->m_grid_view->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  priv->m_grid_view->set_view_geometry(_base_widget->geometry());
  priv->m_grid_view->set_coordinates(0, 0);
  priv->m_grid_view->set_content_size(128, 128);
  priv->m_grid_view->set_column_count(4);
  priv->m_grid_view->set_enable_scrollbars(false);
  priv->m_grid_view->set_content_spacing(0);

  priv->m_progress_window->raise();
  priv->m_progress_widget->show();

  priv->create_pool();
  search("montreal", 7);
}

void pixabay_dialog::download_image(const std::string &a_url) {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([=](const social_kit::url_response &response) {
    if (response.status_code() == 200) {
      std::unique_ptr<cherry_kit::image_io> image(
          new cherry_kit::image_io(0, 0));

      image->on_ready([=](cherry_kit::image_io::buffer_load_status_t s,
                          cherry_kit::image_io *a_img) {
        if (s == cherry_kit::image_io::kSuccess) {
          cherry_kit::io_surface *surface = a_img->surface();

          std::unique_ptr<cherry_kit::image_io> sync_img(
              new cherry_kit::image_io(0, 0));

          if (surface) {
            sync_img->on_image_saved([this](const std::string &a_file_name) {

              notify_message("url", a_file_name);

              if (priv->m_progress_widget) {
                priv->m_progress_widget->set_value(75);
                priv->m_progress_widget->set_value(100);
                priv->m_progress_window->hide();
              }

              std::unique_ptr<cherry_kit::image_io>(a_img);
              std::unique_ptr<cherry_kit::image_io>(sync_img);
            });

            sync_img->save(surface->dup(), "wallpaper");
          }
        } else {
          std::cout << __FUNCTION__ << "Error creating Image:" << std::endl;
        }
      });

      priv->m_progress_widget->set_value(50);
      image->create((response.data_buffer()), response.data_buffer_size());
    } else {
      std::cout << __FUNCTION__ << "Error downloading url :" << a_url
                << std::endl;
    }
  });

  priv->m_progress_widget->set_value(25);

  request->send_message(social_kit::url_request::kGETRequest, a_url);
}

void pixabay_dialog::search(const std::string &a_query, int page_count) {
  pixabay_service *srv = new pixabay_service();

  srv->on_progress([=](int a_value) {
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

  srv->on_ready([=](const pixabay_service *a_service) {
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

      if (!ck_view)
        return;

      ck_view->set_image(image_buffer);
      ck_view->reset_click_event();
      ck_view->on_click([=]() {
        priv->m_progress_window->show();
        priv->m_progress_window->raise();
        priv->m_progress_window->set_window_title(
            "Setting Desktop Wallpaper...");
        priv->m_progress_window->setZValue(10000);
        priv->m_progress_widget->set_value(10);
        download_image(a_hit->hd_image_url());
      });
    });

    delete srv;
    priv->m_in_progress = false;
  });

  priv->m_in_progress = true;
  srv->search(a_query, page_count);
}

cherry_kit::window *pixabay_dialog::dialog_window() const {
  return priv->m_main_window;
}

bool pixabay_dialog::purge() {
  if (priv->m_in_progress) {
      /*
      priv->m_busy_wait = std::async(std::launch::async, [=]() {
          while(priv->m_in_progress);
          std::cout << "Ready for Deletion ..." << std::endl;
          discard_activity();
          return true;
      });
      */
    return false;
  }

  /*
  if (priv->m_busy_wait.get() == true) {
      std::cout << "Purge Dialog  ..." << std::endl;
  }
  */

  std::cout << "Purge Dialog  ..." << std::endl;
  /* clean up progress window */
  if (priv->m_progress_window) {
    priv->m_progress_window->hide();

    if (priv->m_progress_widget)
      delete priv->m_progress_widget;

    priv->m_progress_widget = 0;

    delete priv->m_progress_window;

    priv->m_progress_window = 0;
  }

  /* cleanup layout */

  priv->release_pool();

  if (priv->m_grid_view) {
    delete priv->m_grid_view;
  }

  if (priv->m_layout) {
    delete priv->m_layout;
  }

  if (priv->m_main_window) {
      delete priv->m_main_window;
  }

  priv->m_main_window = 0;

  return true;
}
