/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_progress_bar.h>

class progress_dialog::PrivateProgressDialog {
public:
  PrivateProgressDialog() {}
  ~PrivateProgressDialog() {}

  cherry_kit::window *mFrame;
  cherry_kit::progress_bar *m_progress_bar_widget;
  int mMax;
  int mMin;
  bool m_task_completed;
};

progress_dialog::progress_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateProgressDialog) {}

progress_dialog::~progress_dialog() {
  qDebug() << Q_FUNC_INFO;
  delete o_desktop_dialog;
}

void progress_dialog::create_window(const QRectF &window_geometry,
                                           const QString &window_title,
                                           const QPointF &window_pos) {
  qDebug() << Q_FUNC_INFO << window_geometry;
  qDebug() << Q_FUNC_INFO << window_pos;

  o_desktop_dialog->mFrame = new cherry_kit::window();
  set_geometry(window_geometry);

  o_desktop_dialog->mFrame->set_widget_name("Progress Dialog");
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderBackground, true);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType, true);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow, true);

  o_desktop_dialog->m_task_completed = 0;

  connect(o_desktop_dialog->mFrame, SIGNAL(closed(cherry_kit::widget *)), this,
          SLOT(onWidgetClosed(cherry_kit::widget *)));

  o_desktop_dialog->mMax = 100.0;
  o_desktop_dialog->mMin = 0.0;

  if (has_attribute("max")) {
    o_desktop_dialog->mMax = attributes()["max"].toFloat();
  }

  if (has_attribute("min")) {
    o_desktop_dialog->mMin = attributes()["min"].toFloat();
  }

  o_desktop_dialog->m_progress_bar_widget =
      new cherry_kit::progress_bar(o_desktop_dialog->mFrame);
  o_desktop_dialog->m_progress_bar_widget->set_range(o_desktop_dialog->mMin,
                                                     o_desktop_dialog->mMax);
  o_desktop_dialog->m_progress_bar_widget->set_size(
      QSize(window_geometry.width() - 10, 32));
  o_desktop_dialog->m_progress_bar_widget->set_value(0.0);
  o_desktop_dialog->m_progress_bar_widget->setPos(5.0, 64.0);
  o_desktop_dialog->m_progress_bar_widget->show();

  update_content_geometry(o_desktop_dialog->mFrame);
  exec(window_pos);

  show_activity();
}

QVariantMap progress_dialog::result() const { return QVariantMap(); }

void progress_dialog::update_attribute(const QString &name,
                                              const QVariant &data) {
  if (!o_desktop_dialog->mFrame) {
    return;
  }

  if (o_desktop_dialog->m_task_completed) {
    return;
  }

  float progress = data.toFloat();

  if (o_desktop_dialog->m_progress_bar_widget) {
    o_desktop_dialog->m_progress_bar_widget->set_value(progress);
  }

  if (o_desktop_dialog->mMax == progress) {
    discard_activity();
  }
}

cherry_kit::window *progress_dialog::activity_window() const {
  return o_desktop_dialog->mFrame;
}

void progress_dialog::cleanup() {
  if (o_desktop_dialog->mFrame) {
    delete o_desktop_dialog->mFrame;
  }
  o_desktop_dialog->mFrame = 0;
}

void progress_dialog::onWidgetClosed(cherry_kit::widget *widget) {
  discard_activity();
}

void progress_dialog::onHideAnimationFinished() {
  o_desktop_dialog->m_task_completed = 1;
}
