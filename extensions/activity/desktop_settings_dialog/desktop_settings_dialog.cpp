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

#include <ck_fixed_layout.h>
#include <ck_button.h>
#include <ck_dial_view.h>
#include <ck_item_view.h>
#include <ck_image_io.h>

class desktop_settings_dialog::PrivatePhotoSearch {
public:
  PrivatePhotoSearch() {}
  ~PrivatePhotoSearch() {}

  cherry_kit::window *m_window;
  cherry_kit::progress_bar *m_progress_bar_widget;
  cherry_kit::item_view *m_image_view;

  QRectF mGeometry;
  QVariantMap mResult;
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

  ck_icon_gird->on_geometry_changed([&](const QRectF &a_rect) {
    d->m_image_view->setGeometry(a_rect);
  });

  ui_data["label"] = "00";

  ck_ui->add_widget(1, 0, "button", ui_data);
  ck_ui->add_widget(1, 1, "button", ui_data);
  ck_ui->add_widget(1, 2, "button", ui_data);

  d->m_window->set_window_content(ck_ui->viewport());

  QTimer::singleShot(500, this, SLOT(load_from_system_path()));
  exec(window_pos);
}

QRectF desktop_settings_dialog::geometry() const { return d->mGeometry; }

QVariantMap desktop_settings_dialog::result() const { return d->mResult; }

window *desktop_settings_dialog::dialog_window() const { return d->m_window; }

void desktop_settings_dialog::cleanup() {
  if (d->m_window) {
    delete d->m_window;
  }

  d->m_window = 0;
}
/*
void desktop_settings_dialog::onShowAnimationFinished() {
  this->load_from_system_path();
}

void desktop_settings_dialog::onClicked(table_view_item *item) {
  if (d->mFactory && d->mFactory->hasRunningThreads()) {
    return;
  }

  ImageCell *i = qobject_cast<ImageCell *>(item);
  if (i) {
    d->mResult["action"] = QString("Change Background");
    d->mResult["background"] = "file:///" + i->label();
    update_action();
  }
}

void desktop_settings_dialog::onProgressValue(int value) {
  if (value == 100) {
    if (d->m_window) {
      if (has_attribute("title")) {
      }
    }
  }
}
*/

void desktop_settings_dialog::load_from_system_path() const {
  std::vector<std::string> image_path_list;

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
    qDebug() << Q_FUNC_INFO << a_path.c_str();
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

        cherry_kit::image_io *ck_image_service = new cherry_kit::image_io(0, 0);

        ck_image_service->on_ready([&](
            cherry_kit::image_io::buffer_load_status_t a_status,
            cherry_kit::image_io *a_image_io) {
          cherry_kit::io_surface *ck_img_surface_ref = a_image_io->surface();

          if (a_status != cherry_kit::image_io::kSuccess ||
              !ck_img_surface_ref) {
            delete a_image_io;
            return;
          }

          qDebug() << Q_FUNC_INFO << "Loaded!";
          delete a_image_io;
        });

        ck_image_service->create(image_full_path.toStdString());
      }
    }
  });
}
