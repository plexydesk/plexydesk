/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : Lahiru Lakmal <llahiru@gmail.com>
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

#ifndef PLEXY_DAY_WIDGET_H
#define PLEXY_DAY_WIDGET_H

#include "videoentity.h"

#include <extensionmanager.h>

#include <desktopwidget.h>
#include <scrollwidget.h>
#include <QGraphicsTextItem>
#include <datasource.h>
#include <QGraphicsBlurEffect>

#include <button.h>
#include <lineedit.h>
#include <tableview.h>

#include "cellsprovider.h"

class QTimer;

namespace PlexyDesk {

class PlexyDayWidget : public PlexyDesk::Widget {
  Q_OBJECT

public:
  PlexyDayWidget(const QRectF &rect);

  virtual ~PlexyDayWidget();

  void drawBox(QPainter *painter, QLinearGradient titleCornerGrad,
               const QRectF &rect, QRectF titleCornerRect);

  QLinearGradient genGradiant(QRectF titleCornerRect);

  QLinearGradient genGrad(QPointF start, QPointF end, QList<QColor> &colors);

public
Q_SLOTS:
  void onDataReady();

  void buttonClicked();

Q_SIGNALS:
  void dataChanged();

protected:
  virtual void paintFrontView(QPainter *painter, const QRectF &rect);

private:
  PlexyDesk::TableView *mTableView;
  CellsProvider *mCellProvider;
  class PlexyDayWidgetPrivate;
  PlexyDayWidgetPrivate *const d;
};

} // namespace PlexyDesk
#endif
