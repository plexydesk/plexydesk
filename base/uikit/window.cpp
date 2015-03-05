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
class Window::PrivateWindow {
public:
    PrivateWindow() :  m_window_content(0),
    mWindowBackgroundVisibility(true){}
    ~PrivateWindow() {}

    QRectF m_window_geometry;
    Widget *m_window_content;
    Space *m_window_viewport;
    QString m_window_title;

    WindowType m_window_type;
    bool mWindowBackgroundVisibility;

    WindowButton *m_window_close_button;

    std::function<void (const QSizeF &size)> m_window_size_callback;
    std::function<void (const QPointF &size)> m_window_move_callback;
    std::function<void (Window *)> m_window_close_callback;
};

Window::Window(QGraphicsObject *parent) : Widget(parent), d(new PrivateWindow)
{
    setWindowFlag(Widget::kRenderBackground, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setGeometry(QRectF(0, 0, 400, 400));
    setWindowTitle("");
    d->m_window_type = kApplicationWindow;

    d->m_window_close_button = new WindowButton(this);
    d->m_window_close_button->setPos(5,5);
    d->m_window_close_button->show();
    d->m_window_close_button->setZValue(10000);

    setFocus(Qt::MouseFocusReason);

    onInputEvent([this](Widget::InputEvent aEvent, const Widget *aWidget) {
        if (aEvent == Widget::kFocusOutEvent &&
                d->m_window_type == kPopupWindow) {
            hide();
        }
    });

    d->m_window_close_button->onInputEvent([this](Widget::InputEvent aEvent,
                                               const Widget *aWidget) {
        if (aEvent == Widget::kMouseReleaseEvent) {
            if (d->m_window_close_callback)
                d->m_window_close_callback(this);

        }
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

    float sWindowTitleHeight = 0;

    if (UIKit::Theme::style()) {
        sWindowTitleHeight = UIKit::Theme::style()
                                 ->attrbute("frame", "window_title_height")
                                 .toFloat();
    }

    if (d->m_window_type == kApplicationWindow) {
       d->m_window_content->setPos(0.0, sWindowTitleHeight);
    } else {
       d->m_window_close_button->hide();
    }

    if (d->m_window_type != kFramelessWindow) {
        QGraphicsDropShadowEffect *lEffect = new QGraphicsDropShadowEffect(this);
        lEffect->setColor(QColor("#111111"));
        lEffect->setBlurRadius(26);
        lEffect->setXOffset(0);
        lEffect->setYOffset(0);
        setGraphicsEffect(lEffect);
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

    if (d->m_window_type == kPopupWindow) {
        setZValue(10000);
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

void Window::setWindowCloseCallback(std::function<void (Window *)> aCallback)
{
    d->m_window_close_callback = aCallback;
}

void Window::paintView(QPainter *painter, const QRectF &rect)
{
    if (!d->mWindowBackgroundVisibility)
        return;

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

void Window::setEnableWindowBackground(bool aVisible)
{
    d->mWindowBackgroundVisibility = aVisible;
}

void Window::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsObject::mousePressEvent(event);
}

void Window::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   QGraphicsObject::mouseReleaseEvent(event);
}

}
