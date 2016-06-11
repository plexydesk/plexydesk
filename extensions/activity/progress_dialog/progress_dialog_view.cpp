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
      priv(new PrivateProgressDialog) {}

progress_dialog_view::~progress_dialog_view() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void progress_dialog_view::create_window() {
  QRectF window_geometry(0, 0, 240, 128);
  priv->m_window = new cherry_kit::window();

  priv->m_window->set_window_title("Busy");
  priv->m_task_completed = 0;

  priv->m_max_value = 100.0;
  priv->m_min_value = 0.0;

  priv->m_progress_bar_widget =
      new cherry_kit::progress_bar(priv->m_window);
  priv->m_progress_bar_widget->set_range(priv->m_min_value,
                                                     priv->m_max_value);
  priv->m_progress_bar_widget->set_size(
      QSize(window_geometry.width() - 10, 32));
  priv->m_progress_bar_widget->set_value(0.0);
  priv->m_progress_bar_widget->setPos(5.0, 64.0);

  priv->m_window->set_window_content(
              priv->m_progress_bar_widget);
}

cherry_kit::window *progress_dialog_view::dialog_window() const {
  return priv->m_window;
}

bool progress_dialog_view::purge() {
  if (priv->m_window) {
    delete priv->m_window;
  }
  priv->m_window = 0;

  return true;
}
