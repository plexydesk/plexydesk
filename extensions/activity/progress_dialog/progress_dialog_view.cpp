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
#include "progress_dialog_view.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_progress_bar.h>

class progress_dialog_view::PrivateProgressDialog {
public:
  PrivateProgressDialog() {}
  ~PrivateProgressDialog() {}

  cherry_kit::window *m_window;
  cherry_kit::progress_bar *m_progress_bar_widget;
  int m_max_value;
  int m_min_value;
  bool m_task_completed;
};

progress_dialog_view::progress_dialog_view(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateProgressDialog) {}

progress_dialog_view::~progress_dialog_view() {
  qDebug() << Q_FUNC_INFO;
  delete o_desktop_dialog;
}

void progress_dialog_view::create_window(const QRectF &window_geometry,
                                           const QString &window_title,
                                           const QPointF &window_pos) {
  qDebug() << Q_FUNC_INFO << window_geometry;
  qDebug() << Q_FUNC_INFO << window_pos;

  o_desktop_dialog->m_window = new cherry_kit::window();
  set_geometry(window_geometry);

  o_desktop_dialog->m_window->set_window_title(window_title);
  o_desktop_dialog->m_task_completed = 0;

  //connect(o_desktop_dialog->m_window, SIGNAL(closed(cherry_kit::widget *)), this,
   //       SLOT(onWidgetClosed(cherry_kit::widget *)));

  o_desktop_dialog->m_max_value = 100.0;
  o_desktop_dialog->m_min_value = 0.0;

  if (has_attribute("max")) {
    o_desktop_dialog->m_max_value = attributes()["max"].toFloat();
  }

  if (has_attribute("min")) {
    o_desktop_dialog->m_min_value = attributes()["min"].toFloat();
  }

  o_desktop_dialog->m_progress_bar_widget =
      new cherry_kit::progress_bar(o_desktop_dialog->m_window);
  o_desktop_dialog->m_progress_bar_widget->set_range(o_desktop_dialog->m_min_value,
                                                     o_desktop_dialog->m_max_value);
  o_desktop_dialog->m_progress_bar_widget->set_size(
      QSize(window_geometry.width() - 10, 32));
  o_desktop_dialog->m_progress_bar_widget->set_value(0.0);
  o_desktop_dialog->m_progress_bar_widget->setPos(5.0, 64.0);

  o_desktop_dialog->m_window->set_window_content(
              o_desktop_dialog->m_progress_bar_widget);

  update_content_geometry(o_desktop_dialog->m_window);
  exec(window_pos);

  show_activity();
}

QVariantMap progress_dialog_view::result() const { return QVariantMap(); }

void progress_dialog_view::update_attribute(const QString &name,
                                              const QVariant &data) {
  if (!o_desktop_dialog->m_window) {
    return;
  }

  if (o_desktop_dialog->m_task_completed) {
    return;
  }

  float progress = data.toFloat();

  if (o_desktop_dialog->m_progress_bar_widget) {
    o_desktop_dialog->m_progress_bar_widget->set_value(progress);
  }

  if (o_desktop_dialog->m_max_value == progress) {
    discard_activity();
  }
}

cherry_kit::window *progress_dialog_view::dialog_window() const {
  return o_desktop_dialog->m_window;
}

void progress_dialog_view::cleanup() {
  if (o_desktop_dialog->m_window) {
    delete o_desktop_dialog->m_window;
  }
  o_desktop_dialog->m_window = 0;
}
