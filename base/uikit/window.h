#ifndef WINDOW_H
#define WINDOW_H

#include <functional>
#include <widget.h>

class QRectF;
class QPointF;
class QSizeF;

namespace UIKit {
class Space;

class Window : public Widget
{
public:
    typedef enum {
        kFramelessWindow,
        kPanelWindow,
        kApplicationWindow,
        kNotificationWindow,
        kDialogWindow,
        kMessageWindow,
        kPopupWindow
    } WindowType;

    Window(QGraphicsObject *parent = 0);
    virtual ~Window();

    virtual void setWindowContent(Widget *widget);
    virtual void setWindowViewport(Space *space);

    virtual void setWindowTitle(const QString &a_window_title);
    virtual QString windowTitlte() const;

    virtual WindowType windowType();
    virtual void setWindowType(WindowType a_window_type);

    virtual void setWindowResizeCallback(
            std::function<void (const QSizeF &size)> handler);
    virtual void setWindowMoveCallback(
            std::function<void (const QPointF &pos)> handler);
    virtual void setWindowCloseCallback(
            std::function<void ()> handler);
protected:
    virtual void paintView(QPainter *painter, const QRectF &rect);

private:
    class PrivateWindow;
    PrivateWindow * const d;
};
}
#endif // WINDOW_H
