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

#ifndef MAC_BACKGROUNDRENDER_H
#define MAC_BACKGROUNDRENDER_H

#include <QImage>

#include <abstractdesktopck_widget.h>

class MacBackgroundRender : public PlexyDesk::AbstractDesktopWidget {
  Q_OBJECT
public:
  explicit MacBackgroundRender(const QRectF &rect, QGraphicsObject *parent = 0,
                               const QImage &background_image = QImage());

  void setBackgroundImage(const QString &path);

  void setStyle(StylePtr style);

  virtual void paintRotatedView(QPainter *painter, const QRectF &rect);
  virtual void paintFrontView(QPainter *painter, const QRectF &rect);
  virtual void paintDockView(QPainter *painter, const QRectF &rect);
  virtual void paintEditMode(QPainter *painter, const QRectF &rect);

private:
  QImage mBackgroundImage;
};

#endif // CLASSICBACKGROUNDRENDER_H
