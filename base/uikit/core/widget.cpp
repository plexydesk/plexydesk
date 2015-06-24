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

#include <QCryptographicHash>
#include <QDir>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPaintEvent>
#include <QPainter>

#include "resource_manager.h"
#include <view_controller.h>
#include "widget.h"

/**
 * \class UI::AbstractDesktopWidget
 *
 * \ingroup PlexyDesk
 *
 * \brief Base class for Visual Desktop Extensions
 * UI::AbstractDesktopWidget defines all the common features of a visual
 *desktop extension.
 * When writing visual extensions, the user must inherit from this class. So
 *that it behaves the
 * same way.
 */

/**
 * \fn UI::AbstractDesktopWidget::boundingRect()
 * \breif Returns the Bounding rectangle of the Widget Content
 *
 * \paragraph This method returns the bounding Rectangle of the Widget Contents
 * it returns the user specificed content rectangle
 *(UI::AbstractDesktopWidget::setContentRect())
 * And does not automatically calculate or adjust with the content changes, The
 *size of the content must be
 * manually set if the content changes and this widget needs to adjust to that
 *size.
 *
 * @returns The bounding Rectangle for this widget
 * \sa UI::AbstractDesktopWidget::setContentRect()
 * \sa UI::AbstractDesktopWidget::setDockRect()
 */

/**
* \fn UI::AbstractDesktopWidget::setContentRect()
*\brief Set the content Rectangle of the Widget
*
* \paragraph Sets the content rectangle of the widget
* in situations where the widget can't
* find out by it self. For instance widets which changes
* it's content during runtime or chiild QML widgets.
* Use it only if you are changing the widget size.
*
* \param  rect The Content bounding box area
*/

/**
* \fn UI::AbstractDesktopWidget::setRect()
*\brief Set the bounding Rectangle of the Widget
*
* \paragraph Sets the bounding rectangle of the widget
* in situations where the widget rectangle property needs
* to be set.
*
* \param  rect The Content bounding box area
*/

/**
  *\fn UI::AbstractDesktopWidget::setChildWidetVisibility()
  *
  * \brief Hides the child items of this widget
  *
  * \param show Decides the visibility of the child widgets, set true to show
  *the child elements
  * false to hide them.
  *
  */

/** UI::AbstractDesktopWidget::setLabel
  * \fn
  * \brief Sets a label for the widget
  * \param name Name of the icon
  *
  * Label to display when the widget is in dock mode
  */

namespace UIKit {
typedef std::function<void(Widget::InputEvent, const Widget *)>
EventCallbackFunc;

class Widget::PrivateAbstractDesktopWidget {
public:
  PrivateAbstractDesktopWidget()
      : m_widget_controller(0), mWidgetID(0) {}
  ~PrivateAbstractDesktopWidget() {}

  void _exec_func(Widget::InputEvent a_type, const Widget *a_widget_ptr);
  void _inoke_geometry_func(const QRectF &a_rect);

  QVariantMap mStyleAttributeMap;

  RenderLevel m_current_layer_type;
  QString m_widget_name;
  unsigned int mWidgetID;

  ViewController *m_widget_controller;

  std::vector<std::function<void(const QRectF &)> > m_on_geometry_func_list;
  std::vector<EventCallbackFunc> m_handler_list;
};

Widget::Widget(QGraphicsObject *parent)
    : QGraphicsObject(parent), QGraphicsLayoutItem(0),
      d(new PrivateAbstractDesktopWidget) {
  d->m_widget_name = QLatin1String("Widget");
  d->m_current_layer_type = kRenderAtForgroundLevel;

  setCacheMode(DeviceCoordinateCache);
  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
  setGraphicsItem(this);
}

Widget::~Widget() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QRectF Widget::boundingRect() const {
  return QRectF(QPointF(0, 0), geometry().size()); // d->m_content_geometry;
}

void Widget::set_widget_flag(int a_flags, bool a_enable) {}

void Widget::on_input_event(
    std::function<void(InputEvent, const Widget *)> a_callback) {
  d->m_handler_list.push_back(a_callback);
}

void
Widget::on_geometry_changed(std::function<void(const QRectF &)> a_callback) {
  d->m_on_geometry_func_list.push_back(a_callback);
}

void Widget::set_style_attribute(const QString &a_key, QVariant a_data) {
  d->mStyleAttributeMap[a_key] = a_data;
}

QVariant Widget::style_attribute(const QString &aKey) {
  return d->mStyleAttributeMap[aKey];
}

void Widget::set_widget_name(const QString &a_name) {
  d->m_widget_name = a_name;
}

QString Widget::label() const { return d->m_widget_name; }

void Widget::set_widget_id(unsigned int a_id) { d->mWidgetID = a_id; }

unsigned Widget::widget_id() const { return d->mWidgetID; }

StylePtr Widget::style() const { return ResourceManager::style(); }

Widget::RenderLevel Widget::layer_type() const {
  return d->m_current_layer_type;
}

void Widget::set_layer_type(RenderLevel a_level) const {
  d->m_current_layer_type = a_level;
}

void Widget::paint(QPainter *a_painter_ptr,
                   const QStyleOptionGraphicsItem *a_option_ptr,
                   QWidget * /*widget*/) {
  if (!a_painter_ptr->isActive()) {
    return;
  }
  if (isObscured()) {
    return;
  }

  a_painter_ptr->setClipRect(boundingRect());
  paint_view(a_painter_ptr, boundingRect());
}

void Widget::setGeometry(const QRectF &a_rect) {
  // d->m_content_geometry = rect;
  prepareGeometryChange();
  QGraphicsLayoutItem::setGeometry(a_rect);
  setPos(a_rect.topLeft());

  d->_inoke_geometry_func(a_rect);
}

QSizeF Widget::sizeHint(Qt::SizeHint a_which,
                        const QSizeF &a_constraint) const {
  // todo: ignoreing which for now. we will return based on
  // return geometry().size();
  QSizeF sh;
  switch (a_which) {
  case Qt::MinimumSize:
    sh = QSizeF(0, 0);
    break;
  case Qt::PreferredSize:
    sh = QSizeF(50, 50); // rather arbitrary
    break;
  case Qt::MaximumSize:
    sh = QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    break;
  default:
    qWarning("QGraphicsWidget::sizeHint(): Don't know how to handle the "
             "value of 'which'");
    break;
  }
  return sh;
}

void Widget::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  /*
  StyleFeatures feature;
  feature.geometry = rect;

  if (style()) {
  style()->draw("window_frame", feature, painter);
  }
  */
}

void Widget::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  // todo : check why mouse release events are not called.
  // https://github.com/plexydesk/plexydesk/issues/7
  d->_exec_func(kMousePressedEvent, this);

  setFocus(Qt::MouseFocusReason);
  // QGraphicsObject::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  qDebug() << Q_FUNC_INFO << metaObject()->className();

  d->_exec_func(kMouseReleaseEvent, this);

  // QGraphicsObject::mouseReleaseEvent(event);
}

void Widget::focusOutEvent(QFocusEvent *event) {
  event->accept();

  d->_exec_func(kFocusOutEvent, this);

  QGraphicsObject::focusOutEvent(event);
}

float Widget::scale_factor_for_width() const {
  return geometry().width() / boundingRect().width();
}

float Widget::scale_factor_for_height() const {
  return geometry().height() / boundingRect().height();
}

void Widget::set_child_widet_visibility(bool a_visibility) {
  Q_FOREACH(QGraphicsItem * item, this->childItems()) {
    (a_visibility) ? item->show() : item->hide();
  }
}

void Widget::set_controller(ViewController *a_view_controller_ptr) {
  d->m_widget_controller = a_view_controller_ptr;
}

ViewController *Widget::controller() const { return d->m_widget_controller; }

void
Widget::PrivateAbstractDesktopWidget::_exec_func(InputEvent a_type,
                                                 const Widget *a_widget_ptr) {
  std::for_each(std::begin(m_handler_list), std::end(m_handler_list),
                [&](EventCallbackFunc a_func) {
    if (a_func)
      a_func(a_type, a_widget_ptr);
    else
      qWarning() << Q_FUNC_INFO << "Fatal Error : Function out of scope";
  });
}

void Widget::PrivateAbstractDesktopWidget::_inoke_geometry_func(
    const QRectF &a_rect) {
  std::for_each(std::begin(m_on_geometry_func_list),
                std::end(m_on_geometry_func_list),
                [&](std::function<void(const QRectF &)> a_func) {
    if (a_func)
      a_func(a_rect);
    else
      qDebug() << Q_FUNC_INFO << "INvalid function pointer";
  });
}

} // namespace PlexyDesk
