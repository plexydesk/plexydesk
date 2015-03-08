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
#include <QPainter>
#include <space.h>
#include <workspace.h>
#include <QAction>

// widondows
#ifdef Q_OS_WIN
#include <Windows.h>
#include <tchar.h>
#endif
// self
#include "classicbackgroundrender.h"

class BackgroundController::PrivateBackgroundController
{
public:
  PrivateBackgroundController() {}

  ~PrivateBackgroundController() {}

  QAction *createAction(BackgroundController *controller, const QString &name,
                        const QString &icon, int id);

  void updateProgress(float progress);

  QString mCurrentMode;

  UIKit::ActionList mActionList;
  ClassicBackgroundRender *m_background_render_item;
};

BackgroundController::BackgroundController(QObject *object)
  : UIKit::ViewController(object),
    d(new PrivateBackgroundController)
{
  d->m_background_render_item = 0;
}

BackgroundController::~BackgroundController()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void BackgroundController::init()
{
  QString wallpaperPath = QDir::toNativeSeparators(
                            UIKit::Config::getInstance()->prefix() +
                            QString("/share/plexy/themepack/default/resources/default-16x9.png"));

  d->m_background_render_item = new ClassicBackgroundRender(
    QRectF(0.0, 0.0, 0.0, 0.0), 0, QImage(wallpaperPath));
  d->m_background_render_item->setController(this);
  d->m_background_render_item->setLabelName("classic Backdrop");
  /*
  d->m_background_render_item->setLayerType(
      UI::UIWidget::kRenderAtBackgroundLevel);
  */

  d->createAction(this, tr("Desktop"), "pd_background_frame_icon.png", 1);
  d->createAction(this, tr("Search"), "pd_search_frame_icon.png", 2);
  d->createAction(this, tr("Adjust"), "pd_adjust_frame_icon.png", 3);
  d->createAction(this, tr("Seamless"), "pd_eye_frame_icon.png", 4);

  insert(d->m_background_render_item);
}

void BackgroundController::revokeSession(const QVariantMap &args)
{
  QUrl backgroundUrl = args["background"].toString();
  QString mode = args["mode"].toString();

  if (backgroundUrl.isEmpty()) {
    return;
  }

  d->mCurrentMode = mode;

  qDebug() << Q_FUNC_INFO << backgroundUrl.isLocalFile();

  if (backgroundUrl.isLocalFile()) {
    d->m_background_render_item->setBackgroundImage(
      QUrl(args["background"].toString()));
  } else {
    downloadRemoteFile(backgroundUrl);
  }
}

UIKit::ActionList BackgroundController::actions() const
{
  return d->mActionList;
}

void BackgroundController::createSeamlessDesktop()
{
  /*
      bool _is_seamless_set = false;

      if (d->m_background_render_item) {
          _is_seamless_set = d->m_background_render_item->isSeamlessModeSet();
      }
      if (viewport() && viewport()->workspace()) {
          UI::WorkSpace *_workspace = qobject_cast<UI::WorkSpace*>
  (viewport()->workspace());

          if (_workspace) {
              _workspace->setAcceleratedRendering(!_is_seamless_set);

  #ifdef Q_OS_WIN
              if (!_is_seamless_set) {
                  SetParent((HWND)_workspace->winId(), NULL);
              } else {
                  HWND hShellWnd = GetShellWindow();
                  HWND hDefView = FindWindowEx(hShellWnd, NULL,
  _T("SHELLDLL_DefView"), NULL);
                  HWND folderView = FindWindowEx(hDefView, NULL,
  _T("SysListView32"), NULL);

                  SetParent((HWND)_workspace->winId(), folderView);
              }
  #endif
          }
      }

  */
  if (d->m_background_render_item)
    d->m_background_render_item->setSeamLessMode(
      !(d->m_background_render_item->isSeamlessModeSet()));
}

void BackgroundController::requestAction(const QString &actionName,
    const QVariantMap &data)
{
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
    revokeSession(data);
    saveSession("background", QVariant(data["background"].toString()));
  } else if (actionName == tr("Fit Height")) {
  }
}

void BackgroundController::downloadRemoteFile(QUrl fileUrl)
{
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

void BackgroundController::createModeChooser()
{
  QVariantMap data;

  data["Streach"] = QVariant("pd_home_sym_icon.png");
  data["Tile"] = QVariant("pd_app_store_icon.png");
  data["Frame"] = QVariant("pd_disk_volum_icon.png");
  data["Fit Width"] = QVariant("pd_disk_volum_icon.png");
  data["Fit Height"] = QVariant("pd_disk_volum_icon.png");

  createModeActivity("icongrid", "Desktop Mode", data);
}

void BackgroundController::setScaleMode(
  ClassicBackgroundRender::ScalingMode mode)
{
  if (d->m_background_render_item) {
    d->m_background_render_item->setBackgroundMode(mode);
  }
}

void BackgroundController::handleDropEvent(UIKit::Widget * /*widget*/,
    QDropEvent *event)
{
  qDebug() << Q_FUNC_INFO;

  if (event->mimeData()->hasImage()) {
    QImage image = qvariant_cast<QImage>(event->mimeData()->imageData());

    if (!image.isNull()) {
      qDebug() << Q_FUNC_INFO << "Request Save Image Locally";
      saveImageLocally(image, UIKit::Config::cacheDir("wallpaper"), true);
    }
    return;
  }

  if (event->mimeData()->urls().count() >= 0) {
    QUrl fileUrl = event->mimeData()->urls().value(0);
    QString droppedFile;

    if (fileUrl.isLocalFile()) {
      droppedFile = fileUrl.toLocalFile();
    } else {
      // handle remote files;
      downloadRemoteFile(fileUrl);
      return;
    }

    QFileInfo info(droppedFile);

    if (!info.isDir()) {
      if (d->m_background_render_item) {
        d->m_background_render_item->setBackgroundImage(droppedFile);
      }

      createModeChooser();

      // todo FIX!
      if (viewport()) {
        UIKit::Space *space = viewport();
        if (space)
          space->update_session_value(controllerName(), "background",
                                      fileUrl.toString());
      }
    }
  }
}

void BackgroundController::setViewRect(const QRectF &rect)
{
  if (d->m_background_render_item) {
    d->m_background_render_item->setBackgroundGeometry(rect);
    // d->m_background_render_item->setGeometry(rect);
    // d->m_background_render_item->setPos(0.0, 0.0);
  }
}

void BackgroundController::saveImageLocally(const QByteArray &data,
    const QString &source,
    bool saveLocally)
{
  QuetzalSocialKit::AsyncImageCreator *imageSave =
    new QuetzalSocialKit::AsyncImageCreator(this);

  connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

  QVariantMap metaData;
  metaData["url"] = source;
  imageSave->setMetaData(metaData);
  imageSave->setData(data, UIKit::Config::cacheDir("wallpaper"),
                     saveLocally);
  imageSave->start();
}

void BackgroundController::saveImageLocally(const QImage &data,
    const QString &source,
    bool saveLocally)
{
  QuetzalSocialKit::AsyncImageCreator *imageSave =
    new QuetzalSocialKit::AsyncImageCreator(this);

  connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

  QVariantMap metaData;
  metaData["url"] = source;
  imageSave->setMetaData(metaData);
  imageSave->setData(data, UIKit::Config::cacheDir("wallpaper"),
                     saveLocally);
  imageSave->start();
}

void BackgroundController::onImageReady()
{
  QuetzalSocialKit::AsyncDataDownloader *downloader =
    qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    saveImageLocally(downloader->data(),
                     downloader->metaData()["url"].toString(), true);
    downloader->deleteLater();
  }
}

void BackgroundController::onImageSaveReady()
{
  QuetzalSocialKit::AsyncImageCreator *c =
    qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    if (c->image().isNull()) {
      c->quit();
      c->deleteLater();
      return;
    }

    if (d->m_background_render_item) {
      d->m_background_render_item->setBackgroundImage(c->image());
    }

    if (viewport()) {
      if (!c->offline()) {
        viewport()->update_session_value(controllerName(), "background",
                                         c->metaData()["url"].toString());
      } else {
        viewport()->update_session_value(
          controllerName(), "background",
          QDir::toNativeSeparators("file://" + c->imagePath()));
      }
    }

    if (d->mCurrentMode.isEmpty() || d->mCurrentMode.isNull()) {
      createModeChooser();
    } else {
      setScaleMode(d->mCurrentMode);
    }

    c->quit();
    c->deleteLater();
  }
}

void BackgroundController::setScaleMode(const QString &action)
{
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

QString BackgroundController::icon() const
{
  return QString("pd_home_sym_icon.png");
}

QString BackgroundController::label() const { return QString(tr("Desktop")); }

void BackgroundController::configure(const QPointF &pos)
{
  QPointF localPos;
  QPointF _activity_pos;

  localPos.setX(pos.x());
  localPos.setY(pos.y());

  _activity_pos.setX(d->m_background_render_item->mapFromScene(localPos).x());
  _activity_pos.setY(d->m_background_render_item->mapFromScene(localPos).y());

  QVariantMap data;

  data["Wallpaper"] = "pd_photo_icon.png";
  data["Search"] = "pd_settings_icon.png";
  data["Adjust"] = "pd_quit_icon.png";
  data["Seamless"] = "pd_desktop_icon.png";

  viewport()->create_activity("icongrind", "Desktop", _activity_pos,
                              QRectF(0, 0, 330, 192), data);
}

void BackgroundController::prepareRemoval()
{
}

// remove
void BackgroundController::onModeActivityFinished()
{
  if (sender()) {
    UIKit::DesktopActivity *activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

    if (activity) {
      QVariantMap resultData = activity->result();
      QString action = resultData["action"].toString();

      setScaleMode(action);
      saveSession("mode", resultData["action"]);
    }
  }
}

// remove
void BackgroundController::onWallpaperActivityFinished()
{
  if (sender()) {
    UIKit::DesktopActivity *activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

    if (activity) {
      QVariantMap resultData = activity->result();

      revokeSession(resultData);
      saveSession("background", QVariant(resultData["background"].toString()));
    }
  }
}

void BackgroundController::onSearchFinished()
{
  if (sender()) {
    UIKit::DesktopActivity *activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

    if (activity) {
      QVariantMap resultData = activity->result();
      qDebug() << Q_FUNC_INFO << resultData;
      revokeSession(resultData);
    }
  }
}

void BackgroundController::onConfigureDone()
{
  if (sender()) {
    UIKit::DesktopActivity *activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

    if (activity) {
      QVariantMap result = activity->result();

      if (result["action"] == "Search") {
        createSearchActivity("flikrsearchactivity", tr("Online Search"),
                             QVariantMap());
      } else if (result["action"] == "Wallpaper") {
        createWallpaperActivity("photosearchactivity", tr("Wallpapers"),
                                QVariantMap());
      } else if (result["action"] == "Adjust") {
        createModeChooser();
      } else if (result["action"] == "Seamless") {
        if (d->m_background_render_item)
          d->m_background_render_item->setSeamLessMode(
            !(d->m_background_render_item->isSeamlessModeSet()));
      }
    }
  }
}

void BackgroundController::createModeActivity(const QString &activity,
    const QString &title,
    const QVariantMap &data)
{
  if (!viewport()) {
    return;
  }

  QRectF _view_geometry(0.0, 0.0, 420.0, 192.0);

  UIKit::DesktopActivityPtr intent = viewport()->create_activity(activity, title,
                                     viewport()->center(_view_geometry),
                                     _view_geometry,
                                     data);
  intent->setController(viewport()->controller("classicbackdrop"));
}

void BackgroundController::createWallpaperActivity(const QString &activity,
    const QString &title,
    const QVariantMap &data)
{
  if (!viewport()) { return; }

  QRectF _view_geometry(0.0, 0.0, 600, 480);

  UIKit::DesktopActivityPtr intent =
    viewport()->create_activity("photosearchactivity", title,
                                viewport()->center(_view_geometry),
                                _view_geometry,
                                data);
  intent->setController(viewport()->controller("classicbackdrop"));
}

void BackgroundController::createSearchActivity(const QString &activity,
    const QString &title,
    const QVariantMap &data)
{
  if (!viewport()) { return; }

  QRectF _activity_geometry(0.0, 0.0, 572, 480);

  UIKit::DesktopActivityPtr intent =
    viewport()->create_activity("flikrsearchactivity", title,
                                viewport()->center(_activity_geometry),
                                _activity_geometry,
                                data);
  intent->setController(viewport()->controller("classicbackdrop"));
}

void BackgroundController::saveSession(const QString &key,
                                       const QVariant &value)
{
  UIKit::Space *view = viewport();

  if (view) {
    view->update_session_value(controllerName(), key, value.toString());
  }
}

QVariant BackgroundController::sessionValue(const QString &key)
{
  return QVariant();
}

QAction *BackgroundController::PrivateBackgroundController::createAction(
  BackgroundController *controller, const QString &name, const QString &icon,
  int id)
{
  QAction *action = new QAction(controller);
  action->setText(name);
  action->setProperty("id", QVariant(id));
  action->setProperty("icon_name", icon);

  mActionList << action;

  return action;
}

void BackgroundController::PrivateBackgroundController::updateProgress(
  float progress)
{
  QVariant progressVal = progress;
}
