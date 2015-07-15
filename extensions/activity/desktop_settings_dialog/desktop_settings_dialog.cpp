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

#include <asyncimageloader.h>
#include <asyncimagecreator.h>
#include <ck_image_view.h>

#include <ck_fixed_layout.h>
#include <ck_button.h>
#include <ck_dial_view.h>
#include <ck_item_view.h>
#include <ck_image_io.h>
#include <mutex>

class desktop_settings_dialog::PrivatePhotoSearch {
public:
  PrivatePhotoSearch() : m_task_count(0) {}
  ~PrivatePhotoSearch() {}

  std::vector<std::string> build_image_list();

  cherry_kit::window *m_window;
  cherry_kit::progress_bar *m_progress_bar_widget;
  cherry_kit::item_view *m_image_view;
  cherry_kit::fixed_layout *m_ck_layout;

  std::vector<QPixmap> m_pixmap_list;
  std::vector<QImage> m_io_surface_list;
  std::vector<cherry_kit::image_io *> m_service_list;

  QRectF mGeometry;
  QVariantMap mResult;

  int m_task_count;
  std::mutex m_task_lock;
};

desktop_settings_dialog::desktop_settings_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object), d(new PrivatePhotoSearch) {}

desktop_settings_dialog::~desktop_settings_dialog() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void desktop_settings_dialog::create_window(const QRectF &a_window_geometry,
                                            const QString &a_window_title,
                                            const QPointF &window_pos) {
  d->mGeometry = a_window_geometry;

  d->m_window = new cherry_kit::window();
  d->m_window->setGeometry(a_window_geometry);
  d->m_window->set_window_title(a_window_title);

  cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(d->m_window);

  d->m_ck_layout = ck_ui;

  ck_ui->set_content_margin(10, 10, 10, 10);
  ck_ui->set_geometry(0, 0, a_window_geometry.width(),
                      a_window_geometry.height());

  ck_ui->add_rows(2);
  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 6);

  ck_ui->set_row_height(0, "95%");
  ck_ui->set_row_height(1, "5%");

  cherry_kit::widget_properties_t ui_data;

  cherry_kit::widget *ck_icon_gird = ck_ui->add_widget(0, 0, "widget", ui_data);
  d->m_image_view = new cherry_kit::item_view(
      ck_icon_gird, cherry_kit::item_view::kGridModel);
  // d->m_image_view->set_content_margin(0, 0, 0, 0);
  d->m_image_view->set_content_spacing(0);

  ck_icon_gird->on_geometry_changed([&](const QRectF &a_rect) {
    d->m_image_view->setGeometry(a_rect);
  });

  ui_data["label"] = "Apply";
  ck_ui->add_widget(1, 0, "button", ui_data);
  ui_data["label"] = "Next";
  ck_ui->add_widget(1, 1, "button", ui_data);
  ui_data["label"] = "Previous";
  ck_ui->add_widget(1, 2, "button", ui_data);

  d->m_window->set_window_content(ck_ui->viewport());

  d->m_image_view->set_view_geometry(ck_icon_gird->geometry());
  d->m_image_view->setPos(QPointF());

  QTimer::singleShot(500, this, SLOT(load_from_system_path()));
  // load_from_system_path();
  exec(window_pos);
}

QRectF desktop_settings_dialog::geometry() const { return d->mGeometry; }

QVariantMap desktop_settings_dialog::result() const { return d->mResult; }

window *desktop_settings_dialog::dialog_window() const { return d->m_window; }

void desktop_settings_dialog::cleanup() {
  if (d->m_image_view)
    d->m_image_view->clear();

  if (d->m_ck_layout)
    delete d->m_ck_layout;

  if (d->m_window) {
    delete d->m_window;
  }

  d->m_io_surface_list.erase(std::end(d->m_io_surface_list));
  d->m_io_surface_list.clear();

  std::for_each(std::begin(d->m_service_list), std::end(d->m_service_list),
                [this](cherry_kit::image_io *a_service) {
    qDebug() << Q_FUNC_INFO << "clear service";
    if (a_service) {
      delete a_service;
    }
  });

  d->m_service_list.clear();

  d->m_window = 0;
}

void desktop_settings_dialog::insert_image_to_grid(
    const QImage &ck_preview_pixmap) const {

  if (ck_preview_pixmap.isNull()) {
    qDebug() << Q_FUNC_INFO << "Null image in list";
    return;
  }

  cherry_kit::image_view *ck_image_preview =
      new cherry_kit::image_view(d->m_image_view);

  ck_image_preview->set_image(ck_preview_pixmap);

  int width = 72;
  int height = width;

  ck_image_preview->set_size(QSizeF(width, height));

  cherry_kit::model_view_item *ck_preview_item =
      new cherry_kit::model_view_item();

  ck_preview_item->on_view_removed([](cherry_kit::model_view_item *item) {
    if (item && item->view()) {
      cherry_kit::widget *view = item->view();
      if (view)
        delete view;
    }
  });

  ck_image_preview->setGeometry(QRectF(0, 0, width, height));
  ck_image_preview->setMinimumSize(width, height);

  ck_preview_item->set_view(ck_image_preview);

  d->m_image_view->insert(ck_preview_item);
}

void desktop_settings_dialog::load_images() const {
  std::for_each(
      std::begin(d->m_io_surface_list), std::end(d->m_io_surface_list),
      [this](const QImage &a_image) { insert_image_to_grid(a_image); });
}

void desktop_settings_dialog::load_from_system_path() const {
  std::vector<std::string> current_file_list = d->build_image_list();

  int task_count = current_file_list.size();

  std::for_each(std::begin(current_file_list), std::end(current_file_list),
                [=](const std::string &a_file) {

    cherry_kit::image_io *ck_image_service = new cherry_kit::image_io(0, 0);
    d->m_service_list.push_back(ck_image_service);

    ck_image_service->on_ready([=](
        cherry_kit::image_io::buffer_load_status_t a_status,
        cherry_kit::image_io *a_image_io) {

      if (a_status != cherry_kit::image_io::kSuccess) {
        d->m_task_lock.lock();
        d->m_task_count++;
        d->m_task_lock.unlock();
        return;
      }

      cherry_kit::io_surface *ck_surface_ref = a_image_io->surface();

      QImage ck_image(ck_surface_ref->buffer, ck_surface_ref->width,
                      ck_surface_ref->height, QImage::Format_ARGB32);

      // QPixmap pixmap = QPixmap::fromImage(ck_image);
      d->m_task_lock.lock();
      // d->m_pixmap_list.push_back(pixmap);
      d->m_io_surface_list.push_back(ck_image);

      qDebug() << Q_FUNC_INFO << d->m_task_count << "/" << task_count;
      d->m_task_count++;
      if (d->m_task_count == task_count) {
        this->load_images();
      }
      d->m_task_lock.unlock();
    });

    ck_image_service->preview_image(a_file);
  });

  current_file_list.erase(std::end(current_file_list));
}

std::vector<std::string>
desktop_settings_dialog::PrivatePhotoSearch::build_image_list() {
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
      QDir qt_path(path_meta_data.filePath());

      allowed_mime_types << "*.png"
                         << "*.jpg"
                         << "*.jpeg"
                         << "*.tiff"
                         << "*.svg";
      qt_path.setNameFilters(allowed_mime_types);

      QStringList filtered_local_files = qt_path.entryList();

      Q_FOREACH(const QString & image_file_name, filtered_local_files) {
        QString image_full_path = QDir::toNativeSeparators(
            qt_path.absolutePath() + "/" + image_file_name);
        rv_list.push_back(image_full_path.toStdString());
      }
    }
  });

  image_path_list.erase(std::end(image_path_list));

  return rv_list;
}
