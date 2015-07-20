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

// social
#include <webservice.h>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>

// Qt
#include <QDir>
#include <QMimeData>
#include <QPixmap>
#include <QAction>
#include <QPainter>

#include <ck_space.h>
#include <ck_workspace.h>

#include "desktopwindow.h"

class desktop_controller_impl::PrivateBackgroundController {
public:
  PrivateBackgroundController() {}

  ~PrivateBackgroundController() {}

  QAction *add_action(desktop_controller_impl *controller, const QString &name,
                      const QString &icon, int id);
  void save_session(const QString &a_key, const QString &a_value);

  void updateProgress(float progress);

  cherry_kit::ActionList m_supported_actions;

  std::string m_background_texture;
  desktop_window *m_background_window;
};

desktop_controller_impl::desktop_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      o_ctr(new PrivateBackgroundController) {}

desktop_controller_impl::~desktop_controller_impl() { delete o_ctr; }

void desktop_controller_impl::init() {
  // todo : port toNativeSeperator to our datakit
  QString default_wallpaper_file = QDir::toNativeSeparators(
      cherry_kit::config::instance()->prefix() +
      QString("/share/plexy/themepack/default/resources/default-16x9.png"));

  o_ctr->m_background_texture = default_wallpaper_file.toStdString();
  o_ctr->m_background_window = new desktop_window();
  o_ctr->m_background_window->set_controller(this);
  o_ctr->m_background_window->set_background(default_wallpaper_file);

  o_ctr->m_background_window->on_window_discarded([this](
      cherry_kit::window *a_window) {
    if (o_ctr->m_background_window)
      delete o_ctr->m_background_window;
  });

  o_ctr->add_action(this, tr("Desktop"), "pd_background_frame_icon.png", 1);

  insert(o_ctr->m_background_window);
}

void desktop_controller_impl::revoke_session(const QVariantMap &args) {
  if (o_ctr->m_background_window)
    return;

  QUrl qt_image_url = args["background"].toString();
  QString qt_mode_string = args["mode"].toString();

  if (qt_image_url.isEmpty())
    return;

  if (qt_image_url.isLocalFile()) {
    o_ctr->m_background_window->set_background(qt_image_url.toLocalFile());
    o_ctr->m_background_texture = qt_image_url.toString().toStdString();
  } else {
    download_image_from_url(qt_image_url);
  }
}

void desktop_controller_impl::session_data_available(
    const cherry_kit::sync_object &a_session_root) {
  if (!a_session_root.has_property("background") ||
      !a_session_root.has_property("mode"))
    return;

  std::string background_url_str = a_session_root.property("background");
  QUrl qt_background_url = QUrl(background_url_str.c_str());
  QString qt_mode_string = a_session_root.property("mode").c_str();

  if (qt_background_url.isEmpty())
    return;

  if (qt_background_url.isLocalFile()) {
    o_ctr->m_background_window->set_background(qt_background_url.toLocalFile());
    o_ctr->m_background_texture = background_url_str;
  } else {
    download_image_from_url(qt_background_url);
  }
}

void
desktop_controller_impl::submit_session_data(cherry_kit::sync_object *a_object) {
  a_object->set_property("background", o_ctr->m_background_texture);
  a_object->set_property("mode", "scale");
}

cherry_kit::ActionList desktop_controller_impl::actions() const {
  return o_ctr->m_supported_actions;
}

void desktop_controller_impl::expose_platform_desktop() {
  bool _is_seamless_set = false;

  if (o_ctr->m_background_window) {
    _is_seamless_set = o_ctr->m_background_window->is_seamless();
  }

  if (viewport() && viewport()->owner_workspace()) {
    cherry_kit::workspace *ck_workspace = viewport()->owner_workspace();

    if (ck_workspace) {
#ifdef Q_OS_WIN
      if (!_is_seamless_set) {
        SetParent((HWND)ck_workspace->winId(), NULL);
      } else {
        HWND hShellWnd = GetShellWindow();
        HWND hDefView =
            FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
        HWND hFolderView =
            FindWindowEx(hDefView, NULL, _T("SysListView32"), NULL);

        SetParent((HWND)ck_workspace->winId(), hFolderView);
      }
#endif
    }
  }

  if (o_ctr->m_background_window)
    o_ctr->m_background_window->set_seamless(
        !o_ctr->m_background_window->is_seamless());
}

void desktop_controller_impl::request_action(const QString &actionName,
                                          const QVariantMap &data) {
  if (actionName == "Desktop") {
    if (!viewport())
      return;

    QRectF dialog_window_geometry(0, 0, 605, 480);
    QPointF qt_activity_window_location = viewport()->center(
        dialog_window_geometry,
        QRectF(o_ctr->m_background_window->x(), o_ctr->m_background_window->y(),
               o_ctr->m_background_window->geometry().width(),
               o_ctr->m_background_window->geometry().height()),
        cherry_kit::space::kCenterOnWindow);

    cherry_kit::desktop_dialog_ref activity = viewport()->open_desktop_dialog(
        "desktop_settings_dialog", "Desktop", qt_activity_window_location,
        dialog_window_geometry, QVariantMap());

    activity->on_action_completed([=](const QVariantMap &a_data) {});
    return;
  }

  if (actionName == "Seamless") {
    expose_platform_desktop();
    return;
  }
}

void desktop_controller_impl::download_image_from_url(QUrl fileUrl) {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      new QuetzalSocialKit::AsyncDataDownloader(this);

  connect(downloader, SIGNAL(ready()), this, SLOT(image_locally_available()));
  connect(downloader, SIGNAL(progress(float)), this,
          SLOT(onUpdateImageDownloadProgress(float)));

  QVariantMap metaData;
  metaData["url"] = fileUrl.toString();

  downloader->setMetaData(metaData);
  downloader->setUrl(fileUrl);
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

    if (!qt_dropped_file_url.isLocalFile()) {
      download_image_from_url(qt_dropped_file_url);
      return;
    }

    QString qt_dropped_file_name_string = qt_dropped_file_url.toLocalFile();
    QFileInfo qt_dropped_file_info(qt_dropped_file_name_string);

    if (!qt_dropped_file_info.isDir()) {
      if (o_ctr->m_background_window) {
        o_ctr->m_background_window->set_background(qt_dropped_file_name_string);
        o_ctr->m_background_texture = qt_dropped_file_name_string.toStdString();
        sync_session_data("background", qt_dropped_file_name_string);
      }
    }
  }
}

void desktop_controller_impl::set_view_rect(const QRectF &rect) {
  if (o_ctr->m_background_window) {
    o_ctr->m_background_window->resize(rect.width(), rect.height());
    o_ctr->m_background_window->setPos(rect.x(), rect.y());
  }
}

void desktop_controller_impl::sync_image_data_to_disk(const QByteArray &data,
                                                   const QString &source,
                                                   bool a_local_file) {
  QuetzalSocialKit::AsyncImageCreator *ck_image_service =
      new QuetzalSocialKit::AsyncImageCreator(this);

  connect(ck_image_service, SIGNAL(ready()), this,
          SLOT(on_image_data_available()));

  QVariantMap ck_meta_data;
  ck_meta_data["url"] = source;
  ck_image_service->setMetaData(ck_meta_data);
  ck_image_service->setData(data, cherry_kit::config::cache_dir("wallpaper"),
                            a_local_file);
  ck_image_service->start();
}

void desktop_controller_impl::sync_image_data_to_disk(const QImage &data,
                                                   const QString &source,
                                                   bool saveLocally) {
  QuetzalSocialKit::AsyncImageCreator *ck_async_image_service =
      new QuetzalSocialKit::AsyncImageCreator(this);

  connect(ck_async_image_service, SIGNAL(ready()), this,
          SLOT(on_image_data_available()));

  QVariantMap ck_meta_data;
  ck_meta_data["url"] = source;
  ck_async_image_service->setMetaData(ck_meta_data);
  ck_async_image_service->setData(
      data, cherry_kit::config::cache_dir("wallpaper"), saveLocally);
  ck_async_image_service->start();
}

void desktop_controller_impl::image_locally_available() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    sync_image_data_to_disk(downloader->data(),
                            downloader->metaData()["url"].toString(), true);
    downloader->deleteLater();
  }
}

void desktop_controller_impl::on_image_data_available() {
  QuetzalSocialKit::AsyncImageCreator *ck_image_service =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (ck_image_service) {
    if (ck_image_service->image().isNull()) {
      ck_image_service->quit();
      ck_image_service->deleteLater();
      return;
    }

    if (o_ctr->m_background_window) {
      o_ctr->m_background_window->set_background(ck_image_service->image());
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
}

void desktop_controller_impl::set_desktop_scale_type(const QString &a_action) {}

QString desktop_controller_impl::icon() const {
  return QString("pd_home_sym_icon.png");
}

QString desktop_controller_impl::label() const { return QString(tr("Desktop")); }

void desktop_controller_impl::prepare_removal() {}

void desktop_controller_impl::sync_session_data(const QString &key,
                                             const QVariant &value) {
  if (!viewport())
    return;

  viewport()->update_session_value(controller_name(), "", "");
}

QAction *desktop_controller_impl::PrivateBackgroundController::add_action(
    desktop_controller_impl *controller, const QString &name, const QString &icon,
    int id) {
  QAction *qt_action = new QAction(controller);
  qt_action->setText(name);
  qt_action->setProperty("id", QVariant(id));
  qt_action->setProperty("icon_name", icon);

  m_supported_actions << qt_action;

  return qt_action;
}

void desktop_controller_impl::PrivateBackgroundController::save_session(
    const QString &a_key, const QString &a_value) {}

void desktop_controller_impl::PrivateBackgroundController::updateProgress(
    float progress) {}
