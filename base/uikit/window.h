#ifndef WINDOW_H
#define WINDOW_H

#include <functional>
#include <widget.h>

class QRectF;
class QPointF;
class QSizeF;

namespace UI {
class Space;

class Window : public Widget
{
public:
    Window(QGraphicsObject *parent = 0);
    virtual ~Window();

    virtual void setWindowContent(Widget *widget);
    virtual void setWindowViewport(Space *space);

    virtual void setWindowResizeCallback(
            std::function<void (const QSizeF &size)> handler);
    virtual void setWindowMoveCallback(
            std::function<void (const QPointF &pos)> handler);
private:
    class PrivateWindow;
    PrivateWindow * const d;
};
}
#endif // WINDOW_H
