/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or nESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "background_controller.h"

// plexydesk
#include <ck_config.h>
#include <ck_extension_manager.h>
#include <ck_widget.h>
#include <ck_extension_manager.h>
#include <ck_resource_manager.h>

// social

#ifdef __QT5_TOOLKIT__
#include <webservice.h>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#endif


// Qt
#include <QDir>
#include <QMimeData>
#include <QPixmap>
#include <QPainter>

#include <ck_fixed_layout.h>
#include <ck_space.h>
#include <ck_workspace.h>
#include <ck_image_view.h>
#include <ck_icon_button.h>
#include <ck_system_window_context.h>
#include <ck_image_io.h>

#include <ck_url.h>

#include "desktopwindow.h"

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <tchar.h>
#endif

//#temporary fix to port 
static bool _url_is_local_file(const QUrl &a_url) {
#ifdef __QT5_TOOLKIT__
  return a_url.isLocalFile();
#endif

#ifdef __QT4_TOOLKIT__
  QString _local_file = a_url.toLocalFile();

  return !_local_file.isEmpty();
#endif
}

class desktop_controller_impl::PrivateBackgroundController {
public:
  PrivateBackgroundController() {}
  ~PrivateBackgroundController() {}

  void save_session(const QString &a_key, const QString &a_value);

  void updateProgress(float progress);

  std::string m_background_texture;
  desktop_window *m_background_window;

  cherry_kit::ui_action m_supported_action;
};

desktop_controller_impl::desktop_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      o_ctr(new PrivateBackgroundController) {}

desktop_controller_impl::~desktop_controller_impl() {
  delete o_ctr;
  qDebug() << Q_FUNC_INFO;
}

void desktop_controller_impl::init() {
  create_task_group();

  /*
  QString default_wallpaper_file = QDir::toNativeSeparators(
      cherry_kit::config::instance()->prefix() +
      QString("/share/plexy/themepack/default/resources/default-16x9.png"));
 */

  QString default_wallpaper_file =
      cherry_kit::resource_manager::instance()->drawable_file_name(
          "mdpi", "desktop/ck_default_wallpaper.png");

  o_ctr->m_background_texture = default_wallpaper_file.toStdString();
  o_ctr->m_background_window = new desktop_window();
  o_ctr->m_background_window->set_controller(this);
  o_ctr->m_background_window->set_background(
      default_wallpaper_file.toStdString());

  o_ctr->m_background_window->on_window_discarded([this](
      cherry_kit::window *a_window) {
    if (o_ctr->m_background_window)
      delete o_ctr->m_background_window;
  });

  insert(o_ctr->m_background_window);
}

void desktop_controller_impl::revoke_session(const QVariantMap &args) {
  if (o_ctr->m_background_window)
    return;

  QUrl qt_image_url = args["background"].toString();
  QString qt_mode_string = args["mode"].toString();

  if (qt_image_url.isEmpty())
    return;

#ifdef __QT5_TOOLKIT__
  if (qt_image_url.isLocalFile()) {
    o_ctr->m_background_window->set_background(
        qt_image_url.toLocalFile().toStdString());
    o_ctr->m_background_texture = qt_image_url.toString().toStdString();
  } else {
    download_image_from_url(qt_image_url);
  }
#endif

#ifdef __QT4_TOOLKIT__
  QString _local_file = qt_image_url.toLocalFile();

  if (_url_is_local_file(qt_image_url)) {
    o_ctr->m_background_window->set_background(
        qt_image_url.toLocalFile().toStdString());
    o_ctr->m_background_texture = qt_image_url.toString().toStdString();
  } else {
    download_image_from_url(qt_image_url);
  }
#endif
}

void desktop_controller_impl::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {
  if (!a_session_root.has_property("background") ||
      !a_session_root.has_property("mode"))
    return;

  std::string background_url_str = a_session_root.property("background");
  QUrl qt_background_url = QUrl(background_url_str.c_str());
  QString qt_mode_string = a_session_root.property("mode").c_str();


  if (qt_background_url.isEmpty())
    return;

  if (_url_is_local_file(qt_background_url)) {
    o_ctr->m_background_window->set_background(
        qt_background_url.toLocalFile().toStdString());
    o_ctr->m_background_texture = background_url_str;
    viewport()->update_background_texture();
  } else {
    download_image_from_url(qt_background_url);
  }
}

void desktop_controller_impl::submit_session_data(
    cherry_kit::sync_object *a_object) {
  a_object->set_property("background", o_ctr->m_background_texture);
  a_object->set_property("mode", "scale");
}

void desktop_controller_impl::open_background_dialog() const {
  cherry_kit::desktop_dialog_ref ck_activity =
          viewport()->create_activity("desktop_settings_dialog");

  ck_activity->on_notify([=](const std::string &key, const std::string &value) {
    if (key.compare("url") == 0) {
      o_ctr->m_background_window->set_background(value);
      o_ctr->m_background_texture = "file:///" + value;
      viewport()->update_session_value(controller_name(), "", "");
      viewport()->update_background_texture();
    }
  });
}

void desktop_controller_impl::download_image(const std::string &a_url) {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([&](const social_kit::url_response &response) {
    if (response.status_code() == 200) {
      cherry_kit::image_io *image = new cherry_kit::image_io(0, 0);

      image->on_ready([&](cherry_kit::image_io::buffer_load_status_t s,
                          cherry_kit::image_io *a_img) {
        if (s == cherry_kit::image_io::kSuccess) {
          cherry_kit::io_surface *surface = a_img->surface();

          if (surface) {
            QImage bg_image = QImage(surface->copy(), surface->width,
                                     surface->height, QImage::Format_ARGB32);
            o_ctr->m_background_window->set_background(bg_image);
			
			a_img->on_image_saved([&](const std::string &a_file_name) {
		        o_ctr->m_background_texture = "file:///" + a_file_name;
		        viewport()->update_session_value(controller_name(), "", "");
			});
			
			a_img->save(surface, "wallpaper");
          }
        } else {
          std::cout << __FUNCTION__ << "Error creating Image:" << std::endl;
        }
      });

      image->create((response.data_buffer()), response.data_buffer_size());
    } else {
      std::cout << __FUNCTION__ << "Error downloading url :" << a_url
                << std::endl;
    }
  });

  request->submit(social_kit::url_request::kGETRequest, a_url);
}

void desktop_controller_impl::open_online_dialog() {
  QRectF dialog_window_geometry(0, 0, 520, 340);
  QPointF qt_activity_window_location = viewport()->center(
      dialog_window_geometry, QRectF(), cherry_kit::space::kCenterOnViewport);

  cherry_kit::desktop_dialog_ref ck_activity =
          viewport()->create_activity("pixabay_dialog");

  ck_activity->on_notify([=](const std::string &key, const std::string &value) {
    if (key.compare("url") == 0) {
      std::cout << __FUNCTION__ << "url : " << value << std::endl;
	  
      o_ctr->m_background_texture = "file:///" + value;
	  o_ctr->m_background_window->set_background(value);
      viewport()->update_session_value(controller_name(), "", "");
      viewport()->update_background_texture();
    }
  });
}

void desktop_controller_impl::create_task_group() {
  o_ctr->m_supported_action.set_name("Configure");
  o_ctr->m_supported_action.set_icon("navigation/ck_configure.png");
  o_ctr->m_supported_action.set_visible(1);
  o_ctr->m_supported_action.set_controller(controller_name().toStdString());

  cherry_kit::ui_action bg_task;
  bg_task.set_name("Desktop");
  bg_task.set_icon("navigation/ck_desktop.png");
  bg_task.set_visible(true);
  bg_task.set_task([=](const cherry_kit::ui_action *a_action_ref,
                       const cherry_kit::ui_task_data_t &a_data) {
    if (!viewport())
      return;

    open_background_dialog();
  });

  cherry_kit::ui_action seamless_task;
  seamless_task.set_name("Hide");
  seamless_task.set_icon("navigation/ck_expose.png");
  seamless_task.set_id(1);
  seamless_task.set_visible(1);
  seamless_task.set_task([this](const cherry_kit::ui_action *a_ref,
                                const cherry_kit::ui_task_data_t &a_data) {
    expose_platform_desktop();
  });

  cherry_kit::ui_action online_task;
  online_task.set_name("Pixabay");
  online_task.set_icon("navigation/ck_pixabay.png");
  online_task.set_id(1);
  online_task.set_visible(1);
  online_task.set_task([this](const cherry_kit::ui_action *a_ref,
                              const cherry_kit::ui_task_data_t &a_data) {
    open_online_dialog();
  });

  cherry_kit::ui_action dock_task;
  dock_task.set_name("Dock");
  dock_task.set_icon("navigation/ck_computer.png");
  dock_task.set_id(1);
  dock_task.set_visible(1);
  dock_task.set_task([this](const cherry_kit::ui_action *a_ref,
                            const cherry_kit::ui_task_data_t &a_data) {
    cherry_kit::window *ck_window = new cherry_kit::window();
    ck_window->set_geometry(QRectF(0, 0, 400, 400));
    ck_window->set_window_title("Dock Settings");

    ck_window->on_window_discarded([=](cherry_kit::window *a_window) {
      if (a_window)
        delete a_window;
    });

    // setup ui
    cherry_kit::fixed_layout *ck_ui = new cherry_kit::fixed_layout(ck_window);
    ck_ui->set_content_margin(0, 0, 10, 10);

    QPixmap previw_img =
        viewport()->owner_workspace()->thumbnail(viewport(), 2);

    ck_ui->set_geometry(0, 0, 320, 240);

    ck_ui->add_rows(2);

    ck_ui->add_segments(0, 1);
    ck_ui->add_segments(1, 4);

    ck_ui->set_row_height(0, "90%");
    ck_ui->set_row_height(1, "10%");

    cherry_kit::widget_properties_t ck_ui_data;
    cherry_kit::image_view *preview_view =
        dynamic_cast<cherry_kit::image_view *>(
            ck_ui->add_widget(0, 0, "image_view", ck_ui_data, [=]() {}));
    preview_view->set_pixmap(previw_img);

    ck_ui_data["icon"] = "toolbar/ck_left_arrow.png";
    cherry_kit::icon_button *btn_left = dynamic_cast<cherry_kit::icon_button *>(
        ck_ui->add_widget(1, 0, "image_button", ck_ui_data, [=]() {}));

    ck_ui_data["icon"] = "toolbar/ck_up_arrow.png";
    cherry_kit::icon_button *btn_up = dynamic_cast<cherry_kit::icon_button *>(
        ck_ui->add_widget(1, 1, "image_button", ck_ui_data, [=]() {}));

    ck_ui_data["icon"] = "toolbar/ck_down_arrow.png";
    cherry_kit::icon_button *btn_down = dynamic_cast<cherry_kit::icon_button *>(
        ck_ui->add_widget(1, 2, "image_button", ck_ui_data, [=]() {}));

    ck_ui_data["icon"] = "toolbar/ck_right_arrow.png";
    cherry_kit::icon_button *btn_right =
        dynamic_cast<cherry_kit::icon_button *>(
            ck_ui->add_widget(1, 3, "image_button", ck_ui_data, [=]() {}));

    ck_window->set_window_content(ck_ui->viewport());

    insert(ck_window);
    ck_window->setPos(viewport()->center(QRectF(0, 0, 400, 400), QRectF(),
                                         cherry_kit::space::kCenterOnViewport));
  });

  o_ctr->m_supported_action.add_action(bg_task);
  o_ctr->m_supported_action.add_action(online_task);
  o_ctr->m_supported_action.add_action(dock_task);
  o_ctr->m_supported_action.add_action(seamless_task);
}

cherry_kit::ui_action desktop_controller_impl::task() {
  return o_ctr->m_supported_action;
}

void desktop_controller_impl::expose_platform_desktop() const {
  bool _is_seamless_set = false;

  if (o_ctr->m_background_window) {
    _is_seamless_set = o_ctr->m_background_window->is_seamless();
  }

  if (viewport() && viewport()->owner_workspace()) {
    cherry_kit::workspace *ck_workspace = viewport()->owner_workspace();

    if (ck_workspace) {
      if (ck_workspace->is_accelerated_rendering_on()) {
        //todo do a desktop notification
        return;
      }
#ifdef Q_OS_WIN
      cherry_kit::system_window_context::get()->hide_native_desktop();
#endif
    }
  }

  if (o_ctr->m_background_window)
    o_ctr->m_background_window->set_seamless(
        !o_ctr->m_background_window->is_seamless());

   viewport()->update_background_texture();
}

/*
void desktop_controller_impl::request_action(const QString &actionName,
                                             const QVariantMap &data) {
        //todo :
        // replace this kind of comparisons with new actions.
  if (actionName == tr("Configure")) {
    if (!viewport())
      return;

    QRectF dialog_window_geometry(0, 0, 272 + 64, 320);
    QPointF qt_activity_window_location = viewport()->center(
        dialog_window_geometry,
        QRectF(o_ctr->m_background_window->x(), o_ctr->m_background_window->y(),
               o_ctr->m_background_window->geometry().width(),
               o_ctr->m_background_window->geometry().height()),
        cherry_kit::space::kCenterOnWindow);

    cherry_kit::desktop_dialog_ref ck_activity =
        viewport()->open_desktop_dialog("desktop_settings_dialog", "Desktop",
                                        qt_activity_window_location,
                                        dialog_window_geometry, QVariantMap());

    ck_activity->on_notify([&](const std::string &key,
                               const std::string &value) {
      qDebug() << Q_FUNC_INFO << key.c_str() << " = " << value.c_str();
      o_ctr->m_background_window->set_background(value);
    });

    return;
  }

  if (actionName == "Seamless") {
    expose_platform_desktop();
    return;
  }
}
*/

void desktop_controller_impl::download_image_from_url(const QUrl &fileUrl) {
#ifdef 	__QT5_TOOLKIT__
  social_kit::AsyncDataDownloader *downloader =
      new social_kit::AsyncDataDownloader(this);

  connect(downloader, SIGNAL(ready()), this, SLOT(image_locally_available()));
  connect(downloader, SIGNAL(progress(float)), this,
          SLOT(onUpdateImageDownloadProgress(float)));

  QVariantMap metaData;
  metaData["url"] = fileUrl.toString();

  downloader->setMetaData(metaData);
  downloader->setUrl(fileUrl);
#endif
}

void desktop_controller_impl::set_desktop_scale_type(
    desktop_window::DesktopScalingMode a_desktop_mode) {}

void desktop_controller_impl::handle_drop_event(cherry_kit::widget * /*widget*/,
                                                QDropEvent *event) {
  if (event->mimeData()->hasImage()) {
    QImage qt_image_data =
        qvariant_cast<QImage>(event->mimeData()->imageData());

    if (!qt_image_data.isNull()) {
      sync_image_data_to_disk(qt_image_data,
                              cherry_kit::config::cache_dir("wallpaper"), true);
    }
    return;
  }

  if (event->mimeData()->urls().count() >= 0) {
    QUrl qt_dropped_file_url = event->mimeData()->urls().value(0);

    if (!_url_is_local_file(qt_dropped_file_url)) {
      download_image_from_url(qt_dropped_file_url);
      return;
    }

    QString qt_dropped_file_name_string = qt_dropped_file_url.toLocalFile();
    QFileInfo qt_dropped_file_info(qt_dropped_file_name_string);

    if (!qt_dropped_file_info.isDir()) {
      if (o_ctr->m_background_window) {
        o_ctr->m_background_window->set_background(
            qt_dropped_file_name_string.toStdString());
        o_ctr->m_background_texture =
            "file:///" + qt_dropped_file_name_string.toStdString();
        sync_session_data("background", qt_dropped_file_name_string);

        viewport()->update_background_texture();

      }
    }
  }
}

void desktop_controller_impl::set_view_rect(const QRectF &rect) {
  if (o_ctr->m_background_window) {
    QString default_wallpaper_file =
      cherry_kit::resource_manager::instance()->drawable_file_name(
          "mdpi", "desktop/ck_default_wallpaper.png");

    o_ctr->m_background_window->set_contents_geometry(0, 0, rect.width(),
                                                      rect.height());
    o_ctr->m_background_window->set_coordinates(rect.x(), rect.y());
    o_ctr->m_background_window->set_background_size(rect.width(), 
		    rect.height());
    o_ctr->m_background_window->set_background(
      default_wallpaper_file.toStdString());

    viewport()->update_background_texture();
  }
}

void desktop_controller_impl::sync_image_data_to_disk(const QByteArray &data,
                                                      const QString &source,
                                                      bool a_local_file) {
#ifdef __QT5_TOOLKIT__
  // todo:
  // replace with image_io class.
  social_kit::AsyncImageCreator *ck_image_service =
      new social_kit::AsyncImageCreator(this);

  connect(ck_image_service, SIGNAL(ready()), this,
          SLOT(on_image_data_available()));

  QVariantMap ck_meta_data;
  ck_meta_data["url"] = source;
  ck_image_service->setMetaData(ck_meta_data);
  ck_image_service->setData(data, cherry_kit::config::cache_dir("wallpaper"),
                            a_local_file);
  ck_image_service->start();
#endif
}

void desktop_controller_impl::sync_image_data_to_disk(const QImage &data,
                                                      const QString &source,
                                                      bool saveLocally) {
#ifdef __QT5_TOOLKIT__
  social_kit::AsyncImageCreator *ck_async_image_service =
      new social_kit::AsyncImageCreator(this);

  connect(ck_async_image_service, SIGNAL(ready()), this,
          SLOT(on_image_data_available()));

  QVariantMap ck_meta_data;
  ck_meta_data["url"] = source;
  ck_async_image_service->setMetaData(ck_meta_data);
  ck_async_image_service->setData(
      data, cherry_kit::config::cache_dir("wallpaper"), saveLocally);
  ck_async_image_service->start();
#endif
}

void desktop_controller_impl::image_locally_available() {
#ifdef __QT5_TOOLKIT__
  social_kit::AsyncDataDownloader *downloader =
      qobject_cast<social_kit::AsyncDataDownloader *>(sender());

  if (downloader) {
    sync_image_data_to_disk(downloader->data(),
                            downloader->metaData()["url"].toString(), true);
    downloader->deleteLater();
  }
#endif
}

void desktop_controller_impl::on_image_data_available() {
#ifdef __QT5_TOOLKIT__
  social_kit::AsyncImageCreator *ck_image_service =
      qobject_cast<social_kit::AsyncImageCreator *>(sender());

  if (ck_image_service) {
    if (ck_image_service->image().isNull()) {
      ck_image_service->quit();
      ck_image_service->deleteLater();
      return;
    }

    if (o_ctr->m_background_window) {
      o_ctr->m_background_window->set_background(ck_image_service->image());
      viewport()->update_background_texture();
    }

    if (viewport()) {
      if (!ck_image_service->offline()) {
        o_ctr->m_background_texture =
            ck_image_service->metaData()["url"].toString().toStdString();
        sync_session_data("background",
                          ck_image_service->metaData()["url"].toString());
      } else {
        o_ctr->m_background_texture = QDir::toNativeSeparators(
            "file:///" + ck_image_service->imagePath()).toStdString();
        sync_session_data("background",
                          QDir::toNativeSeparators(
                              "file:///" + ck_image_service->imagePath()));
      }
    }

    ck_image_service->quit();
    ck_image_service->deleteLater();
  }
#endif
}

void desktop_controller_impl::set_desktop_scale_type(const QString &a_action) {}

void desktop_controller_impl::prepare_removal() {}

void desktop_controller_impl::sync_session_data(const QString &key,
                                                const QVariant &value) {
  if (!viewport())
    return;

  viewport()->update_session_value(controller_name(), "", "");
}

void desktop_controller_impl::PrivateBackgroundController::save_session(
    const QString &a_key, const QString &a_value) {}

void desktop_controller_impl::PrivateBackgroundController::updateProgress(
    float progress) {}
