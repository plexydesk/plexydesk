#include "themepackloader.h"
#include "window.h"

#include <QRectF>
#include <QPointF>
#include <QSizeF>

#include <button.h>
#include <windowbutton.h>
#include <space.h>

#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

namespace UIKit {
typedef std::function<void(Window *)> WindowActionCallback;
class Window::PrivateWindow {
public:
  PrivateWindow() : m_window_content(0), mWindowBackgroundVisibility(true) {}
  ~PrivateWindow() {}

  QRectF m_window_geometry;
  Widget *m_window_content;
  Space *m_window_viewport;
  QString m_window_title;

  WindowType m_window_type;
  bool mWindowBackgroundVisibility;

  WindowButton *m_window_close_button;

  std::function<void(const QSizeF &size)> m_window_size_callback;
  std::function<void(const QPointF &size)> m_window_move_callback;

  std::vector<std::function<void(const QPointF &size)>> m_window_move_cb_list;
  std::vector<WindowActionCallback> m_window_close_callback_list;
  std::vector<WindowActionCallback> m_window_focus_callback_list;

  std::function<void(Window *)> m_window_discard_callback;
};

void Window::invoke_window_closed_action()
{
  std::for_each(std::begin(m_priv_impl->m_window_close_callback_list),
                std::end(m_priv_impl->m_window_close_callback_list),
                [&](std::function<void (Window *)> a_func) {
    if (a_func)
      a_func(this);
  });
}

void Window::invoke_window_moved_action()
{
  std::for_each(std::begin(m_priv_impl->m_window_move_cb_list),
                std::end(m_priv_impl->m_window_move_cb_list),
                [&](std::function<void (const QPointF &)> a_func) {
    if (a_func)
      a_func(this->pos());
  });
}

Window::Window(QGraphicsObject *parent)
    : Widget(parent), m_priv_impl(new PrivateWindow) {
  set_widget_flag(Widget::kRenderBackground, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setGeometry(QRectF(0, 0, 400, 400));
  set_window_title("");
  m_priv_impl->m_window_type = kApplicationWindow;

  m_priv_impl->m_window_close_button = new WindowButton(this);
  m_priv_impl->m_window_close_button->setPos(5, 5);
  m_priv_impl->m_window_close_button->show();
  m_priv_impl->m_window_close_button->setZValue(10000);

  setFocus(Qt::MouseFocusReason);

  on_input_event([this](Widget::InputEvent aEvent, const Widget *aWidget) {
    if (aEvent == Widget::kFocusOutEvent &&
        m_priv_impl->m_window_type == kPopupWindow) {
      hide();
    }
  });

  m_priv_impl->m_window_close_button->on_input_event([this](
      Widget::InputEvent aEvent, const Widget *aWidget) {
    if (aEvent == Widget::kMouseReleaseEvent) {

        invoke_window_closed_action();
    }
  });
}

Window::~Window() {
  qDebug() << Q_FUNC_INFO;
  delete m_priv_impl;
}

void Window::set_window_content(Widget *a_widget_ptr) {
  if (m_priv_impl->m_window_content) {
    return;
  }

  m_priv_impl->m_window_content = a_widget_ptr;
  m_priv_impl->m_window_content->setParentItem(this);

  float sWindowTitleHeight = 0;

  if (UIKit::Theme::style()) {
    sWindowTitleHeight = UIKit::Theme::style()
                             ->attribute("frame", "window_title_height")
                             .toFloat();
  }

  QRectF content_geometry(a_widget_ptr->boundingRect());
  content_geometry.setHeight(content_geometry.height() + sWindowTitleHeight);



  if (m_priv_impl->m_window_type == kApplicationWindow) {
    m_priv_impl->m_window_content->setPos(0.0, sWindowTitleHeight);
    setGeometry(content_geometry);
  } else {
    m_priv_impl->m_window_close_button->hide();
    setGeometry(m_priv_impl->m_window_content->boundingRect());
  }

  if (m_priv_impl->m_window_type != kFramelessWindow) {
    QGraphicsDropShadowEffect *lEffect = new QGraphicsDropShadowEffect(this);
    lEffect->setColor(QColor("#111111"));
    lEffect->setBlurRadius(26);
    lEffect->setXOffset(0);
    lEffect->setYOffset(0);
    setGraphicsEffect(lEffect);
  }
}

void Window::set_window_viewport(Space *a_space) {
  m_priv_impl->m_window_viewport = a_space;
}

void Window::set_window_title(const QString &a_window_title) {
  m_priv_impl->m_window_title = a_window_title;
  update();
}

QString Window::window_title() const { return m_priv_impl->m_window_title; }

Window::WindowType Window::window_type() { return m_priv_impl->m_window_type; }

void Window::set_window_type(Window::WindowType a_window_type) {
  m_priv_impl->m_window_type = a_window_type;

  if (a_window_type == kApplicationWindow && m_priv_impl->m_window_content) {
    m_priv_impl->m_window_content->setPos(0.0, 72.0);
  } else {
    m_priv_impl->m_window_close_button->hide();
  }

  if (m_priv_impl->m_window_type == kPopupWindow) {
    setZValue(10000);
  }
}

void Window::on_window_resized(std::function<void(const QSizeF &)> handler) {
  m_priv_impl->m_window_size_callback = handler;
}

void Window::on_window_moved(std::function<void(const QPointF &)> a_handler) {
  m_priv_impl->m_window_move_callback = a_handler;
  m_priv_impl->m_window_move_cb_list.push_back(a_handler);
}

void Window::on_window_closed(std::function<void(Window *)> a_handler) {
  //m_priv_impl->m_window_close_callback = a_handler;
  m_priv_impl->m_window_close_callback_list.push_back(a_handler);
}

void Window::on_window_discarded(std::function<void(Window *)> a_handler) {
    m_priv_impl->m_window_discard_callback = a_handler;
}

void Window::on_window_focused(std::function<void (Window *)> a_handler) {
  m_priv_impl->m_window_focus_callback_list.push_back(a_handler);
}

void Window::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect_ptr) {
  if (!m_priv_impl->mWindowBackgroundVisibility) {
    return;
  }

  StyleFeatures feature;
  feature.geometry = a_rect_ptr;
  feature.text_data = m_priv_impl->m_window_title;

  if (style()) {
    style()->draw("window_frame", feature, a_painter_ptr);
  }
}

void Window::show() {
  setVisible(true);
  setFocus(Qt::MouseFocusReason);
}

void Window::hide() {
  // Qt 5.4 -> QTimer::singleShot(250, []() { windowHide(); } );
  QTimer *lTimer = new QTimer(this);
  lTimer->setSingleShot(true);

  connect(lTimer, &QTimer::timeout, [=]() {
    setVisible(false);
    delete lTimer;
  });

  lTimer->start(250);
}

void Window::discard() {
  if (m_priv_impl->m_window_discard_callback) {
    qDebug() << Q_FUNC_INFO << "Discard Requested: Notifiy";
    m_priv_impl->m_window_discard_callback(this);
    }
}

void Window::resize(float a_width, float a_height) {
  setGeometry(QRectF(x(), y(),
                     a_width, a_height + 72.0));

  if (m_priv_impl->m_window_content) {
       if (m_priv_impl->m_window_type == kApplicationWindow) {
           m_priv_impl->m_window_content->setPos(0.0, 72.0);
       }
   }
}

void Window::enable_window_background(bool a_visibility) {
  m_priv_impl->mWindowBackgroundVisibility = a_visibility;
}

void Window::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  //window focus notifycatoin.
  if (m_priv_impl->m_window_type == kApplicationWindow) {
  std::for_each(std::begin(m_priv_impl->m_window_focus_callback_list),
                std::end(m_priv_impl->m_window_focus_callback_list),
                [&](WindowActionCallback a_func) {
      if (a_func)
          a_func(this);
  });
  }
  QGraphicsObject::mousePressEvent(event);
}

void Window::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  invoke_window_moved_action();
  QGraphicsObject::mouseReleaseEvent(a_event_ptr);
}
}
