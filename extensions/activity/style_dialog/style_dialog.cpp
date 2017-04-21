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
#include "style_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>

#include <ck_widget.h>
#include <ck_button.h>
#include <ck_text_editor.h>
#include <ck_line_edit.h>
#include <ck_label.h>
#include <ck_fixed_layout.h>
#include <ck_image_view.h>
#include <ck_progress_bar.h>
#include <ck_resource_manager.h>
#include <ck_item_view.h>

class style_dialog::Privatestyle {
public:
  Privatestyle() {}
  ~Privatestyle() {}

  cherry_kit::window *m_main_window;
  cherry_kit::fixed_layout *m_layout;
  cherry_kit::button *m_done_btn;
  cherry_kit::item_view *m_style_list;
};

style_dialog::style_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      priv(new Privatestyle) {}

style_dialog::~style_dialog() { delete priv; }

void style_dialog::create_window() {
  QRectF window_geometry(0, 0, 320, 500);
  priv->m_main_window = new cherry_kit::window();

  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);
  priv->m_main_window->set_window_title("Pick your Style");

  /* layout */
  priv->m_layout = new cherry_kit::fixed_layout(priv->m_main_window);

  priv->m_layout->set_content_margin(0, 0, 2, 0);
  priv->m_layout->set_geometry(0, 0, window_geometry.width(),
                               window_geometry.height());
  priv->m_layout->set_verticle_spacing(16);
  priv->m_layout->add_rows(2);

  priv->m_layout->set_row_height(0, "90%");
  priv->m_layout->set_row_height(1, "10%");
  priv->m_layout->add_segments(0, 1);
  priv->m_layout->add_segments(1, 1);

  cherry_kit::widget_properties_t prop;
  prop["label"] = "Apply Theme";

  priv->m_done_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(1, 0, "button", prop, [=]() {}));

  priv->m_style_list = dynamic_cast<cherry_kit::item_view *>(
      priv->m_layout->add_widget(0, 0, "model_view", prop, [=]() {}));


  if (priv->m_style_list) {
    cherry_kit::model_view_item *style_item = new cherry_kit::model_view_item();
    cherry_kit::label *style_item_lbl = new cherry_kit::label();

    style_item_lbl->set_alignment(Qt::AlignLeft | Qt::AlignVCenter);
    style_item_lbl->set_text("PlexyDesk - Simple Gray Theme");
    style_item_lbl->set_size(QSize(320, 32));

    style_item->set_view(style_item_lbl);
    priv->m_style_list->set_content_size(320, 32);

    style_item_lbl->on_click([=]() {
      qDebug() <<  Q_FUNC_INFO << "style clicked" << style_item_lbl->text();
      cherry_kit::resource_manager::instance()->set_style("cocoa");
      if (viewport())
        viewport()->update_view();
    });

    priv->m_style_list->insert(style_item);
  }

  priv->m_main_window->set_window_content(priv->m_layout->viewport());
}

cherry_kit::window *style_dialog::dialog_window() const {
  return priv->m_main_window;
}

bool style_dialog::purge() {
  if (priv->m_layout) {
      delete priv->m_layout;
  }

  if (priv->m_main_window) {
    delete priv->m_main_window;
  }

  priv->m_main_window = 0;

  return true;
}
