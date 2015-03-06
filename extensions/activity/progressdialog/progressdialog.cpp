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
#include "progressdialog.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <progressbar.h>

class ProgressDialogActivity::PrivateProgressDialog
{
public:
  PrivateProgressDialog() {}
  ~PrivateProgressDialog() {}

  UIKit::Window *mFrame;
  UIKit::ProgressBar *m_progress_bar_widget;
  int mMax;
  int mMin;
  bool m_task_completed;
};

ProgressDialogActivity::ProgressDialogActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateProgressDialog) {}

ProgressDialogActivity::~ProgressDialogActivity()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void ProgressDialogActivity::createWindow(const QRectF &window_geometry,
    const QString &window_title,
    const QPointF &window_pos)
{
  qDebug() << Q_FUNC_INFO << window_geometry;
  qDebug() << Q_FUNC_INFO << window_pos;

  d->mFrame = new UIKit::Window();
  setGeometry(window_geometry);

  d->mFrame->setLabelName("Progress Dialog");
  d->mFrame->setWindowFlag(UIKit::Widget::kRenderBackground, true);
  d->mFrame->setWindowFlag(UIKit::Widget::kConvertToWindowType, true);
  d->mFrame->setWindowFlag(UIKit::Widget::kRenderDropShadow, true);

  d->m_task_completed = 0;

  connect(d->mFrame, SIGNAL(closed(UIKit::Widget *)), this,
          SLOT(onWidgetClosed(UIKit::Widget *)));

  d->mMax = 100.0;
  d->mMin = 0.0;

  if (hasAttribute("max")) {
    d->mMax = attributes()["max"].toFloat();
  }

  if (hasAttribute("min")) {
    d->mMin = attributes()["min"].toFloat();
  }

  d->m_progress_bar_widget = new UIKit::ProgressBar(d->mFrame);
  d->m_progress_bar_widget->setRange(d->mMin, d->mMax);
  d->m_progress_bar_widget->setSize(QSize(window_geometry.width() - 10, 32));
  d->m_progress_bar_widget->setValue(0.0);
  d->m_progress_bar_widget->setPos(5.0, 64.0);
  d->m_progress_bar_widget->show();

  updateContentGeometry(d->mFrame);
  exec(window_pos);

  showActivity();

  d->mFrame->onWindowDiscarded([this](UIKit::Window *aWindow) {
      discardActivity();
  });
}

QVariantMap ProgressDialogActivity::result() const { return QVariantMap(); }

void ProgressDialogActivity::updateAttribute(const QString &name,
    const QVariant &data)
{
  if (!d->mFrame) {
    return;
  }

  if (d->m_task_completed) {
    return;
  }

  float progress = data.toFloat();

  if (d->m_progress_bar_widget) {
    d->m_progress_bar_widget->setValue(progress);
  }

  if (d->mMax == progress) {
    discardActivity();
  }
}

UIKit::Window *ProgressDialogActivity::window() const { return d->mFrame; }

void ProgressDialogActivity::cleanup()
{
    if (d->mFrame)
        delete d->mFrame;
    d->mFrame = 0;
}

void ProgressDialogActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void ProgressDialogActivity::onHideAnimationFinished()
{
  d->m_task_completed = 1;
  Q_EMIT finished();
}
