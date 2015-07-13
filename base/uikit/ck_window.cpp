#include "ck_resource_manager.h"
#include "ck_window.h"

#include <QRectF>
#include <QPointF>
#include <QSizeF>

#include <ck_button.h>
#include <ck_window_button.h>
#include <ck_space.h>

#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>

namespace cherry_kit {
typedef std::function<void(window *)> WindowActionCallback;
class window::PrivateWindow {
public:
  PrivateWindow() : m_window_content(0), mWindowBackgroundVisibility(true) {}
  ~PrivateWindow() {}

  QRectF m_window_geometry;
  widget *m_window_content;
  space *m_window_viewport;
  QString m_window_title;

  WindowType m_window_type;
  bool mWindowBackgroundVisibility;

  window_button *m_window_close_button;

  std::function<void(const QSizeF &size)> m_window_size_callback;
  std::function<void(const QPointF &size)> m_window_move_callback;

  std::vector<std::function<void(const QPointF &size)> > m_window_move_cb_list;

  std::vector<window::ResizeCallback> m_window_resized_callback_list;
  std::vector<WindowActionCallback> m_window_close_callback_list;
  std::vector<WindowActionCallback> m_window_focus_callback_list;

  std::function<void(window *)> m_window_discard_callback;
};

void window::invoke_window_closed_action() {
  std::for_each(std::begin(o_window->m_window_close_callback_list),
                std::end(o_window->m_window_close_callback_list),
                [&](std::function<void(window *)> a_func) {
    if (a_func)
      a_func(this);
  });
}

void window::invoke_window_moved_action() {
  std::for_each(std::begin(o_window->m_window_move_cb_list),
                std::end(o_window->m_window_move_cb_list),
                [&](std::function<void(const QPointF &)> a_func) {
    if (a_func)
      a_func(this->pos());
  });
}

window::window(widget *parent) : widget(parent), o_window(new PrivateWindow) {
  set_widget_flag(widget::kRenderBackground, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setGeometry(QRectF(0, 0, 400, 400));
  set_window_title("");
  o_window->m_window_type = kApplicationWindow;

  o_window->m_window_close_button = new window_button(this);
  o_window->m_window_close_button->setPos(5, 5);
  o_window->m_window_close_button->show();
  o_window->m_window_close_button->setZValue(10000);

  setFocus(Qt::MouseFocusReason);

  on_input_event([this](widget::InputEvent aEvent, const widget *aWidget) {
    if (aEvent == widget::kFocusOutEvent &&
        o_window->m_window_type == kPopupWindow) {
      hide();
      request_update();
    }
  });

  o_window->m_window_close_button->on_input_event([this](
      widget::InputEvent aEvent, const widget *aWidget) {
    if (aEvent == widget::kMouseReleaseEvent) {

      invoke_window_closed_action();
    }
  });
}

window::~window() {
  qDebug() << Q_FUNC_INFO;
  delete o_window;
}

void window::set_window_content(widget *a_widget_ptr) {
  if (o_window->m_window_content) {
    return;
  }

  o_window->m_window_content = a_widget_ptr;
  o_window->m_window_content->setParentItem(this);

  float sWindowTitleHeight = 0;

  if (cherry_kit::resource_manager::style()) {
    sWindowTitleHeight = cherry_kit::resource_manager::style()
                             ->attribute("frame", "window_title_height")
                             .toFloat();
  }

  QRectF content_geometry(a_widget_ptr->boundingRect());
  content_geometry.setHeight(content_geometry.height() + sWindowTitleHeight);

  if (o_window->m_window_type == kApplicationWindow) {
    o_window->m_window_content->setPos(0.0, sWindowTitleHeight);
    setGeometry(content_geometry);
  } else {
    o_window->m_window_close_button->hide();
    setGeometry(o_window->m_window_content->geometry());
    o_window->m_window_content->setPos(0, 0);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
  }

  if (o_window->m_window_type != kFramelessWindow) {
    QGraphicsDropShadowEffect *lEffect = new QGraphicsDropShadowEffect(this);
    lEffect->setColor(QColor("#111111"));
    lEffect->setBlurRadius(26);
    lEffect->setXOffset(0);
    lEffect->setYOffset(0);
    setGraphicsEffect(lEffect);
  }

  if (o_window->m_window_type == kFramelessWindow) {
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    enable_window_background(false);
  }

  request_update();
}

void window::set_window_viewport(space *a_space) {
  o_window->m_window_viewport = a_space;
}

void window::set_window_title(const QString &a_window_title) {
  o_window->m_window_title = a_window_title;
  update();
  request_update();
}

QString window::window_title() const { return o_window->m_window_title; }

window::WindowType window::window_type() { return o_window->m_window_type; }

void window::set_window_type(window::WindowType a_window_type) {
  o_window->m_window_type = a_window_type;

  if (a_window_type == kApplicationWindow && o_window->m_window_content) {
    o_window->m_window_content->setPos(0.0, 72.0);
  } else {
    o_window->m_window_close_button->hide();
  }

  if (o_window->m_window_type == kPopupWindow) {
    setZValue(10000);
  }

  if (o_window->m_window_type == kFramelessWindow) {
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    // todo : globaly expose z-index of each window layer.
    enable_window_background(false);
  }
}

void window::on_window_resized(ResizeCallback a_handler) {
  o_window->m_window_resized_callback_list.push_back(a_handler);
}

void window::on_window_moved(std::function<void(const QPointF &)> a_handler) {
  o_window->m_window_move_callback = a_handler;
  o_window->m_window_move_cb_list.push_back(a_handler);
}

void window::on_window_closed(std::function<void(window *)> a_handler) {
  // m_priv_impl->m_window_close_callback = a_handler;
  o_window->m_window_close_callback_list.push_back(a_handler);
}

void window::on_window_discarded(std::function<void(window *)> a_handler) {
  o_window->m_window_discard_callback = a_handler;
}

void window::on_window_focused(std::function<void(window *)> a_handler) {
  o_window->m_window_focus_callback_list.push_back(a_handler);
}

void window::raise() { invoke_focus_handlers(); }

void window::close() { invoke_window_closed_action(); }

void window::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect_ptr) {
  if (!o_window->mWindowBackgroundVisibility) {
    return;
  }

  style_data feature;
  feature.geometry = a_rect_ptr;
  feature.text_data = o_window->m_window_title;

  if (style()) {
    style()->draw("window_frame", feature, a_painter_ptr);
  }
}

void window::show() {
  setVisible(true);
  request_update();
}

void window::hide() {
  setVisible(false);
  request_update();
}

void window::discard() {
  if (o_window->m_window_discard_callback) {
    qDebug() << Q_FUNC_INFO << "Discard Requested: Notifiy";
    o_window->m_window_discard_callback(this);
  }
}

void window::resize(float a_width, float a_height) {
  setGeometry(QRectF(x(), y(), a_width, a_height + 72.0));

  if (o_window->m_window_content) {
    if (o_window->m_window_type == kApplicationWindow ||
        o_window->m_window_type == kPopupWindow) {
      o_window->m_window_content->setPos(0.0, 72.0);
    }
  }

  std::for_each(std::begin(o_window->m_window_resized_callback_list),
                std::end(o_window->m_window_resized_callback_list),
                [&](ResizeCallback a_callback) {
    if (a_callback)
      a_callback(this, geometry().width(), geometry().height());
  });

  update();
  request_update();
}

void window::enable_window_background(bool a_visibility) {
  o_window->mWindowBackgroundVisibility = a_visibility;
}

void window::invoke_focus_handlers() {
  std::for_each(std::begin(o_window->m_window_focus_callback_list),
                std::end(o_window->m_window_focus_callback_list),
                [&](WindowActionCallback a_func) {
    if (a_func)
      a_func(this);
  });
}

void window::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    invoke_focus_handlers();
  QGraphicsObject::mousePressEvent(event);
  request_update();
}

void window::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  invoke_window_moved_action();
  QGraphicsObject::mouseReleaseEvent(a_event_ptr);
  request_update();
}
}
