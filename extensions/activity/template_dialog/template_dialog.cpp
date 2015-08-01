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
#include "template_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>

class template_dialog::PrivateTemplate {
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}

  cherry_kit::window *m_main_window;
};

template_dialog::template_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      priv(new PrivateTemplate) {}

template_dialog::~template_dialog() { delete priv; }

void template_dialog::create_window(const QRectF &window_geometry,
                                     const QString &window_title,
                                     const QPointF &window_pos) {
  priv->m_main_window = new cherry_kit::window();

  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  set_geometry(window_geometry);
  update_content_geometry(priv->m_main_window);

  exec(window_pos);
}

QVariantMap template_dialog::result() const { return QVariantMap(); }

void template_dialog::update_attribute(const QString &name,
                                        const QVariant &data) {}

cherry_kit::window *template_dialog::dialog_window() const {
  return priv->m_main_window;
}

void template_dialog::purge() {
  if (priv->m_main_window) {
    delete priv->m_main_window;
  }
  priv->m_main_window = 0;
}
