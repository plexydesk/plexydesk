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
  // QVariantMap m_session_data;
  std::map<std::string, std::string> m_session_data;

  DesktopWindow *m_background_window;
};

BackgroundController::BackgroundController(QObject *object)
    : CherryKit::ViewController(object),
      p_ctr(new PrivateBackgroundController) {}

BackgroundController::~BackgroundController() { delete p_ctr; }

void BackgroundController::init() {
  // todo : port toNativeSeperator to our datakit
  QString default_wallpaper_file = QDir::toNativeSeparators(
      CherryKit::Config::instance()->prefix() +
      QString("/share/plexy/themepack/default/resources/default-16x9.png"));

  p_ctr->m_background_texture = default_wallpaper_file.toStdString();
  p_ctr->m_background_window = new DesktopWindow();
  p_ctr->m_background_window->set_controller(this);
  p_ctr->m_background_window->set_background(default_wallpaper_file);

  p_ctr->m_background_window->on_window_discarded([this](
      CherryKit::Window *a_window) {
    if (p_ctr->m_background_window)
      delete p_ctr->m_background_window;
  });

  p_ctr->add_action(this, tr("Desktop"), "pd_background_frame_icon.png", 1);
  // p_ctr->add_action(this, tr("Search"), "pd_search_frame_icon.png", 2);
  // p_ctr->add_action(this, tr("Adjust"), "pd_adjust_frame_icon.png", 3);
  // p_ctr->add_action(this, tr("Seamless"), "pd_eye_frame_icon.png", 4);

  insert(p_ctr->m_background_window);
}

void BackgroundController::revoke_session(const QVariantMap &args) {
  QUrl backgroundUrl = args["background"].toString();
  QString mode = args["mode"].toString();

  if (backgroundUrl.isEmpty()) {
    return;
  }

  if (backgroundUrl.isLocalFile()) {
    p_ctr->m_background_window->set_background(backgroundUrl.toLocalFile());
    p_ctr->m_background_texture = backgroundUrl.toString().toStdString();
  } else {
    downloadRemoteFile(backgroundUrl);
  }
}

void BackgroundController::session_data_available(
    const QuetzalKit::SyncObject &a_session_root) {
  if (!a_session_root.has_property("background") ||
      !a_session_root.has_property("mode"))
    return;

  std::string background_url_str = a_session_root.property("background");
  QUrl background_url = QUrl(background_url_str.c_str());
  QString mode = a_session_root.property("mode").c_str();

  if (background_url.isEmpty()) {
    return;
  }

  if (background_url.isLocalFile()) {
    p_ctr->m_background_window->set_background(background_url.toLocalFile());
    p_ctr->m_background_texture = background_url_str;
  } else {
    downloadRemoteFile(background_url);
  }
}

void
BackgroundController::submit_session_data(QuetzalKit::SyncObject *a_object) {
  a_object->set_property("background", p_ctr->m_background_texture);
  a_object->set_property("mode", "scale");
}

CherryKit::ActionList BackgroundController::actions() const {
  return p_ctr->m_supported_actions;
}

void BackgroundController::createSeamlessDesktop() {
  bool _is_seamless_set = false;

  if (p_ctr->m_background_window) {
    _is_seamless_set = p_ctr->m_background_window->is_seamless();
  }

  if (viewport() && viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
// _workspace->set_accelerated_rendering(!_is_seamless_set);

#ifdef Q_OS_WIN
      if (!_is_seamless_set) {
        SetParent((HWND)_workspace->winId(), NULL);
      } else {
        HWND hShellWnd = GetShellWindow();
        HWND hDefView =
            FindWindowEx(hShellWnd, NULL, _T("SHELLDLL_DefView"), NULL);
        HWND folderView =
            FindWindowEx(hDefView, NULL, _T("SysListView32"), NULL);

        SetParent((HWND)_workspace->winId(), folderView);
      }
#endif
    }
  }

  if (p_ctr->m_background_window)
    p_ctr->m_background_window->set_seamless(
        !p_ctr->m_background_window->is_seamless());
}

void BackgroundController::request_action(const QString &actionName,
                                          const QVariantMap &data) {
  if (actionName == tr("Adjust")) {
    createModeChooser();
  } else if (actionName == tr("Search")) {
    createSearchActivity("flikrsearchactivity", tr("Online Search"),
                         QVariantMap());
  } else if (actionName == tr("Desktop")) {
    createWallpaperActivity("photosearchactivity", tr("Wallpapers"),
                            QVariantMap());
  } else if (actionName == tr("Seamless")) {
    createSeamlessDesktop();
  } else if (actionName == tr("Change Background")) {
    revoke_session(data);
    saveSession("background", QVariant(data["background"].toString()));
  } else if (actionName == tr("Fit Height")) {
  }
}

void BackgroundController::downloadRemoteFile(QUrl fileUrl) {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      new QuetzalSocialKit::AsyncDataDownloader(this);

  connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
  connect(downloader, SIGNAL(progress(float)), this,
          SLOT(onUpdateImageDownloadProgress(float)));

  QVariantMap metaData;
  metaData["url"] = fileUrl.toString();

  downloader->setMetaData(metaData);
  downloader->setUrl(fileUrl);
}

void BackgroundController::createModeChooser() {
  QVariantMap data;

  data["Streach"] = QVariant("pd_home_sym_icon.png");
  data["Tile"] = QVariant("pd_app_store_icon.png");
  data["Frame"] = QVariant("pd_disk_volum_icon.png");
  data["Fit Width"] = QVariant("pd_disk_volum_icon.png");
  data["Fit Height"] = QVariant("pd_disk_volum_icon.png");

  createModeActivity("icongrid", "Desktop Mode", data);
}

void
BackgroundController::setScaleMode(ClassicBackgroundRender::ScalingMode mode) {
  /*
if (d->m_background_render_item) {
  d->m_background_render_item->setBackgroundMode(mode);
}
*/
}

void BackgroundController::handle_drop_event(CherryKit::Widget * /*widget*/,
                                             QDropEvent *event) {
  if (event->mimeData()->hasImage()) {
    QImage image = qvariant_cast<QImage>(event->mimeData()->imageData());

    if (!image.isNull()) {
      saveImageLocally(image, CherryKit::Config::cache_dir("wallpaper"), true);
    }
    return;
  }

  if (event->mimeData()->urls().count() >= 0) {
    QUrl fileUrl = event->mimeData()->urls().value(0);
    QString droppedFile;

    qDebug() << Q_FUNC_INFO << " Dropped Url ->" << event->mimeData()->urls();

    if (fileUrl.isLocalFile()) {
      droppedFile = fileUrl.toLocalFile();
    } else {
      // handle remote files;
      qDebug() << Q_FUNC_INFO << " Dropped Url ->" << fileUrl;
      downloadRemoteFile(fileUrl);
      return;
    }

    QFileInfo info(droppedFile);

    if (!info.isDir()) {
      if (p_ctr->m_background_window) {
        p_ctr->m_background_window->set_background(droppedFile);
        p_ctr->m_background_texture = fileUrl.toString().toStdString();
        saveSession("background", fileUrl.toString());
      }
    }
  }
}

void BackgroundController::set_view_rect(const QRectF &rect) {
  if (p_ctr->m_background_window) {
    p_ctr->m_background_window->resize(rect.width(), rect.height());
    p_ctr->m_background_window->setPos(rect.x(), rect.y());
  }
}

void BackgroundController::saveImageLocally(const QByteArray &data,
                                            const QString &source,
                                            bool saveLocally) {
  QuetzalSocialKit::AsyncImageCreator *imageSave =
      new QuetzalSocialKit::AsyncImageCreator(this);

  connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

  QVariantMap metaData;
  metaData["url"] = source;
  imageSave->setMetaData(metaData);
  imageSave->setData(data, CherryKit::Config::cache_dir("wallpaper"),
                     saveLocally);
  imageSave->start();
}

void BackgroundController::saveImageLocally(const QImage &data,
                                            const QString &source,
                                            bool saveLocally) {
  QuetzalSocialKit::AsyncImageCreator *imageSave =
      new QuetzalSocialKit::AsyncImageCreator(this);

  connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

  QVariantMap metaData;
  metaData["url"] = source;
  imageSave->setMetaData(metaData);
  imageSave->setData(data, CherryKit::Config::cache_dir("wallpaper"),
                     saveLocally);
  imageSave->start();
}

void BackgroundController::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    saveImageLocally(downloader->data(),
                     downloader->metaData()["url"].toString(), true);
    downloader->deleteLater();
  }
}

void BackgroundController::onImageSaveReady() {
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    if (c->image().isNull()) {
      c->quit();
      c->deleteLater();
      return;
    }

    if (p_ctr->m_background_window) {
      p_ctr->m_background_window->set_background(c->image());
    }

    if (viewport()) {
      if (!c->offline()) {
        p_ctr->m_background_texture =
            c->metaData()["url"].toString().toStdString();
        saveSession("background", c->metaData()["url"].toString());
      } else {
        p_ctr->m_background_texture =
            QDir::toNativeSeparators("file:///" + c->imagePath()).toStdString();
        saveSession("background",
                    QDir::toNativeSeparators("file:///" + c->imagePath()));
      }
    }

    /*
    if (d->mCurrentMode.isEmpty() || d->mCurrentMode.isNull()) {
      createModeChooser();
    } else {
      setScaleMode(d->mCurrentMode);
    }
    */

    c->quit();
    c->deleteLater();
  }
}

void BackgroundController::setScaleMode(const QString &action) {
  if (action == "Fit") {
    this->setScaleMode(ClassicBackgroundRender::None);
  } else if (action == "Streach") {
    this->setScaleMode(ClassicBackgroundRender::Streach);
  } else if (action == "Tile") {
    this->setScaleMode(ClassicBackgroundRender::Tile);
  } else if (action == "Frame") {
    this->setScaleMode(ClassicBackgroundRender::Frame);
  } else if (action == "Fit Width") {
    this->setScaleMode(ClassicBackgroundRender::FitWidth);
  } else if (action == "Fit Height") {
    this->setScaleMode(ClassicBackgroundRender::FitHeight);
  } else {
    this->setScaleMode(ClassicBackgroundRender::None);
  }
}

QString BackgroundController::icon() const {
  return QString("pd_home_sym_icon.png");
}

QString BackgroundController::label() const { return QString(tr("Desktop")); }

void BackgroundController::prepare_removal() {}

void BackgroundController::createModeActivity(const QString &activity,
                                              const QString &title,
                                              const QVariantMap &data) {
  if (!viewport()) {
    return;
  }

  QRectF _view_geometry(0.0, 0.0, 420.0, 192.0);

  CherryKit::DesktopActivityPtr intent = viewport()->create_activity(
      activity, title, viewport()->center(_view_geometry), _view_geometry,
      data);
  intent->set_controller(viewport()->controller("classicbackdrop"));
}

void BackgroundController::createWallpaperActivity(const QString &activity,
                                                   const QString &title,
                                                   const QVariantMap &data) {
  if (!viewport()) {
    return;
  }

  QRectF _view_geometry(0.0, 0.0, 600, 480);

  CherryKit::DesktopActivityPtr intent = viewport()->create_activity(
      "photosearchactivity", title, viewport()->center(_view_geometry),
      _view_geometry, data);
  intent->set_controller(viewport()->controller("classicbackdrop"));
}

void BackgroundController::createSearchActivity(const QString &activity,
                                                const QString &title,
                                                const QVariantMap &data) {
  if (!viewport()) {
    return;
  }

  QRectF _activity_geometry(0.0, 0.0, 572, 480);

  CherryKit::DesktopActivityPtr intent = viewport()->create_activity(
      "flikrsearchactivity", title, viewport()->center(_activity_geometry),
      _activity_geometry, data);
  intent->set_controller(viewport()->controller("classicbackdrop"));
}

void BackgroundController::saveSession(const QString &key,
                                       const QVariant &value) {
  p_ctr->m_session_data[key.toStdString()] = value.toString().toStdString();

  if (!viewport())
    return;

  viewport()->update_session_value(controller_name(), "", "");
}

QVariant BackgroundController::sessionValue(const QString &key) {
  return QVariant();
}

QAction *BackgroundController::PrivateBackgroundController::add_action(
    BackgroundController *controller, const QString &name, const QString &icon,
    int id) {
  QAction *action = new QAction(controller);
  action->setText(name);
  action->setProperty("id", QVariant(id));
  action->setProperty("icon_name", icon);

  m_supported_actions << action;

  return action;
}

void BackgroundController::PrivateBackgroundController::save_session(
    const QString &a_key, const QString &a_value) {}

void BackgroundController::PrivateBackgroundController::updateProgress(
    float progress) {
  QVariant progressVal = progress;
}
