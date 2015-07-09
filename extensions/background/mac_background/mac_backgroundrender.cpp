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

#include "mac_backgroundrender.h"

MacBackgroundRender::MacBackgroundRender(const QRectF &rect,
                                         QGraphicsObject *parent,
                                         const QImage &background_image)
    : PlexyDesk::AbstractDesktopWidget(rect, parent) {
  setFlag(QGraphicsItem::ItemIsMovable, false);
  mBackgroundImage = background_image;
}

void MacBackgroundRender::setBackgroundImage(const QString &path) {
  if (path.isEmpty() || path.isNull()) {
    return;
  }

  mBackgroundImage.load(path);
  update();
}

void MacBackgroundRender::setStyle(StylePtr style) {}

void MacBackgroundRender::paintRotatedView(QPainter * /*painter*/,
                                           const QRectF & /*rect*/) {}

void MacBackgroundRender::paintFrontView(QPainter *painter,
                                         const QRectF & /*rect*/) {
  // painter->drawImage(contentRect(), mBackgroundImage);
  painter->fillRect(contentRect(), Qt::transparent);
}

void MacBackgroundRender::paintDockView(QPainter * /*painter*/,
                                        const QRectF & /*rect*/) {}

void MacBackgroundRender::paintEditMode(QPainter * /*painter*/,
                                        const QRectF & /*rect*/) {}
