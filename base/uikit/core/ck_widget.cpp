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

#include <QGraphicsObject>
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

#include "ck_resource_manager.h"
#include "ck_widget.h"
#include <ck_desktop_controller_interface.h>
#include <ck_screen.h>

namespace cherry_kit {
typedef std::function<void(widget::InputEvent, const widget *)>
    EventCallbackFunc;

class widget::PrivateWidget {
public:
  PrivateWidget()
      : m_surface(0), m_widget_controller(0), m_identifier(0), m_screen_id(0) {}
  ~PrivateWidget() {
    if (m_surface)
      free(m_surface);
  }

  void _invoke_geometry_func(const QRectF &a_rect);

  QVariantMap mStyleAttributeMap;

  RenderLevel m_current_layer_type;
  QString m_name;
  unsigned int m_identifier;

  desktop_controller_interface *m_widget_controller;

  std::vector<std::function<void(const QRectF &)>> m_on_geometry_func_list;
  std::vector<EventCallbackFunc> m_handler_list;
  std::vector<UpdateCallback> m_update_monitor_list;
  std::vector<std::function<void()>> m_on_click_handlers;

  WidgetList m_child_list;
  unsigned char *m_surface;

  QRectF m_content_rect;

  int m_screen_id;
};

widget::widget(widget *parent)
    : QGraphicsObject(parent), QGraphicsLayoutItem(0), priv(new PrivateWidget) {
  priv->m_name = QLatin1String("Widget");
  priv->m_current_layer_type = kRenderAtForgroundLevel;

#ifdef Q_OS_MAC
  setCacheMode(ItemCoordinateCache);
#else
  setCacheMode(ItemCoordinateCache);
#endif

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
  setGraphicsItem(this);
}

widget::~widget() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

QRectF widget::contents_geometry() const {
  return priv->m_content_rect;// QRectF(QPointF(0, 0), geometry().size()); // d->m_content_geometry;
}

QRectF widget::boundingRect() const { return contents_geometry(); }

void widget::setGeometry(const QRectF &rect)
{
    qDebug() << Q_FUNC_INFO << "Geometry : " << rect;
    set_geometry(rect);
}

void widget::set_widget_flag(int a_flags, bool a_enable) {}

void widget::on_input_event(
    std::function<void(InputEvent, const widget *)> a_callback) {
  priv->m_handler_list.push_back(a_callback);
}

void widget::on_click(std::function<void()> a_callback) {
  priv->m_on_click_handlers.push_back(a_callback);
}

void widget::on_geometry_changed(
    std::function<void(const QRectF &)> a_callback) {
  priv->m_on_geometry_func_list.push_back(a_callback);
}

void widget::set_style_attribute(const QString &a_key, QVariant a_data) {
  priv->mStyleAttributeMap[a_key] = a_data;
}

QVariant widget::style_attribute(const QString &aKey) {
  return priv->mStyleAttributeMap[aKey];
}

void widget::draw() {
  if (!priv->m_surface)
    return;

  render(&priv->m_surface);
}

void widget::render(unsigned char **a_ctx) {
  if (!(*a_ctx))
    return;

  QImage paint_device(*a_ctx, geometry().width(), geometry().height(),
                      QImage::Format_ARGB32_Premultiplied);
  QPainter painter;
  painter.begin(&paint_device);
  painter.fillRect(geometry(), Qt::transparent);
  paint_view(&painter, geometry());
  painter.end();
}

GraphicsSurface *widget::surface() { return &priv->m_surface; }

void widget::request_update() {
  std::for_each(std::begin(priv->m_update_monitor_list),
                std::end(priv->m_update_monitor_list),
                [this](UpdateCallback a_func) {
                  if (a_func)
                    a_func(this);
                });
}

void widget::on_update(UpdateCallback a_callback) {
  priv->m_update_monitor_list.push_back(a_callback);
}

WidgetList widget::children() { return priv->m_child_list; }

void widget::set_screen_id(int a_screen_id) {
  // todo:
  // Move the widget to the next screen if the screen id changes.
  priv->m_screen_id = a_screen_id;
}

int widget::screen_id() const { return priv->m_screen_id; }

void widget::set_widget_name(const QString &a_name) {
  priv->m_name = a_name;
  request_update();
}

QString widget::text() const { return priv->m_name; }

void widget::set_widget_id(unsigned int a_id) { priv->m_identifier = a_id; }

unsigned widget::widget_id() const { return priv->m_identifier; }

StylePtr widget::style() const { return resource_manager::style(); }

widget::RenderLevel widget::layer_type() const {
  return priv->m_current_layer_type;
}

void widget::set_layer_type(RenderLevel a_level) {
  priv->m_current_layer_type = a_level;
  request_update();
}

void widget::paint(QPainter *a_painter_ptr,
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

void widget::set_geometry(const QRectF &a_rect) {
  if (!priv->m_surface) {
    priv->m_surface =
        (unsigned char *)malloc(4 * a_rect.width() * a_rect.height());
    memset(priv->m_surface, 0, 4 * a_rect.width() * a_rect.height());
  } else {
    priv->m_surface = (unsigned char *)realloc(
        priv->m_surface, 4 * a_rect.width() * a_rect.height());
    memset(priv->m_surface, 0, 4 * a_rect.width() * a_rect.height());
  }

  /*
  // d->m_content_geometry = rect;
  prepareGeometryChange();
  float scale_factor = 1; // screen().scale_factor();
  QRectF scaled_rect(a_rect.x(), a_rect.y(), a_rect.width() * scale_factor,
                     a_rect.height() * scale_factor);
  QGraphicsLayoutItem::setGeometry(scaled_rect);
  setPos(a_rect.topLeft());

  priv->_invoke_geometry_func(scaled_rect);

  setCacheMode(ItemCoordinateCache, boundingRect().size().toSize());
  request_update();
  */

  set_contents_geometry(a_rect.x(), a_rect.y(), a_rect.width(), a_rect.height());
}

void widget::set_contents_geometry(float a_x, float a_y, float a_width,
                                  float a_height) {
  priv->m_content_rect = QRectF(a_x, a_y, a_width, a_height);
  QRectF a_rect = priv->m_content_rect;

  prepareGeometryChange();
  float scale_factor = 1; // screen().scale_factor();
  QRectF scaled_rect(a_rect.x(), a_rect.y(), a_rect.width() * scale_factor,
                     a_rect.height() * scale_factor);

  //setGeometry(scaled_rect);
  QGraphicsLayoutItem::setGeometry(scaled_rect);

  setPos(mapFromScene(a_rect.topLeft()));
  priv->_invoke_geometry_func(scaled_rect);
  setCacheMode(ItemCoordinateCache, boundingRect().size().toSize());
  request_update();
}

QSizeF widget::sizeHint(Qt::SizeHint a_which,
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

void widget::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  /*
  StyleFeatures feature;
  feature.geometry = a_rect;

  if (style()) {
    ResourceManager::style()->draw("window_frame", feature, a_painter_ptr);
  }
  */
}

void widget::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  // todo : check why mouse release events are not called.
  // https://github.com/plexydesk/plexydesk/issues/7
  exec_func(kMousePressedEvent, this);

  setFocus(Qt::MouseFocusReason);
  // QGraphicsObject::mousePressEvent(event);
}

void widget::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  exec_func(kMouseReleaseEvent, this);
  invoke_click_handlers();
}

void widget::focusOutEvent(QFocusEvent *event) {
  event->accept();

  exec_func(kFocusOutEvent, this);
  QGraphicsObject::focusOutEvent(event);
}

float widget::scale_factor_for_width() const {
  return geometry().width() / boundingRect().width();
}

float widget::scale_factor_for_height() const {
  return geometry().height() / boundingRect().height();
}

void widget::set_child_widet_visibility(bool a_visibility) {
  Q_FOREACH (QGraphicsItem *item, this->childItems()) {
    (a_visibility) ? item->show() : item->hide();
  }
}

void widget::set_controller(
    desktop_controller_interface *a_view_controller_ptr) {
  priv->m_widget_controller = a_view_controller_ptr;
}

desktop_controller_interface *widget::controller() const {
  return priv->m_widget_controller;
}

void widget::exec_func(InputEvent a_type, const widget *a_widget_ptr) {
  std::for_each(std::begin(priv->m_handler_list),
                std::end(priv->m_handler_list), [&](EventCallbackFunc a_func) {
                  if (a_func)
                    a_func(a_type, a_widget_ptr);
                  else
                    qWarning() << Q_FUNC_INFO
                               << "Fatal Error : Function out of scope";
                });
}

void widget::invoke_click_handlers() {
  std::for_each(std::begin(priv->m_on_click_handlers),
                std::end(priv->m_on_click_handlers),
                [&](std::function<void()> a_func) {
                  if (a_func)
                    a_func();
                  else
                    qDebug() << Q_FUNC_INFO << "INvalid function pointer";
                });
}

void widget::PrivateWidget::_invoke_geometry_func(const QRectF &a_rect) {
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
