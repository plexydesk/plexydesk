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

#include "plexydayck_ck_widget.h"

#include <QMatrix>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QBitmap>
#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>

#include <ck_button.h>
#include <ck_TableView.h>
#include <nativestyle.h>

#include "cellsprovider.h"

namespace PlexyDesk {

class PlexyDayWidget::PlexyDayWidgetPrivate {
public:
  PlexyDayWidgetPrivate() {}
  ~PlexyDayWidgetPrivate() {}
  PlexyDesk::Style *mStyle;
};

PlexyDayWidget::PlexyDayWidget(const QRectF &rect)
    : Widget(rect), d(new PlexyDayWidgetPrivate) {
  setContentRect(rect);

  this->setWidgetFlag(PlexyDesk::Widget::kRenderBackground, false);

#ifdef Q_WS_QPA
  setFlag(QGraphicsItem::ItemIsMovable, false);
#endif

  mTableView = new PlexyDesk::TableView(rect, this);
  mTableView->show();
  /// mTableView->setLabelName("TableView");

  CellsProvider *cellFactory = new CellsProvider(rect, this);
  mTableView->setDelegate(cellFactory);

  float headerHight = 74.0;
#ifdef Q_WS_QPA
  headerHight = (64.0 * 2) + 20;
#endif
  // Leave a margin of 74px for the ck_ck_widget.header. and a 5px left margin
  mTableView->setPos(cellFactory->leftMargin(), headerHight);
}

PlexyDayWidget::~PlexyDayWidget() { delete mTableView; }

void PlexyDayWidget::onDataReady() {}

void PlexyDayWidget::buttonClicked() {}

void PlexyDayWidget::drawBox(QPainter *painter, QLinearGradient titleCornerGrad,
                             const QRectF &rect, QRectF titleCornerRect) {
  painter->fillRect(titleCornerRect, titleCornerGrad);

  PlexyDesk::StyleFeatures feature;
  feature.exposeRect = titleCornerRect;
  feature.state = PlexyDesk::StyleFeatures::SF_FrontView;

  QPen pen(QColor(88, 88, 88), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  QPen shadowPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
                 Qt::RoundJoin);

#ifdef Q_WS_QPA
  QFont font = QFont("Georgia", 10);
#else
  QFont font = QFont("Georgia", 34);
#endif

  painter->setFont(font);

  painter->setPen(shadowPen);
  painter->drawText(titleCornerRect.adjusted(1, 1, 1, 1),
                    Qt::AlignVCenter | Qt::AlignHCenter,
                    "Your Day, Simplified!");
  painter->setOpacity(0.9);
  painter->setPen(pen);
  painter->drawText(titleCornerRect, Qt::AlignVCenter | Qt::AlignHCenter,
                    "Your Day, Simplified!");

  // QPen pen2(QColor(244, 242, 246), 1, Qt::SolidLine, Qt::RoundCap,
  // Qt::RoundJoin);
  // painter->setPen(pen2);

  // QRectF titleHeaderRect = QRect(titleCornerRect.width(), 3.0, (rect.width()
  // - titleCornerRect.width()), titleCornerRect.height());

  // painter->fillRect(titleHeaderRect, titleCornerGrad);
  // painter->drawRect(titleCornerRect);

  // QPen pen3(QColor(0, 255, 0), 1, Qt::SolidLine, Qt::RoundCap,
  // Qt::RoundJoin);
  // painter->setPen(pen3);

  // painter->drawLine(QPointF(titleCornerRect.x(), titleCornerRect.height() -
  // 1), QPointF(titleCornerRect.width(), titleCornerRect.height() -1));
}

QLinearGradient PlexyDayWidget::genGrad(QPointF start, QPointF end,
                                        QList<QColor> &colors) {
  QLinearGradient titleHeaderGrad(start, end);

  for (int i = 0; i < colors.count(); i++) {
    titleHeaderGrad.setColorAt(i, colors.at(i));
  }

  return titleHeaderGrad;
}

void PlexyDayWidget::paintFrontView(QPainter *painter, const QRectF &rect) {
  if (!painter->isActive()) {
    return;
  }
  if (isObscured()) {
    return;
  }

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  /* Draw the background. */
  float headerHight = 64.0;
#ifdef Q_WS_QPA
  headerHight = 64.0 * 2;
#endif

  QPainterPath backgroundPath;
  backgroundPath.addRect(QRectF(0.0, headerHight, rect.width(), rect.height()));

  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, rect.height()));

  linearGrad.setColorAt(1, QColor(189, 191, 196));
  linearGrad.setColorAt(0, QColor(231, 235, 238));

  painter->fillPath(backgroundPath, linearGrad);

  QPen pen(QColor(98, 101, 108), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawPath(backgroundPath);

  /* Draw the top header date box */

  QRectF titleCornerRect = QRect(0.0, 0.0, headerHight, headerHight);

  QLinearGradient titleCornerGrad(QPointF(0, 0),
                                  QPointF(0.0, titleCornerRect.height()));

  titleCornerGrad.setColorAt(0, QColor(81, 168, 201));
  titleCornerGrad.setColorAt(1, QColor(21, 108, 169));

  painter->fillRect(titleCornerRect, titleCornerGrad);

  QPointF start(0.0, 0.0);
  QPointF end(0.0, titleCornerRect.height());

  QList<QColor> colors;
  QColor startColor(235, 238, 241);
  QColor endColor(220, 225, 228);

  colors << startColor << endColor;

  QLinearGradient titleHeaderGrad = genGrad(start, end, colors);

  QRectF titleHeaderRect(titleCornerRect.width(), 0.0,
                         rect.width() - titleCornerRect.width(),
                         titleCornerRect.height());

  drawBox(painter, titleHeaderGrad, rect, titleHeaderRect);
}

} // namespace PlexyDesk
