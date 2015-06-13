/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include "photosearch.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <QDir>
#include <QFileSystemModel>

#include <plexyconfig.h>
#include <view_controller.h>
#include <tableview.h>
#include <default_table_component.h>
#include <progressbar.h>
#include <default_table_model.h>
#include "imagecell.h"
#include "imagecelladaptor.h"
#include <asyncimageloader.h>
#include <asyncimagecreator.h>

#include "localwallpapers.h"

class PhotoSearchActivity::PrivatePhotoSearch {
public:
  PrivatePhotoSearch() {}
  ~PrivatePhotoSearch() {}

  UIKit::Window *mWindowFrame;
  UIKit::ProgressBar *mProgressBar;
  UIKit::TableView *mTable;
  ImageCellAdaptor *mFactory;
  QRectF mGeometry;
  QVariantMap mResult;
};

PhotoSearchActivity::PhotoSearchActivity(QGraphicsObject *object)
    : UIKit::DesktopActivity(object), d(new PrivatePhotoSearch) {}

PhotoSearchActivity::~PhotoSearchActivity() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void PhotoSearchActivity::create_window(const QRectF &aWindowGeometry,
                                        const QString &aWindowTitle,
                                        const QPointF &window_pos) {
  d->mGeometry = aWindowGeometry;

  // todo: invoke UI
  d->mWindowFrame = new UIKit::Window();
  d->mWindowFrame->setGeometry(aWindowGeometry);
  d->mWindowFrame->set_window_title(aWindowTitle);

  // table
  d->mTable = new UIKit::TableView(d->mWindowFrame);
  d->mFactory = new ImageCellAdaptor(d->mWindowFrame);

  d->mTable->set_model(d->mFactory);
  d->mTable->setGeometry(aWindowGeometry);

  d->mWindowFrame->set_window_content(d->mTable);

  connect(d->mTable, SIGNAL(activated(TableViewItem *)), this,
          SLOT(onClicked(TableViewItem *)));
  connect(d->mFactory, SIGNAL(completed(int)), this,
          SLOT(onProgressValue(int)));

  QTimer::singleShot(500, this, SLOT(locateLocalFiles()));

  exec(window_pos);
}

QRectF PhotoSearchActivity::geometry() const { return d->mGeometry; }

QVariantMap PhotoSearchActivity::result() const { return d->mResult; }

Window *PhotoSearchActivity::window() const { return d->mWindowFrame; }

void PhotoSearchActivity::cleanup() {
  if (d->mWindowFrame) {
    delete d->mWindowFrame;
  }

  d->mWindowFrame = 0;
}

void PhotoSearchActivity::onShowAnimationFinished() {
  this->locateLocalFiles();
}

void PhotoSearchActivity::onClicked(TableViewItem *item) {
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

void PhotoSearchActivity::onProgressValue(int value) {
  if (value == 100) {
    if (d->mWindowFrame) {
      if (has_attribute("title")) {
      }
    }
  }
}

void PhotoSearchActivity::locateLocalFiles() const {
  QStringList pathList;

#ifdef Q_OS_LINUX
  pathList << QLatin1String("/usr/share/backgrounds/");
  pathList << QLatin1String("/usr/share/backgrounds/gnome");
#elif defined(Q_OS_MAC)
  pathList << QLatin1String("/Library/Desktop Pictures/");
#elif defined(Q_OS_WIN)
  pathList << QLatin1String("C:\\Windows\\Web\\Wallpaper\\Theme1\\");
  pathList << QLatin1String("C:\\Windows\\Web\\Wallpaper\\Theme2\\");
  pathList << QLatin1String("C:\\Windows\\Web\\Wallpaper\\Windows\\");
#endif

  pathList << UIKit::Config::cache_dir("wallpaper");

  d->mFactory->addPathList(pathList);
}
