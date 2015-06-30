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

namespace UIKit {
typedef std::function<void(Widget::InputEvent, const Widget *)>
EventCallbackFunc;

class Widget::PrivateWidget {
public:
  PrivateWidget() : m_surface(0), m_widget_controller(0), m_identifier(0) {}
  ~PrivateWidget() {
    if (m_surface)
      free(m_surface);
  }

  void _exec_func(Widget::InputEvent a_type, const Widget *a_widget_ptr);
  void _inoke_geometry_func(const QRectF &a_rect);

  QVariantMap mStyleAttributeMap;

  RenderLevel m_current_layer_type;
  QString m_name;
  unsigned int m_identifier;

  ViewController *m_widget_controller;

  std::vector<std::function<void(const QRectF &)> > m_on_geometry_func_list;
  std::vector<EventCallbackFunc> m_handler_list;
  std::vector<UpdateCallback> m_update_monitor_list;

  WidgetList m_child_list;

  unsigned char *m_surface;
};

Widget::Widget(Widget *parent)
    : QGraphicsObject(parent), QGraphicsLayoutItem(0),
      p_widget(new PrivateWidget) {
  p_widget->m_name = QLatin1String("Widget");
  p_widget->m_current_layer_type = kRenderAtForgroundLevel;

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
  delete p_widget;
}

QRectF Widget::boundingRect() const {
  return QRectF(QPointF(0, 0), geometry().size()); // d->m_content_geometry;
}

void Widget::set_widget_flag(int a_flags, bool a_enable) {}

void Widget::on_input_event(
    std::function<void(InputEvent, const Widget *)> a_callback) {
  p_widget->m_handler_list.push_back(a_callback);
}

void
Widget::on_geometry_changed(std::function<void(const QRectF &)> a_callback) {
  p_widget->m_on_geometry_func_list.push_back(a_callback);
}

void Widget::set_style_attribute(const QString &a_key, QVariant a_data) {
  p_widget->mStyleAttributeMap[a_key] = a_data;
}

QVariant Widget::style_attribute(const QString &aKey) {
  return p_widget->mStyleAttributeMap[aKey];
}

void Widget::draw() {
  if (!p_widget->m_surface)
    return;

  render(&p_widget->m_surface);
}

void Widget::render(unsigned char **a_ctx) {
  qDebug() << Q_FUNC_INFO << "Rendering start";

  if (!(*a_ctx))
    return;

  QImage paint_device(*a_ctx, geometry().width(), geometry().height(),
                      QImage::Format_ARGB32_Premultiplied);
  QPainter painter;
  painter.begin(&paint_device);
  painter.fillRect(geometry(), Qt::transparent);
  paint_view(&painter, geometry());
  painter.end();
  qDebug() << Q_FUNC_INFO << "End";
}

GraphicsSurface *Widget::surface() { return &p_widget->m_surface; }

void Widget::request_update() {
   std::for_each(std::begin(p_widget->m_update_monitor_list),
                 std::end(p_widget->m_update_monitor_list),
                 [this](UpdateCallback a_func) {
     if (a_func)
         a_func(this);
   });
}

void Widget::on_update(UpdateCallback a_callback) {
    p_widget->m_update_monitor_list.push_back(a_callback);
}

WidgetList Widget::children() {
    return p_widget->m_child_list;
}

void Widget::set_widget_name(const QString &a_name) {
  p_widget->m_name = a_name;
  request_update();
}

QString Widget::label() const { return p_widget->m_name; }

void Widget::set_widget_id(unsigned int a_id) { p_widget->m_identifier = a_id; }

unsigned Widget::widget_id() const { return p_widget->m_identifier; }

StylePtr Widget::style() const { return ResourceManager::style(); }

Widget::RenderLevel Widget::layer_type() const {
  return p_widget->m_current_layer_type;
}

void Widget::set_layer_type(RenderLevel a_level) {
  p_widget->m_current_layer_type = a_level;
  request_update();
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

  if (!p_widget->m_surface) {
    p_widget->m_surface =
        (unsigned char *)malloc(4 * a_rect.width() * a_rect.height());
    memset(p_widget->m_surface, 0, 4 * a_rect.width() * a_rect.height());
  } else {
    p_widget->m_surface = (unsigned char *)realloc(
        p_widget->m_surface, 4 * a_rect.width() * a_rect.height());
    memset(p_widget->m_surface, 0, 4 * a_rect.width() * a_rect.height());
  }

  // d->m_content_geometry = rect;
  prepareGeometryChange();
  QGraphicsLayoutItem::setGeometry(a_rect);
  setPos(a_rect.topLeft());

  p_widget->_inoke_geometry_func(a_rect);

  request_update();
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
  feature.geometry = a_rect;

  if (style()) {
    ResourceManager::style()->draw("window_frame", feature, a_painter_ptr);
  }
  */
}

void Widget::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  // todo : check why mouse release events are not called.
  // https://github.com/plexydesk/plexydesk/issues/7
  p_widget->_exec_func(kMousePressedEvent, this);

  setFocus(Qt::MouseFocusReason);
  // QGraphicsObject::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  qDebug() << Q_FUNC_INFO << metaObject()->className();

  p_widget->_exec_func(kMouseReleaseEvent, this);

  // QGraphicsObject::mouseReleaseEvent(event);
}

void Widget::focusOutEvent(QFocusEvent *event) {
  event->accept();

  p_widget->_exec_func(kFocusOutEvent, this);

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
  p_widget->m_widget_controller = a_view_controller_ptr;
}

ViewController *Widget::controller() const {
  return p_widget->m_widget_controller;
}

void Widget::PrivateWidget::_exec_func(InputEvent a_type,
                                       const Widget *a_widget_ptr) {
  std::for_each(std::begin(m_handler_list), std::end(m_handler_list),
                [&](EventCallbackFunc a_func) {
    if (a_func)
      a_func(a_type, a_widget_ptr);
    else
      qWarning() << Q_FUNC_INFO << "Fatal Error : Function out of scope";
  });
}

void Widget::PrivateWidget::_inoke_geometry_func(const QRectF &a_rect) {
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
