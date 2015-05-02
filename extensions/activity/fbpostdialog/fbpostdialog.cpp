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
#include "fbpostdialog.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>
#include <webkitwebview.h>
#include <desktopwidget.h>
#include <QGraphicsWebView>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <cookiejar.h>
#include <config.h>

class FBPostDialogActivity::PrivateFBPostDialog {
public:
  PrivateFBPostDialog() {}
  ~PrivateFBPostDialog() {}

  // QPropertyAnimation *mMotionAnim;
  PlexyDesk::Window *mFrame;
  // PlexyDesk::WebKitWebView *mWebView;
  QGraphicsWebView *mWebView;

  QNetworkAccessManager *mNm;
};

FBPostDialogActivity::FBPostDialogActivity(QGraphicsObject *object)
    : PlexyDesk::DesktopActivity(object), d(new PrivateFBPostDialog) {}

FBPostDialogActivity::~FBPostDialogActivity() { delete d; }

void FBPostDialogActivity::createWindow(const QRectF &window_geometry,
                                        const QString &window_title,
                                        const QPointF &window_pos) {
  d->mFrame = new PlexyDesk::Window();
  d->mFrame->setGeometry(geometry());
  d->mFrame->setVisible(true);
  d->mFrame->setLabelName("Message Dialog");

  if (hasAttribute("title")) {
    d->mFrame->setWindowTitle(attributes()["title"].toString());
  }

  d->mFrame->setWindowFlag(PlexyDesk::Window::kRenderBackground);
  d->mFrame->setWindowFlag(PlexyDesk::Window::kTopLevelWindow);
  d->mFrame->setWindowFlag(PlexyDesk::Window::kConvertToWindowType);
  d->mFrame->setWindowFlag(PlexyDesk::Window::kRenderWindowTitle);
  d->mFrame->setWindowFlag(PlexyDesk::Window::kRenderDropShadow);

  exec();
  this->showActivity();
  // todo: invoke UI

  connect(d->mFrame, SIGNAL(closed(PlexyDesk::Widget *)), this,
          SLOT(onWidgetClosed(PlexyDesk::Widget *)));

  d->mNm = PlexyDesk::Config::getNetworkAccessManager();

  d->mWebView = new QGraphicsWebView(d->mFrame);
  d->mWebView->page()->setNetworkAccessManager(d->mNm);

  QRectF webRect(0.0, 0.0, this->geometry().width(),
                 this->geometry().height() - 64);

  d->mWebView->setGeometry(webRect);
  d->mWebView->setPos(QPoint(0.0, 72.0));
  d->mWebView->setResizesToContents(false);
  d->mWebView->show();
  QString token = attributes()["token"].toString();
  QString urlStr(QString("https://www.facebook.com/dialog/"
                         "feed?app_id=170356722999159&display=popup&caption=An%"
                         "20example%20caption&link=https%3A%2F%2Fdevelopers."
                         "facebook.com%2Fdocs%2Fdialogs%2F&redirect_uri=http://"
                         "www.facebook.com/connect/login_success.html"));
  qDebug() << Q_FUNC_INFO << urlStr;
  d->mWebView->setUrl(QUrl(urlStr));
}

QRectF FBPostDialogActivity::geometry() const {
  return QRectF(0.0, 0.0, 480, 480);
}

PlexyDesk::Widget *FBPostDialogActivity::window() const { return d->mFrame; }

QVariantMap FBPostDialogActivity::result() const { return QVariantMap(); }

void FBPostDialogActivity::onWidgetClosed(PlexyDesk::Widget *widget) {
  connect(this, SIGNAL(discarded()), this, SLOT(onMotionAnimFinished()));
  this->discardActivity();
}

void FBPostDialogActivity::onMotionAnimFinished() { Q_EMIT finished(); }
