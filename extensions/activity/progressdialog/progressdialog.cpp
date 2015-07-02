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

class ProgressDialogActivity::PrivateProgressDialog {
public:
  PrivateProgressDialog() {}
  ~PrivateProgressDialog() {}

  CherryKit::Window *mFrame;
  CherryKit::ProgressBar *m_progress_bar_widget;
  int mMax;
  int mMin;
  bool m_task_completed;
};

ProgressDialogActivity::ProgressDialogActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateProgressDialog) {}

ProgressDialogActivity::~ProgressDialogActivity() {
  qDebug() << Q_FUNC_INFO;
  delete o_desktop_activity;
}

void ProgressDialogActivity::create_window(const QRectF &window_geometry,
                                           const QString &window_title,
                                           const QPointF &window_pos) {
  qDebug() << Q_FUNC_INFO << window_geometry;
  qDebug() << Q_FUNC_INFO << window_pos;

  o_desktop_activity->mFrame = new CherryKit::Window();
  set_geometry(window_geometry);

  o_desktop_activity->mFrame->set_widget_name("Progress Dialog");
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderBackground, true);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kConvertToWindowType, true);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderDropShadow, true);

  o_desktop_activity->m_task_completed = 0;

  connect(o_desktop_activity->mFrame, SIGNAL(closed(CherryKit::Widget *)), this,
          SLOT(onWidgetClosed(CherryKit::Widget *)));

  o_desktop_activity->mMax = 100.0;
  o_desktop_activity->mMin = 0.0;

  if (has_attribute("max")) {
    o_desktop_activity->mMax = attributes()["max"].toFloat();
  }

  if (has_attribute("min")) {
    o_desktop_activity->mMin = attributes()["min"].toFloat();
  }

  o_desktop_activity->m_progress_bar_widget = new CherryKit::ProgressBar(o_desktop_activity->mFrame);
  o_desktop_activity->m_progress_bar_widget->set_range(o_desktop_activity->mMin, o_desktop_activity->mMax);
  o_desktop_activity->m_progress_bar_widget->set_size(QSize(window_geometry.width() - 10, 32));
  o_desktop_activity->m_progress_bar_widget->set_value(0.0);
  o_desktop_activity->m_progress_bar_widget->setPos(5.0, 64.0);
  o_desktop_activity->m_progress_bar_widget->show();

  update_content_geometry(o_desktop_activity->mFrame);
  exec(window_pos);

  show_activity();
}

QVariantMap ProgressDialogActivity::result() const { return QVariantMap(); }

void ProgressDialogActivity::update_attribute(const QString &name,
                                              const QVariant &data) {
  if (!o_desktop_activity->mFrame) {
    return;
  }

  if (o_desktop_activity->m_task_completed) {
    return;
  }

  float progress = data.toFloat();

  if (o_desktop_activity->m_progress_bar_widget) {
    o_desktop_activity->m_progress_bar_widget->set_value(progress);
  }

  if (o_desktop_activity->mMax == progress) {
    discard_activity();
  }
}

CherryKit::Window *ProgressDialogActivity::window() const { return o_desktop_activity->mFrame; }

void ProgressDialogActivity::cleanup() {
  if (o_desktop_activity->mFrame) {
    delete o_desktop_activity->mFrame;
  }
  o_desktop_activity->mFrame = 0;
}

void ProgressDialogActivity::onWidgetClosed(CherryKit::Widget *widget) {
  discard_activity();
}

void ProgressDialogActivity::onHideAnimationFinished() {
  o_desktop_activity->m_task_completed = 1;
}
