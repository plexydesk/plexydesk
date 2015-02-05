/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexyplanet.org>
*  Authored By  : Siraj Razick <siraj@plexyplanet.org>
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

#ifndef FILEINFORVIEW_H
#define FILEINFORVIEW_H

#include <plexy.h>
#include <widget.h>

#include <QIcon>
#include <QFileInfo>

class FileInforView : public UI::Window
{
  Q_OBJECT
public:
  FileInforView(QGraphicsObject *parent = 0);

  void setFileInfo(const QFileInfo &info);

  void setIcon(const QIcon &icon);

  void pop();

  void push();

  void setSliderPos(const QPointF &start, const QPointF &end);

public Q_SLOTS:
  void onClicked();
  void onCloseButtonClicked();

protected:
  virtual void paintView(QPainter *painter, const QRectF &rect);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
  class PrivateFileInforView;
  PrivateFileInforView *const d;
};

#endif // FILEINFORVIEW_H
