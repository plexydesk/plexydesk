/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <plexyconfig.h>
#include <extensionmanager.h>
#include <widget.h>
#include <extensionmanager.h>

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

#include <space.h>
#include <workspace.h>

#include "desktopwindow.h"

class BackgroundController::PrivateBackgroundController {
public:
  PrivateBackgroundController() {}

  ~PrivateBackgroundController() {}

  QAction *add_action(BackgroundController *controller, const QString &name,
                      const QString &icon, int id);
  void save_session(const QString &a_key, const QString &a_value);

  void updateProgress(float progress);

  CherryKit::ActionList m_supported_actions;

  std::string m_background_texture;
  DesktopWindow *m_background_window;
};

BackgroundController::BackgroundController(QObject *object)
    : CherryKit::ViewController(object),
      o_ctr(new PrivateBackgroundController) {}

BackgroundController::~BackgroundController() { delete o_ctr; }

void BackgroundController::init() {
  // todo : port toNativeSeperator to our datakit
  QString default_wallpaper_file = QDir::toNativeSeparators(
      CherryKit::Config::instance()->prefix() +
      QString("/share/plexy/themepack/default/resources/default-16x9.png"));

  o_ctr->m_background_texture = default_wallpaper_file.toStdString();
  o_ctr->m_background_window = new DesktopWindow();
  o_ctr->m_background_window->set_controller(this);
  o_ctr->m_background_window->set_background(default_wallpaper_file);

  o_ctr->m_background_window->on_window_discarded([this](
      CherryKit::Window *a_window) {
    if (o_ctr->m_background_window)
      delete o_ctr->m_background_window;
  });

  o_ctr->add_action(this, tr("Desktop"), "pd_background_frame_icon.png", 1);

  insert(o_ctr->m_background_window);
}

void BackgroundController::revoke_session(const QVariantMap &args) {
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

void BackgroundController::session_data_available(
    const ck::SyncObject &a_session_root) {
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
BackgroundController::submit_session_data(ck::SyncObject *a_object) {
  a_object->set_property("background", o_ctr->m_background_texture);
  a_object->set_property("mode", "scale");
}

CherryKit::ActionList BackgroundController::actions() const {
  return o_ctr->m_supported_actions;
}

void BackgroundController::expose_platform_desktop() {
  bool _is_seamless_set = false;

  if (o_ctr->m_background_window) {
    _is_seamless_set = o_ctr->m_background_window->is_seamless();
  }

  if (viewport() && viewport()->workspace()) {
    CherryKit::WorkSpace *ck_workspace = viewport()->workspace();

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

void BackgroundController::request_action(const QString &actionName,
                                          const QVariantMap &data) {
  if (actionName == "Desktop") {
    if (!viewport())
      return;

    QRectF dialog_window_geometry(0, 0, 640, 640);
    QPointF qt_activity_window_location = viewport()->center(
        dialog_window_geometry,
        QRectF(0, 0, o_ctr->m_background_window->geometry().width(),
               o_ctr->m_background_window->geometry().height()),
        CherryKit::Space::kCenterOnWindow);

    CherryKit::DesktopActivityPtr activity = viewport()->create_activity(
        "system_wallpapers", "Desktop", qt_activity_window_location,
        dialog_window_geometry, QVariantMap());

    activity->on_action_completed([=](const QVariantMap &a_data) {});
    return;
  }

  if (actionName == "Seamless") {
    expose_platform_desktop();
    return;
  }
}

void BackgroundController::download_image_from_url(QUrl fileUrl) {
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

void BackgroundController::set_desktop_scale_type(
    DesktopWindow::DesktopScalingMode a_desktop_mode) {}

void BackgroundController::handle_drop_event(CherryKit::Widget * /*widget*/,
                                             QDropEvent *event) {
  if (event->mimeData()->hasImage()) {
    QImage qt_image_data =
        qvariant_cast<QImage>(event->mimeData()->imageData());

    if (!qt_image_data.isNull()) {
      sync_image_data_to_disk(qt_image_data,
                              CherryKit::Config::cache_dir("wallpaper"), true);
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

void BackgroundController::set_view_rect(const QRectF &rect) {
  if (o_ctr->m_background_window) {
    o_ctr->m_background_window->resize(rect.width(), rect.height());
    o_ctr->m_background_window->setPos(rect.x(), rect.y());
  }
}

void BackgroundController::sync_image_data_to_disk(const QByteArray &data,
                                                   const QString &source,
                                                   bool a_local_file) {
  QuetzalSocialKit::AsyncImageCreator *ck_image_service =
      new QuetzalSocialKit::AsyncImageCreator(this);

  connect(ck_image_service, SIGNAL(ready()), this,
          SLOT(on_image_data_available()));

  QVariantMap ck_meta_data;
  ck_meta_data["url"] = source;
  ck_image_service->setMetaData(ck_meta_data);
  ck_image_service->setData(data, CherryKit::Config::cache_dir("wallpaper"),
                            a_local_file);
  ck_image_service->start();
}

void BackgroundController::sync_image_data_to_disk(const QImage &data,
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
      data, CherryKit::Config::cache_dir("wallpaper"), saveLocally);
  ck_async_image_service->start();
}

void BackgroundController::image_locally_available() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    sync_image_data_to_disk(downloader->data(),
                            downloader->metaData()["url"].toString(), true);
    downloader->deleteLater();
  }
}

void BackgroundController::on_image_data_available() {
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

void BackgroundController::set_desktop_scale_type(const QString &a_action) {}

QString BackgroundController::icon() const {
  return QString("pd_home_sym_icon.png");
}

QString BackgroundController::label() const { return QString(tr("Desktop")); }

void BackgroundController::prepare_removal() {}

void BackgroundController::sync_session_data(const QString &key,
                                             const QVariant &value) {
  if (!viewport())
    return;

  viewport()->update_session_value(controller_name(), "", "");
}

QAction *BackgroundController::PrivateBackgroundController::add_action(
    BackgroundController *controller, const QString &name, const QString &icon,
    int id) {
  QAction *qt_action = new QAction(controller);
  qt_action->setText(name);
  qt_action->setProperty("id", QVariant(id));
  qt_action->setProperty("icon_name", icon);

  m_supported_actions << qt_action;

  return qt_action;
}

void BackgroundController::PrivateBackgroundController::save_session(
    const QString &a_key, const QString &a_value) {}

void BackgroundController::PrivateBackgroundController::updateProgress(
    float progress) {}
