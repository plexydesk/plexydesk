/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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
#include "ubuntu.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>

class ubuntuStyle::Privateubuntu {
public:
  Privateubuntu() {}
  ~Privateubuntu() {}
};

ubuntuStyle::ubuntuStyle(QObject *parent)
    : PlexyDesk::Style(parent), d(new Privateubuntu) {}

ubuntuStyle::~ubuntuStyle() { delete d; }

void ubuntuStyle::paintControlElement(Style::ControlElement element,
                                      const StyleFeatures &feature,
                                      QPainter *painter) {
  switch (element) {
    case CE_PushButton:
      drawPushButton(feature, painter);
      break;
    case CE_Frame:
      drawFrame(feature, painter);
      break;
    case CE_LineEdit:
      drawLineEdit(feature, painter);
      break;
    case CE_Seperator:
      drawSeperatorLine(feature, painter);
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Unknown Control Element";
  }
}

void ubuntuStyle::paintControlElementText(Style::ControlElement element,
                                          const StyleFeatures &feature,
                                          const QString &text,
                                          QPainter *painter) {
  switch (element) {
    case CE_PushButton:
      drawPushButtonText(feature, text, painter);
      break;
    case CE_LineEdit:
      drawLineEditText(feature, text, painter);
      break;
    case CE_Label:
      drawLabelEditText(feature, text, painter);
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Unknown Control Element";
  }
}

StyleFeatures ubuntuStyle::controlElementFeatures(
    Style::ControlElement element) {
  StyleFeatures style;

  style.windowTitleHeight = 16;
  style.windowTitleWidth = 16;
  style.windowButtonHeight = 12;
  style.windowButtonWidth = 12;
  style.windowTitleHeight = 48;
  style.windowTitleWidth = 48;
  style.buttonWidth = 64;
  style.buttonHeight = 32;
  style.inputHeight = 64;
  style.inputWidth = 200;
  style.tableCellHeight = 128;
  style.tableCellWidth = 200;
  style.tableCellPadding = 0.0;
  style.tableCellMargin = 0.0;
  style.windowDockHeight = 96;
  style.windowDockWidth = 96;

#ifdef Q_WS_QPA
  style.windowTitleHeight = 58;
  style.windowTitleWidth = 58;
  style.windowButtonHeight = 48;
  style.windowButtonWidth = 48;
  style.buttonWidth = 200;
  style.buttonHeight = 100;
  style.inputHeight = 100;
  style.inputWidth = 200;
  style.tableCellHeight = 128;
  style.tableCellWidth = 200;
  style.tableCellPadding = 0.0;
  style.tableCellMargin = 0.0;
  style.windowDockHeight = 180;
  style.windowDockWidth = 180;
#endif

  return style;
}

void ubuntuStyle::drawPushButton(const StyleFeatures &features,
                                 QPainter *painter) {
  QRectF rect = features.exposeRect;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, features.buttonBorderRoundness,
                                features.buttonBorderRoundness);

  if (features.state == StyleFeatures::SF_Raised) {
    painter->fillPath(backgroundPath, features.buttonGradiantRaised);
    QPen pen(features.buttonBorderGradient, 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  } else {
    painter->fillPath(backgroundPath, features.buttonGradiantMouseOver);
    QPen pen(features.buttonBorderGradient, 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  }
}

void ubuntuStyle::drawFrame(const StyleFeatures &features, QPainter *painter) {
  QRectF rect = features.exposeRect;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 6, 6);

  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, rect.height()));

  linearGrad.setColorAt(1, QColor(246, 246, 246));
  linearGrad.setColorAt(0, QColor(236, 236, 236));
  // QColor(189, 191, 196)
  painter->fillPath(backgroundPath, linearGrad);
  // QPen pen(QColor(98, 101, 108), 0, Qt::SolidLine, Qt::RoundCap,
  // Qt::RoundJoin);
  // painter->setPen(pen);
  // painter->drawPath(backgroundPath);

  if (features.windowTitleHeight > 0 && features.windowTitleWidth > 0) {
    // QLinearGra
    QRectF frameTitleRect(-1.0, -1.0, features.windowTitleWidth + 2,
                          features.windowTitleHeight + 2);
    painter->fillRect(frameTitleRect, QColor(250, 226, 10));

    painter->drawText(frameTitleRect, features.fontFlags,
                      features.windowTitlLabel);
  }
}

void ubuntuStyle::drawPushButtonText(const StyleFeatures &features,
                                     const QString &text, QPainter *painter) {
  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  QPen pen;

  pen = QPen(features.fontColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.exposeRect, Qt::AlignCenter, text);
}

void ubuntuStyle::drawLineEdit(const StyleFeatures &features,
                               QPainter *painter) {
  QRectF rect = features.exposeRect;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 2, 2);

  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, rect.height()));

  linearGrad.setColorAt(0, QColor(189, 191, 196));
  linearGrad.setColorAt(1, QColor(255, 255, 255));

  painter->fillPath(backgroundPath, linearGrad);
  QPen pen;
  if (features.state == StyleFeatures::SF_MouseOver) {
    pen = QPen(QColor(98, 101, 208), 1, Qt::SolidLine, Qt::RoundCap,
               Qt::RoundJoin);
  } else {
    pen = QPen(QColor(98, 101, 108), 1, Qt::SolidLine, Qt::RoundCap,
               Qt::RoundJoin);
  }
  painter->setPen(pen);
  painter->drawPath(backgroundPath);
}

void ubuntuStyle::drawLineEditText(const StyleFeatures &features,
                                   const QString &text, QPainter *painter) {
  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  QPen
  pen; // pen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  QFont font = QFont("", 18);
  QFontMetrics fontMetrics(font);
  int width = fontMetrics.width(text.left(features.cursorLocation));

  painter->setFont(font);
  painter->setPen(pen);
  painter->drawText(features.exposeRect.adjusted(11.0, 1.0, 1.0, 1.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen = QPen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.exposeRect.adjusted(10.0, 0.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen =
      QPen(QColor(98, 101, 108), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(QPoint(width + 11, 4),
                    QPoint(width + 11, features.exposeRect.height() - 4));
}

void ubuntuStyle::drawLabelEditText(const StyleFeatures &features,
                                    const QString &text, QPainter *painter) {
  QPen pen;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  painter->setFont(features.font);

  pen = QPen(QColor(88, 88, 88), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.exposeRect.adjusted(1, 1, 1, 1),
                    features.fontFlags, text);

  painter->setOpacity(0.8);

  pen = QPen(features.fontColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.exposeRect, features.fontFlags, text);
}

void ubuntuStyle::drawSeperatorLine(const StyleFeatures &features,
                                    QPainter *painter) {
  painter->save();
  QPen pen = QPen(QColor(217, 217, 217), 1, Qt::SolidLine, Qt::RoundCap,
                  Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.exposeRect.x(), features.exposeRect.y()),
      QPoint(features.exposeRect.width(), features.exposeRect.height()));
  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.exposeRect.x() + 1, features.exposeRect.y() + 1),
      QPoint(features.exposeRect.width() + 1,
             features.exposeRect.height() + 1));
  painter->restore();
}
