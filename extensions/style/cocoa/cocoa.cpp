/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include "cocoa.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>
#include <stylefeatures.h>

class CocoaStyle::PrivateCocoa {
public:
  PrivateCocoa() {}
  ~PrivateCocoa() {}

  QHash<QString, int> m_type_map;
  QVariantMap m_attribute_map;
};

CocoaStyle::CocoaStyle() : d(new PrivateCocoa) {
  d->m_type_map["button"] = 1;
  d->m_type_map["vertical_list_item"] = 2;
  d->m_type_map["window_button"] = 3;
  d->m_type_map["seperator"] = 4;
  d->m_type_map["text_edit"] = 5;
  d->m_type_map["line_edit"] = 6;
  d->m_type_map["image_view"] = 7;
  d->m_type_map["image_frame"] = 8;
  d->m_type_map["label"] = 9;
  d->m_type_map["clock"] = 10;
  d->m_type_map["cell"] = 11;
  d->m_type_map["dial"] = 12;
  d->m_type_map["knob"] = 13;
  d->m_type_map["tool_button"] = 14;
  d->m_type_map["tool_bar"] = 15;
  d->m_type_map["dock_background"] = 16;
  d->m_type_map["desktop_background"] = 17;
  d->m_type_map["desktop_side_panel"] = 18;
  d->m_type_map["linear_progress_bar"] = 19;
  d->m_type_map["circular_progress_bar"] = 20;
  d->m_type_map["window_frame"] = 21;
  d->m_type_map["window_title"] = 22;
  d->m_type_map["window_resize_handle"] = 23;

  // style attributes. this could be read from a xml file or a stylesheet.
  QVariantMap _frame_attributes;
  QVariantMap _widget_attributes;
  QVariantMap _size_attributes;
  QVariantMap _button_attributes;
  QVariantMap _vlist_item_attributes;

  _frame_attributes["window_title_height"] = 64.0;
  _frame_attributes["window_minimized_height"] = 128.0;
  _frame_attributes["window_minimized_width"] = 96.0;
  _frame_attributes["window_blur_radius"] = 24.0;
  _frame_attributes["window_close_button_width"] = 16.0;
  _frame_attributes["window_close_button_height"] = 16.0;
  _frame_attributes["window_shadow_color"] = QString("#323232");
  _frame_attributes["padding"] = 5.0;

  _widget_attributes["line_edit_width"] = 256.0;
  _widget_attributes["line_edit_height"] = 32.0;

  _size_attributes["small"] = 0.5;
  _size_attributes["original"] = 1.0;
  _size_attributes["large"] = 2.0;
  _size_attributes["huge"] = 4.0;

  _button_attributes["width"] = 128.0;
  _button_attributes["height"] = 32.0;

  d->m_attribute_map["frame"] = _frame_attributes;
  d->m_attribute_map["widget"] = _widget_attributes;
  d->m_attribute_map["size"] = _size_attributes;
}

CocoaStyle::~CocoaStyle() { delete d; }

QVariantMap CocoaStyle::attributeMap(const QString &type) const {
  return d->m_attribute_map[type].toMap();
}

void CocoaStyle::draw(const QString &type, const StyleFeatures &options,
                      QPainter *painter) {
  switch (d->m_type_map[type]) {
    case 1:
      drawPushButton(options, painter);
      break;
    case 2:
      drawVListItem(options, painter);
      break;
    case 3:
      drawWindowButton(options, painter);
      break;
    case 6:
      drawLineEdit(options, painter);
      break;
    case 19:
      drawProgressBar(options, painter);
      break;
    case 21:
      drawFrame(options, painter);
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Unknown Element:" << type;
  }
}

void CocoaStyle::drawPushButton(const StyleFeatures &features,
                                QPainter *painter) {
  QRectF rect = features.geometry;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 0.0, 0.0);

  if (features.render_state == StyleFeatures::kRenderPressed) {
    painter->fillPath(backgroundPath, QColor("#0082EE"));
    QPen pen(QColor("#F0F0F0"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  } else if (features.render_state == StyleFeatures::kRenderRaised) {
    painter->fillPath(backgroundPath, QColor("#0092CC"));
    QPen pen(QColor("#ffffff"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  } else {
    painter->fillPath(backgroundPath, QColor("#0092CC"));
    QPen pen(QColor("#ffffff"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  }

  painter->drawText(features.geometry, Qt::AlignCenter, features.text_data);
}

void CocoaStyle::drawWindowButton(const StyleFeatures &features,
                                  QPainter *painter) {
  QRectF rect = features.geometry.adjusted(0, 0, 0, 0);

  painter->save();
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);

  QPainterPath background;
  background.addRoundedRect(rect, 3.0, 3.0);

  if (features.render_state == StyleFeatures::kRenderElement) {
    painter->fillPath(background, QColor("#CC3333"));
  } else {
    painter->fillPath(background, QColor("#000000"));
  }

  painter->save();
  QPen white_pen(QColor("#ffffff"), 2, Qt::SolidLine, Qt::RoundCap,
                 Qt::RoundJoin);
  painter->setPen(white_pen);
  QRectF cross_rect(6.0, 6.0, rect.width() - 12, rect.height() - 12);

  // painter->drawRect(cross_rect);
  painter->drawLine(cross_rect.topLeft(), cross_rect.bottomRight());
  painter->drawLine(cross_rect.topRight(), cross_rect.bottomLeft());

  painter->restore();
  painter->restore();
}

void CocoaStyle::drawFrame(const StyleFeatures &features, QPainter *painter) {
  QRectF rect = features.geometry;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 8.0, 8.0);

  painter->fillPath(backgroundPath, QColor("#F0F0F0"));

  // draw seperator
  if (!features.text_data.isNull() || !features.text_data.isEmpty()) {
    QRectF _window_title_rect(0.0, 0.0, features.geometry.width(), 54.0);

    QLinearGradient _seperator_line_grad(_window_title_rect.bottomLeft(),
                                         _window_title_rect.bottomRight());
    _seperator_line_grad.setColorAt(0.0, QColor("#F0F0F0"));
    _seperator_line_grad.setColorAt(0.5, QColor("#d6d6d6"));
    _seperator_line_grad.setColorAt(1.0, QColor("#F0F0F0"));

    QPen linePen = QPen(_seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);

    // draw frame text;
    painter->save();
    QFont _font = painter->font();
    _font.setBold(true);
    _font.setPointSize(18);
    painter->setFont(_font);
    QTextOption option;
    option.setAlignment(Qt::AlignCenter);

    QPen _title_font_pen =
        QPen(QColor("#969696"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(_title_font_pen);

    painter->drawText(_window_title_rect, features.text_data, option);

    painter->restore();

    painter->save();
    painter->setPen(linePen);
    painter->drawLine(_window_title_rect.bottomLeft(),
                      _window_title_rect.bottomRight());
    painter->restore();
  } else {
    // painter->fillRect(QRectF(0.0, 0.0, rect.width(), 10), QColor("#F0F0F0"));
  }
  painter->restore();
}

void CocoaStyle::drawPushButtonText(const StyleFeatures &features,
                                    const QString &text, QPainter *painter) {
  /* Painter settings */
  QPen pen;

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  painter->setPen(pen);
  painter->drawText(features.geometry, Qt::AlignCenter, text);
  painter->restore();
}

void CocoaStyle::drawLineEdit(const StyleFeatures &features,
                              QPainter *painter) {
  QRectF rect = features.geometry.adjusted(1, 1, 0, 0);

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 6, 0);

  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, rect.height()));

  linearGrad.setColorAt(0, QColor(189, 191, 196));
  linearGrad.setColorAt(1, QColor(255, 255, 255));

  painter->fillPath(backgroundPath, QColor("#F0F0FF"));

  QPen pen;
  if (features.render_state == StyleFeatures::kRenderRaised) {
    pen =
        QPen(QColor("#D1D1D1"), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  } else {
    pen =
        QPen(QColor("#D0D0D0"), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  }
  painter->setPen(pen);
  painter->drawPath(backgroundPath);

  painter->save();
  pen.setColor(QColor("#323232"));
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(10.0, 0.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, features.text_data);
  painter->restore();
}

void CocoaStyle::drawLineEditText(const StyleFeatures &features,
                                  const QString &text, QPainter *painter) {
  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  QPen pen;

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  QFont font = QFont("", 18);
  QFontMetrics fontMetrics(font);
  // int width = fontMetrics.width(text.left(features.cursorLocation));

  painter->setFont(font);
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(11.0, 1.0, 1.0, 1.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen = QPen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(10.0, 0.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen =
      QPen(QColor(98, 101, 108), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  // painter->drawLine(QPoint(width + 11, 4), QPoint(width +
  // 11,features.geometry.height() - 4));
}

void CocoaStyle::drawLabelEditText(const StyleFeatures &features,
                                   const QString &text, QPainter *painter) {
  /*
  QPen pen;

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  painter->setFont(features.font);

  pen = QPen(QColor (88, 88, 88), 1, Qt::SolidLine, Qt::RoundCap,
  Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(1, 1, 1, 1), features.fontFlags,
  text);

  painter->setOpacity(0.8);

  pen = QPen(features.fontColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry, features.fontFlags, text);
  */
}

void CocoaStyle::drawSeperatorLine(const StyleFeatures &features,
                                   QPainter *painter) {
  painter->save();
  QPen pen = QPen(QColor(217, 217, 217), 1, Qt::SolidLine, Qt::RoundCap,
                  Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.geometry.x(), features.geometry.y()),
      QPoint(features.geometry.width(), features.geometry.height()));
  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.geometry.x() + 1, features.geometry.y() + 1),
      QPoint(features.geometry.width() + 1, features.geometry.height() + 1));
  painter->restore();
}

void CocoaStyle::drawProgressBar(const StyleFeatures &features,
                                 QPainter *painter) {
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  switch (features.render_state) {
    case StyleFeatures::kRenderBackground: {
      QRectF rect = features.geometry;
      QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, 20));
      linearGrad.setColorAt(1, QColor("#F7F7F7"));
      linearGrad.setColorAt(0.5, QColor("#C2C2C2"));
      linearGrad.setColorAt(0, QColor("#F7F7F7"));
      QPen backgroundPen(linearGrad, 4, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin);

      QPointF backgroundLineStart(rect.topLeft().x(),
                                  rect.bottomLeft().y() / 2);
      QPointF backgroundLineEnd(rect.topRight().x(),
                                rect.bottomRight().y() / 2);
      painter->setPen(backgroundPen);
      painter->drawLine(backgroundLineStart, backgroundLineEnd);
    } break;
    case StyleFeatures::kRenderForground: {
      QRectF rect = features.geometry;
      QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, 20));
      linearGrad.setColorAt(1, QColor("#91B670"));
      linearGrad.setColorAt(0, QColor("#B3DF89"));
      QPen backgroundPen(linearGrad, 4, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin);

      QPointF backgroundLineStart(rect.topLeft().x(),
                                  rect.bottomLeft().y() / 2);
      QPointF backgroundLineEnd(rect.topRight().x(),
                                rect.bottomRight().y() / 2);
      painter->setPen(backgroundPen);
      painter->drawLine(backgroundLineStart, backgroundLineEnd);
    } break;
    default:
      qDebug() << Q_FUNC_INFO << "Unknown progress bar state";
  }

  painter->restore();
}

void CocoaStyle::drawVListItem(const StyleFeatures &features,
                               QPainter *painter) {
  QRectF rect = features.geometry;

  QPen backgroundPen(QColor("#d6d6d6"), 1, Qt::SolidLine, Qt::RoundCap,
                     Qt::RoundJoin);

  painter->save();
  if (features.render_state == StyleFeatures::kRenderElement) {
    painter->setPen(backgroundPen);
    painter->fillRect(rect, QColor("#f7f7f7"));

    QLinearGradient _seperator_line_grad(rect.bottomLeft(), rect.bottomRight());
    _seperator_line_grad.setColorAt(0.0, QColor("#F0F0F0"));
    _seperator_line_grad.setColorAt(0.5, QColor("#d6d6d6"));
    _seperator_line_grad.setColorAt(1.0, QColor("#F0F0F0"));

    QPen linePen = QPen(_seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  }
  painter->restore();
}
