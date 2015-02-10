#include "window.h"

#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <space.h>

namespace UI {
class Window::PrivateWindow {
public:
    PrivateWindow() :  m_window_content(0){}
    ~PrivateWindow() {}

    QRectF m_window_geometry;
    Widget *m_window_content;
    Space *m_window_viewport;

    std::function<void (const QSizeF &size)> m_window_size_callback;
    std::function<void (const QPointF &size)> m_window_move_callback;
};

Window::Window(QGraphicsObject *parent) : Widget(parent), d(new PrivateWindow)
{
    setWindowFlag(Widget::kRenderBackground, true);
    setGeometry(QRectF(0, 0, 400, 400));
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
    //d->m_window_content->setParent(this);
    //d->m_window_content->setParentItem(this);

    this->setGeometry(d->m_window_content->boundingRect());
}

void Window::setWindowViewport(Space *space)
{
    d->m_window_viewport = space;
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
}
