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
typedef std::function<void(window *, bool)> WindowVisibilityCallback;
class window::PrivateWindow {
public:
  PrivateWindow()
      : m_window_opacity(1.0f), m_window_content(0),
        mWindowBackgroundVisibility(true) {}
  ~PrivateWindow() {}

  float m_window_opacity;
  QRectF m_window_geometry;
  widget *m_window_content;
  space *m_window_viewport;
  QString m_window_title;

  WindowType m_window_type;
  bool mWindowBackgroundVisibility;

  window_button *m_window_close_button;

  std::function<void(const QSizeF &size)> m_window_size_callback;
  std::function<void(const QPointF &size)> m_window_move_callback;

  std::vector<std::function<void(const QPointF &size)>> m_window_move_cb_list;

  std::vector<window::ResizeCallback> m_window_resized_callback_list;
  std::vector<WindowActionCallback> m_window_close_callback_list;
  std::vector<WindowActionCallback> m_window_focus_callback_list;
  std::vector<WindowVisibilityCallback> m_window_visibility_callback_list;

  std::function<void(window *)> m_window_discard_callback;
};

void window::invoke_window_closed_action() {
  std::for_each(std::begin(priv->m_window_close_callback_list),
                std::end(priv->m_window_close_callback_list),
                [&](std::function<void(window *)> a_func) {
    if (a_func)
      a_func(this);
  });
}

void window::invoke_window_visibility_action(bool a_visibility) {
  std::for_each(std::begin(priv->m_window_visibility_callback_list),
                std::end(priv->m_window_visibility_callback_list),
                [&](WindowVisibilityCallback a_func) {
    if (a_func)
      a_func(this, a_visibility);
  });
}

void window::invoke_window_moved_action() {
  std::for_each(std::begin(priv->m_window_move_cb_list),
                std::end(priv->m_window_move_cb_list),
                [&](std::function<void(const QPointF &)> a_func) {
    if (a_func)
      a_func(this->pos());
  });
}

window::window(widget *parent) : widget(parent), priv(new PrivateWindow) {
  set_widget_flag(widget::kRenderBackground, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setGeometry(QRectF(0, 0, 400, 400));
  set_window_title("");
  priv->m_window_type = kApplicationWindow;

  priv->m_window_close_button = new window_button(this);
  priv->m_window_close_button->setPos(8, 8);
  priv->m_window_close_button->show();
  // todo:
  // set the correct z-order
  priv->m_window_close_button->setZValue(10000);

  setFocus(Qt::MouseFocusReason);

  priv->m_window_close_button->on_click(
      [this]() { invoke_window_closed_action(); });
}

window::~window() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

float window::window_title_height() {
  if (cherry_kit::resource_manager::style()) {
    return cherry_kit::resource_manager::style()
        ->attribute("frame", "window_title_height")
        .toFloat();
  }

  return 64;
}

void window::removeFocus() { invoke_window_visibility_action(0); }

void window::set_window_content(widget *a_widget_ptr) {
  if (priv->m_window_content) {
    return;
  }

  priv->m_window_content = a_widget_ptr;
  priv->m_window_content->setParentItem(this);

  float window_bordr_height = window_title_height();

  QRectF content_geometry(a_widget_ptr->boundingRect());
#ifdef __APPLE__
  content_geometry.setHeight(content_geometry.height() + window_title_height() +
                             30);
  content_geometry.setWidth(content_geometry.width() + 30);
#else
  content_geometry.setHeight(content_geometry.height() + window_title_height());
  content_geometry.setWidth(content_geometry.width() + 2);
#endif

  if (priv->m_window_type == kApplicationWindow) {
#ifdef __APPLE__
    priv->m_window_content->setPos(15.0, window_bordr_height + 10);
    content_geometry.setHeight(content_geometry.height());
#else
    priv->m_window_content->setPos(1.0, window_bordr_height);
    content_geometry.setHeight(content_geometry.height() + 8);
#endif
    setGeometry(content_geometry);
  } else if (priv->m_window_type == kNotificationWindow) {
#ifdef __APPLE__
    priv->m_window_content->setPos(15.0, window_bordr_height + 10);
#else
    priv->m_window_content->setPos(0.0, window_bordr_height);
#endif

    priv->m_window_close_button->hide();
    setGeometry(content_geometry);
  } else if (priv->m_window_type == kPanelWindow) {
    priv->m_window_close_button->hide();
#ifdef __APPLE__
    setGeometry(content_geometry);
    priv->m_window_content->setPos(15.0, window_bordr_height);
#else
    setGeometry(priv->m_window_content->geometry());
    priv->m_window_content->setPos(0, 4);
#endif
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
  } else if (priv->m_window_type == kPopupWindow) {
    priv->m_window_close_button->hide();
    setGeometry(content_geometry);
#ifdef __APPLE__
    priv->m_window_content->setPos(15, window_bordr_height + 10);
#else
    priv->m_window_content->setPos(0, window_bordr_height);
#endif
  } else {
    priv->m_window_close_button->hide();
    setGeometry(content_geometry);
    priv->m_window_content->setPos(0,
                                   window_bordr_height + window_title_height());
  }

  if (priv->m_window_type != kFramelessWindow) {
#ifndef __APPLE__
    QGraphicsDropShadowEffect *lEffect = new QGraphicsDropShadowEffect(this);
    lEffect->setColor(QColor("#111111"));
    lEffect->setBlurRadius(26);
    lEffect->setXOffset(0);
    lEffect->setYOffset(0);
    setGraphicsEffect(lEffect);
    setCacheMode(NoCache);
#endif
  }

  if (priv->m_window_type == kFramelessWindow) {
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    enable_window_background(false);
  }

  request_update();
}

void window::set_window_viewport(space *a_space) {
  priv->m_window_viewport = a_space;
}

void window::set_window_title(const QString &a_window_title) {
  priv->m_window_title = a_window_title;
  update();
  request_update();
}

QString window::window_title() const { return priv->m_window_title; }

window::WindowType window::window_type() { return priv->m_window_type; }

void window::set_window_type(window::WindowType a_window_type) {
  priv->m_window_type = a_window_type;

  if (a_window_type == kApplicationWindow && priv->m_window_content) {
    priv->m_window_content->setPos(0.0, window_title_height());
  } else {
    priv->m_window_close_button->hide();
  }

  if (priv->m_window_type == kPopupWindow) {
    setZValue(10000);
  }

  if (priv->m_window_type == kFramelessWindow) {
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    // todo : globaly expose z-index of each window layer.
    enable_window_background(false);
  }
}

void window::on_window_resized(ResizeCallback a_handler) {
  priv->m_window_resized_callback_list.push_back(a_handler);
}

void window::on_window_moved(std::function<void(const QPointF &)> a_handler) {
  priv->m_window_move_callback = a_handler;
  priv->m_window_move_cb_list.push_back(a_handler);
}

void window::on_window_closed(std::function<void(window *)> a_handler) {
  // m_priv_impl->m_window_close_callback = a_handler;
  priv->m_window_close_callback_list.push_back(a_handler);
}

void window::on_window_discarded(std::function<void(window *)> a_handler) {
  priv->m_window_discard_callback = a_handler;
}

void window::on_window_focused(std::function<void(window *)> a_handler) {
  priv->m_window_focus_callback_list.push_back(a_handler);
}

void
window::on_visibility_changed(std::function<void(window *, bool)> a_handler) {
  priv->m_window_visibility_callback_list.push_back(a_handler);
}

void window::raise() { invoke_focus_handlers(); }

void window::close() { invoke_window_closed_action(); }

void window::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect_ptr) {
  if (!priv->mWindowBackgroundVisibility) {
    return;
  }

  style_data feature;
  feature.geometry = a_rect_ptr;
  feature.text_data = priv->m_window_title;
  feature.opacity = priv->m_window_opacity;
  feature.style_object = this;

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
  if (priv->m_window_discard_callback) {
    qDebug() << Q_FUNC_INFO << "Discard Requested: Notifiy";
    priv->m_window_discard_callback(this);
  }
}

float window::window_opacity() const { return priv->m_window_opacity; }

void window::set_window_opacity(float a_value) {
  priv->m_window_opacity = a_value;
  update();
}

void window::resize(float a_width, float a_height) {
  setGeometry(QRectF(x(), y(), a_width, a_height + window_title_height()));

  if (priv->m_window_content) {
    if (priv->m_window_type == kApplicationWindow ||
        priv->m_window_type == kPopupWindow) {
      priv->m_window_content->setPos(0.0, window_title_height());
    }
  }

  std::for_each(std::begin(priv->m_window_resized_callback_list),
                std::end(priv->m_window_resized_callback_list),
                [&](ResizeCallback a_callback) {
    if (a_callback)
      a_callback(this, geometry().width(), geometry().height());
  });

  update();
  request_update();
}

void window::enable_window_background(bool a_visibility) {
  priv->mWindowBackgroundVisibility = a_visibility;
}

void window::invoke_focus_handlers() {
  std::for_each(std::begin(priv->m_window_focus_callback_list),
                std::end(priv->m_window_focus_callback_list),
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
