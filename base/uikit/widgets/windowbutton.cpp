/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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

#include "windowbutton.h"
#include "style.h"
#include <themepackloader.h>
#include <extensionmanager.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

namespace UIKit
{

class WindowButton::PrivateWindowButton
{
public:
  PrivateWindowButton() {}
  ~PrivateWindowButton() {}

  WindowButtonType mType;
};

WindowButton::WindowButton(QGraphicsObject *parent)
  : Button(parent), d(new PrivateWindowButton)
{
  setGeometry(boundingRect());
  d->mType = CLOSE;
}

WindowButton::~WindowButton() { delete d; }

void WindowButton::set_button_type(WindowButton::WindowButtonType a_type)
{
  d->mType = a_type;
}

QRectF WindowButton::boundingRect() const
{
  if (!Theme::style()) {
    return QRectF();
  }

  return QRectF(
           0.0, 0.0,
           Theme::style()->attribute("frame", "window_close_button_width").toFloat(),
           Theme::style()
           ->attribute("frame", "window_close_button_height")
           .toFloat());
}

void WindowButton::paint_normal_button(QPainter *a_painter_ptr, const QRectF &a_rect)
{
  StyleFeatures feature;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderElement;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("window_button", feature, a_painter_ptr);
  }
}

void WindowButton::paint_sunken_button(QPainter *a_painter_ptr, const QRectF &a_rect)
{
  StyleFeatures feature;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderRaised;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("window_button", feature, a_painter_ptr);
  }
}
}
