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
#include "pixabay_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_fixed_layout.h>

class pixabay_dialog::Privatepixabay {
public:
  Privatepixabay() {}
  ~Privatepixabay() {}

  cherry_kit::window *m_main_window;
  cherry_kit::fixed_layout *m_layout;
};

pixabay_dialog::pixabay_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      priv(new Privatepixabay) {}

pixabay_dialog::~pixabay_dialog() { delete priv; }

void pixabay_dialog::create_window(const QRectF &window_geometry,
                                     const QString &window_title,
                                     const QPointF &window_pos) {
  priv->m_main_window = new cherry_kit::window();

  set_geometry(window_geometry);
  update_content_geometry(priv->m_main_window);

  /* Layout */
  priv->m_layout = new cherry_kit::fixed_layout(priv->m_main_window);
  priv->m_layout->set_content_margin(4, 4, 4, 4);
  priv->m_layout->set_geometry(0, 0, priv->m_main_window->geometry().width(),
                               priv->m_main_window->geometry().height());

  priv->m_main_window->set_window_content(priv->m_layout->viewport());
  exec(window_pos);
}

void pixabay_dialog::update_attribute(const QString &name,
                                        const QVariant &data) {}

cherry_kit::window *pixabay_dialog::dialog_window() const {
  return priv->m_main_window;
}

void pixabay_dialog::purge() {
  if (priv->m_main_window) {
    delete priv->m_main_window;
  }
  priv->m_main_window = 0;
}
