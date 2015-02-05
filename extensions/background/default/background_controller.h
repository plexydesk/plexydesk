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
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef PLEXY_BACKGROUND_CONTROLLER_H
#define PLEXY_BACKGROUND_CONTROLLER_H

#include <plexy.h>
#include <config.h>
#include <view_controller.h>
#define QT_SHAREDPOINTER_TRACK_POINTERS 1
#include <QSharedPointer>

// self
#include "classicbackgroundrender.h"

class BackgroundController : public UI::ViewController
{
  Q_OBJECT

public:
  BackgroundController(QObject *object = 0);

  virtual ~BackgroundController();

  void init();

  void revokeSession(const QVariantMap &args);

  UI::ActionList actions() const;

  void requestAction(const QString &actionName, const QVariantMap &data);

  void handleDropEvent(UI::Widget *widget, QDropEvent *event);

  void setViewRect(const QRectF &rect);

  QString icon() const;

  QString label() const;

  void configure(const QPointF &pos);

  void prepareRemoval();

  void createSeamlessDesktop();
private Q_SLOTS:
  void onImageReady();

  void onImageSaveReady();

  void onModeActivityFinished();

  void onWallpaperActivityFinished();

  void onSearchFinished();

  void onUpdateImageDownloadProgress(float progress);

  void onConfigureDone();

private:
  void downloadRemoteFile(QUrl fileUrl);

  void saveImageLocally(const QByteArray &data, const QString &source,
                        bool saveLocally = false);

  void saveImageLocally(const QImage &data, const QString &source,
                        bool saveLocally = false);

  void createModeChooser();

  void setScaleMode(ClassicBackgroundRender::ScalingMode mode);

  void setScaleMode(const QString &action);

  void createModeActivity(const QString &activity, const QString &title,
                          const QVariantMap &data);

  void createWallpaperActivity(const QString &activity, const QString &title,
                               const QVariantMap &data);

  void createSearchActivity(const QString &activity, const QString &title,
                            const QVariantMap &data);

  void createProgressDialog();

  void saveSession(const QString &key, const QVariant &value);

  QVariant sessionValue(const QString &key);

  QList<ClassicBackgroundRender *> mBackgroundRenderList;

  const QRectF mBackgroundRect;
  class PrivateBackgroundController;
  PrivateBackgroundController *const d;
};

#endif // PLEXY_BACKGROUND_CONTROLLER_H
