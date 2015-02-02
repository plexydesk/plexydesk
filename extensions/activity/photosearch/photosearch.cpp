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
#include <desktopwidget.h>
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

  PlexyDesk::UIWidget *mFrame;
  PlexyDesk::ProgressBar *mProgressBar;
  PlexyDesk::TableView *mTable;
  ImageCellAdaptor *mFactory;
  QRectF m_frame_geometry;
  QVariantMap mResult;
};

PhotoSearchActivity::PhotoSearchActivity(QGraphicsObject *object)
    : PlexyDesk::DesktopActivity(object), d(new PrivatePhotoSearch) {}

PhotoSearchActivity::~PhotoSearchActivity() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void PhotoSearchActivity::createWindow(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  d->m_frame_geometry = QRectF(0.0, 0.0, 600.0, 480.0);

  // todo: invoke UI
  d->mFrame = new PlexyDesk::UIWidget();
  updateContentGeometry(d->mFrame);
  setGeometry(d->m_frame_geometry);

  d->mFrame->setLabelName("WallPaper");

  d->mFrame->setWindowTitle("Please Wait : Indexing...");

  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderWindowTitle);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow);

  // table
  d->mTable = new PlexyDesk::TableView(d->mFrame);

  d->mFactory = new ImageCellAdaptor(d->mFrame);

  d->mTable->setModel(d->mFactory);
  d->mTable->setGeometry(d->m_frame_geometry);
  d->mTable->setPos(0.0, 72.0);

  connect(d->mTable, SIGNAL(activated(TableViewItem *)), this,
          SLOT(onClicked(TableViewItem *)));
  connect(d->mFrame, SIGNAL(closed(PlexyDesk::Widget *)), this,
          SLOT(onWidgetClosed(PlexyDesk::Widget *)));
  connect(d->mFactory, SIGNAL(completed(int)), this,
          SLOT(onProgressValue(int)));
  QTimer::singleShot(500, this, SLOT(locateLocalFiles()));

  exec(window_pos);
  // showActivity();
}

QRectF PhotoSearchActivity::geometry() const { return d->m_frame_geometry; }

QVariantMap PhotoSearchActivity::result() const { return d->mResult; }

Widget *PhotoSearchActivity::window() const { return d->mFrame; }

void PhotoSearchActivity::onWidgetClosed(PlexyDesk::Widget *widget) {
  if (d->mFactory && d->mFactory->hasRunningThreads())
    return;

  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void PhotoSearchActivity::onHideAnimationFinished() {
  // Q_EMIT finished();
}

void PhotoSearchActivity::onShowAnimationFinished() {
  this->locateLocalFiles();
}

void PhotoSearchActivity::onClicked(TableViewItem *item) {
  if (d->mFactory && d->mFactory->hasRunningThreads())
    return;

  ImageCell *i = qobject_cast<ImageCell *>(item);
  if (i) {
    // qDebug() << Q_FUNC_INFO <<
    // d->mAsyncImageLoader->filePathFromName(i->label());

    d->mResult["action"] = QString("Change Background");
    d->mResult["background"] = "file://" + i->label();

    updateAction();

    connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
    discardActivity();
  }
}

void PhotoSearchActivity::onProgressValue(int value) {
  if (value == 100) {
    // d->mProgressBar->setValue(100);
    // d->mProgressBar->hide();
    if (d->mFrame) {
      if (hasAttribute("title")) {
        d->mFrame->setWindowTitle(attributes()["title"].toString());
      }
    }
  }
}

void PhotoSearchActivity::locateLocalFiles() const {
  QString url;
  QStringList pathList;
#ifdef Q_OS_LINUX
  url = QLatin1String("/usr/share/backgrounds/");
  pathList << url;
  pathList << QLatin1String("/usr/share/backgrounds/gnome");
#endif

#ifdef Q_OS_MAC
  url = QLatin1String("/Library/Desktop Pictures/");
  pathList << url;
#endif

  pathList << PlexyDesk::Config::cacheDir("wallpaper");

  d->mFactory->addPathList(pathList);
}
