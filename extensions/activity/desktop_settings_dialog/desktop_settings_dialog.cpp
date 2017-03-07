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
#include "desktop_settings_dialog.h"
#include <thread>

#include <ck_widget.h>
#include <ck_config.h>

#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>

#include <ck_config.h>
#include <ck_desktop_controller_interface.h>
#include <ck_progress_bar.h>

#ifdef __QT5_TOOLKIT__
#include <asyncimageloader.h>
#include <asyncimagecreator.h>
#endif

#include <ck_image_view.h>

#include <ck_fixed_layout.h>
#include <ck_button.h>
#include <ck_dial_view.h>
#include <ck_item_view.h>
#include <ck_image_io.h>
#include <ck_sandbox.h>

// stdlib
#include <condition_variable>
#include <mutex>

class desktop_settings_dialog::PrivatePhotoSearch {
public:
  PrivatePhotoSearch() {}
  ~PrivatePhotoSearch() {}

  std::vector<std::string> locate_system_images();

  cherry_kit::window *m_window;

  cherry_kit::window *m_progress_window;
  cherry_kit::progress_bar *m_progress_widget;

  cherry_kit::item_view *m_image_view;
  cherry_kit::fixed_layout *m_ck_layout;

  QRectF m_geometry;
  QVariantMap m_activity_result;

  std::mutex m_task_lock;
};

desktop_settings_dialog::desktop_settings_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object), priv(new PrivatePhotoSearch) {}

desktop_settings_dialog::~desktop_settings_dialog() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void desktop_settings_dialog::create_window() {
  QRectF a_window_geometry(0, 0, 672, 340);
  priv->m_geometry = a_window_geometry;
  QString a_window_title = "Desktop Wallpapers";

  priv->m_window = new cherry_kit::window();
  priv->m_window->set_geometry(a_window_geometry);
  priv->m_window->set_window_title(a_window_title);

  cherry_kit::fixed_layout *ck_ui =
      new cherry_kit::fixed_layout(priv->m_window);

  priv->m_ck_layout = ck_ui;

  ck_ui->set_content_margin(4, 4, 4, 4);
  ck_ui->set_geometry(0, 0, a_window_geometry.width(),
                      a_window_geometry.height());

  ck_ui->add_rows(2);
  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 3);

  ck_ui->set_row_height(0, "95%");
  ck_ui->set_row_height(1, "5%");

  cherry_kit::widget_properties_t ui_data;

  cherry_kit::widget *ck_icon_gird =
      ck_ui->add_widget(0, 0, "widget", ui_data, [=]() {});

  priv->m_image_view = new cherry_kit::item_view(
      ck_icon_gird, cherry_kit::item_view::kGridModel);

  priv->m_image_view->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  priv->m_image_view->set_content_size(128, 128);
  priv->m_image_view->set_column_count(5);
  priv->m_image_view->set_enable_scrollbars(true);
  priv->m_image_view->set_content_spacing(0);

  ck_icon_gird->on_geometry_changed([&](const QRectF &a_rect) {
    priv->m_image_view->set_geometry(a_rect);
  });

  ui_data["label"] = "";
  ui_data["icon"] = "actions/pd_previous.png";

  ck_ui->add_widget(1, 0, "image_button", ui_data, [=]() {});
  ui_data["label"] = "";
  ui_data["icon"] = "actions/pd_search.png";
  ck_ui->add_widget(1, 1, "image_button", ui_data, [=]() {});
  ui_data["label"] = "";
  ui_data["icon"] = "actions/pd_next.png";
  ck_ui->add_widget(1, 2, "image_button", ui_data, [=]() {});

  priv->m_window->set_window_content(ck_ui->viewport());

  priv->m_image_view->set_view_geometry(ck_icon_gird->geometry());
  priv->m_image_view->setPos(QPointF());

  priv->m_progress_window = new cherry_kit::window(priv->m_window);
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
  priv->m_progress_window->setPos((a_window_geometry.width() / 2) - 120,
                                  (a_window_geometry.height() / 2) - 24);

  // todo: to check if this is the first time.
  // Fix this with true asynchronis signaling across threads.
  if (!QFileInfo(ck_sandbox_root() + "/" + ".cherry_io/cache/").exists())
    QTimer::singleShot(0, this, SLOT(invoke_image_loader()));
  else
    invoke_image_loader();
}

window *desktop_settings_dialog::dialog_window() const {
  return priv->m_window;
}

bool desktop_settings_dialog::purge() {
  if (priv->m_image_view) {
     priv->m_image_view->clear();
    delete priv->m_image_view;
  }

  if (priv->m_progress_widget)
      delete priv->m_progress_widget;

  if (priv->m_progress_window) {
    delete priv->m_progress_window;
  }

  if (priv->m_ck_layout) {
    delete priv->m_ck_layout;
  }

  if (priv->m_window) {
    delete priv->m_window;
    priv->m_window = 0;
  }

  return true;
}

void
desktop_settings_dialog::insert_image_to_grid(const QImage &ck_preview_pixmap,
                                              const std::string &a_file_url) {
  if (ck_preview_pixmap.isNull()) {
    qWarning() << Q_FUNC_INFO << "Null image in list";
    return;
  }

  cherry_kit::image_view *ck_image_preview =
      new cherry_kit::image_view(priv->m_image_view);

  ck_image_preview->on_click([=]() { notify_message("url", a_file_url); });

  ck_image_preview->set_image(ck_preview_pixmap);

  int width = 128;
  int height = 128;

  ck_image_preview->set_size(QSizeF(width, height));

  cherry_kit::model_view_item *ck_preview_item =
      new cherry_kit::model_view_item();

  ck_image_preview->set_contents_geometry(0, 0, width, height);

  ck_preview_item->set_view(ck_image_preview);
  priv->m_image_view->insert(ck_preview_item);
}

void desktop_settings_dialog::load_images() {}

void desktop_settings_dialog::invoke_image_loader() {
  std::vector<std::string> current_file_list = priv->locate_system_images();

  int load_progress = 0;

  if (priv->m_progress_widget)
    priv->m_progress_widget->set_range(0, current_file_list.size());

  std::for_each(std::begin(current_file_list), std::end(current_file_list),
                [&](const std::string &a_file) {
    cherry_kit::image_io *ck_image_service = new cherry_kit::image_io(0, 0);
    //priv->m_service_list.push_back(ck_image_service);

    ck_image_service->on_ready([&](
        cherry_kit::image_io::buffer_load_status_t a_status,
        cherry_kit::image_io *a_image_io) {

      load_progress++;
      if (a_status != cherry_kit::image_io::kSuccess) {
        return;
      }

      cherry_kit::io_surface *ck_surface_ref = a_image_io->surface();
      QImage image_buffer(ck_surface_ref->copy(), ck_surface_ref->width,
                          ck_surface_ref->height, QImage::Format_ARGB32);
      insert_image_to_grid(image_buffer, a_image_io->url());
      if (priv->m_progress_widget && priv->m_progress_window) {
        priv->m_progress_widget->set_value(load_progress);
        priv->m_progress_window->set_window_title(QString(
            "Optimizing Images : %1%").arg(priv->m_progress_widget->value()));
      }
      delete a_image_io;
    });

    ck_image_service->create(a_file, true);
  });

  // current_file_list.erase(std::end(current_file_list));

  if (priv->m_progress_window) {
    priv->m_progress_window->hide();
    priv->m_progress_window->discard();
  }
}

std::vector<std::string>
desktop_settings_dialog::PrivatePhotoSearch::locate_system_images() {
  std::vector<std::string> image_path_list;
  std::vector<std::string> rv_list;

#ifdef Q_OS_LINUX
  image_path_list.push_back("/usr/share/backgrounds/");
  image_path_list.push_back("/usr/share/backgrounds/gnome");
#elif defined(Q_OS_MAC)
  image_path_list.push_back("/Library/Desktop Pictures/");
#elif defined(Q_OS_WIN)
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Theme1\\");
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Theme2\\");
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Windows\\");
#endif

  image_path_list.push_back(
      cherry_kit::config::cache_dir("wallpaper").toStdString());

  std::for_each(std::begin(image_path_list), std::end(image_path_list),
                [&](const std::string &a_path) {
    QFileInfo path_meta_data(a_path.c_str());

    if (path_meta_data.isDir()) {
      QStringList allowed_mime_types;
      QDir image_dir(path_meta_data.filePath());

      allowed_mime_types << "*.png"
                         << "*.jpg"
                         << "*.jpeg"
                         << "*.tiff"
                         << "*.svg";
      image_dir.setNameFilters(allowed_mime_types);

      QStringList filtered_local_files = image_dir.entryList();

      Q_FOREACH(const QString & image_file_name, filtered_local_files) {
        QString image_full_path = QDir::toNativeSeparators(
            image_dir.absolutePath() + "/" + image_file_name);
        rv_list.push_back(image_full_path.toStdString());
      }
    }
  });

  // if (image_path_list.size() > 0)
  // image_path_list.erase(std::end(image_path_list));

  return rv_list;
}
