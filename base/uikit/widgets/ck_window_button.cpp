/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
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

#include "ck_window_button.h"
#include "ck_style.h"
#include <ck_resource_manager.h>
#include <ck_extension_manager.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

namespace cherry_kit {

class window_button::PrivateWindowButton {
public:
  PrivateWindowButton() {}
  ~PrivateWindowButton() {}

  WindowButtonType mType;
};

window_button::window_button(widget *parent)
    : button(parent), priv(new PrivateWindowButton) {
  set_geometry(boundingRect());
  priv->mType = CLOSE;
}

window_button::~window_button() { delete priv; }

void window_button::set_button_type(window_button::WindowButtonType a_type) {
  priv->mType = a_type;
}

QRectF window_button::boundingRect() const {
  if (!resource_manager::style()) {
    return QRectF();
  }

  return QRectF(0.0, 0.0, resource_manager::style()
                              ->attribute("frame", "window_close_button_width")
                              .toFloat() * resource_manager::style()->scale_factor(),
                resource_manager::style()
                    ->attribute("frame", "window_close_button_height")
                    .toFloat() * resource_manager::style()->scale_factor());
}

void window_button::paint_normal_button(QPainter *a_painter_ptr,
                                       const QRectF &a_rect) {
  style_data feature;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderElement;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("window_button", feature,
                                                a_painter_ptr);
  }
}

void window_button::paint_sunken_button(QPainter *a_painter_ptr,
                                       const QRectF &a_rect) {
  style_data feature;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderRaised;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("window_button", feature,
                                                a_painter_ptr);
  }
}

void window_button::paint_hover_button(QPainter *a_painter,
                                      const QRectF &a_rect) {
  style_data feature;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderRaised;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("window_button", feature,
                                                a_painter);
  }
}
}
