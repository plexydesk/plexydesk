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

namespace UI {

class WindowButton::PrivateWindowButton {
public:
  PrivateWindowButton() {}
  ~PrivateWindowButton() {}

  WindowButtonType mType;
};

WindowButton::WindowButton(QGraphicsObject *parent)
    : UI::Button(parent), d(new PrivateWindowButton) {
  setGeometry(boundingRect());
  d->mType = CLOSE;
}

WindowButton::~WindowButton() { delete d; }

void WindowButton::setButtonType(WindowButton::WindowButtonType type) {
  d->mType = type;
}

QRectF WindowButton::boundingRect() const {
  if (!Theme::style())
    return QRectF();

  return QRectF(
      0.0, 0.0,
      Theme::style()->attrbute("frame", "window_close_button_width").toFloat(),
      Theme::style()
          ->attrbute("frame", "window_close_button_height")
          .toFloat());
}

void WindowButton::paintNormalButton(QPainter *painter, const QRectF &rect) {
  StyleFeatures feature;
  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderElement;

  if (UI::Theme::style()) {
    UI::Theme::style()->draw("window_button", feature, painter);
  }
}

void WindowButton::paintSunkenButton(QPainter *painter, const QRectF &rect) {
  StyleFeatures feature;
  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderRaised;

  if (UI::Theme::style()) {
    UI::Theme::style()->draw("window_button", feature, painter);
  }
}
}
