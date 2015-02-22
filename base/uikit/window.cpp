#include "window.h"

#include <QRectF>
#include <QPointF>
#include <QSizeF>

#include <button.h>
#include <windowbutton.h>
#include <space.h>

#include <QDebug>
#include <QTimer>

namespace UIKit {
class Window::PrivateWindow {
public:
    PrivateWindow() :  m_window_content(0){}
    ~PrivateWindow() {}

    QRectF m_window_geometry;
    Widget *m_window_content;
    Space *m_window_viewport;
    QString m_window_title;

    WindowType m_window_type;

    WindowButton *m_window_close_button;

    std::function<void (const QSizeF &size)> m_window_size_callback;
    std::function<void (const QPointF &size)> m_window_move_callback;
    std::function<void ()> m_window_close_callback;
};

Window::Window(QGraphicsObject *parent) : Widget(parent), d(new PrivateWindow)
{
    setWindowFlag(Widget::kRenderBackground, true);
    setGeometry(QRectF(0, 0, 400, 400));
    setWindowTitle("Test");
    d->m_window_type = kApplicationWindow;

    d->m_window_close_button = new WindowButton(this);
    d->m_window_close_button->setPos(5,5);
    d->m_window_close_button->show();
    d->m_window_close_button->setZValue(10000);

    setFocus(Qt::MouseFocusReason);

    connect(this, &Widget::focusLost, [&]() {
        qDebug() << Q_FUNC_INFO << "Lost focus";
        if (d->m_window_type == kPopupWindow)
            hide();
    });
}

Window::~Window()
{
    delete d;
}

void Window::setWindowContent(Widget *widget)
{
    if (d->m_window_content)
        return;

    d->m_window_content = widget;
    d->m_window_content->setParentItem(this);

    this->setGeometry(d->m_window_content->boundingRect());

    if (d->m_window_type == kApplicationWindow) {
       d->m_window_content->setPos(0.0, 72.0);
    } else {
       d->m_window_close_button->hide();
    }
}

void Window::setWindowViewport(Space *space)
{
    d->m_window_viewport = space;
}

void Window::setWindowTitle(const QString &a_window_title)
{
    d->m_window_title = a_window_title;
}

QString Window::windowTitlte() const
{
    return d->m_window_title;
}

Window::WindowType Window::windowType()
{
    return d->m_window_type;
}

void Window::setWindowType(Window::WindowType a_window_type)
{
    d->m_window_type = a_window_type;

    if (a_window_type == kApplicationWindow &&
            d->m_window_content) {
       d->m_window_content->setPos(0.0, 72.0);
    } else {
       d->m_window_close_button->hide();
    }
}

void Window::setWindowResizeCallback(
        std::function<void (const QSizeF &)> handler)
{
    d->m_window_size_callback = handler;
}

void Window::setWindowMoveCallback(std::function<void (const QPointF &)> handler)
{
    d->m_window_move_callback = handler;
}

void Window::setWindowCloseCallback(std::function<void ()> handler)
{
    d->m_window_close_callback = handler;
}

void Window::paintView(QPainter *painter, const QRectF &rect)
{
    StyleFeatures feature;
    feature.geometry = rect;
    feature.text_data = d->m_window_title;

    if (style()) {
      style()->draw("window_frame", feature, painter);
    }
}

void Window::show()
{
    setVisible(true);
    setFocus(Qt::MouseFocusReason);
}

void Window::hide()
{
    // Qt 5.4 -> QTimer::singleShot(250, []() { windowHide(); } );
    QTimer *lTimer = new QTimer(this);
    lTimer->setSingleShot(true);

    connect(lTimer, &QTimer::timeout, [=]() {
        setVisible(false);
        delete lTimer;
    } );

    lTimer->start(250);
}

}
