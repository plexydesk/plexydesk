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
    QString m_window_title;

    std::function<void (const QSizeF &size)> m_window_size_callback;
    std::function<void (const QPointF &size)> m_window_move_callback;
    std::function<void ()> m_window_close_callback;
};

Window::Window(QGraphicsObject *parent) : Widget(parent), d(new PrivateWindow)
{
    setWindowFlag(Widget::kRenderBackground, true);
    setGeometry(QRectF(0, 0, 400, 400));
    setWindowTitle("Test");
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
    this->setGeometry(d->m_window_content->boundingRect());

    d->m_window_content->setPos(0.0, 72.0);
}

void Window::setWindowViewport(Space *space)
{
    d->m_window_viewport = space;
}

void Window::setWindowTitle(const QString &aTitle)
{
    d->m_window_title = aTitle;
}

QString Window::windowTitlte() const
{
    return d->m_window_title;
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
}
