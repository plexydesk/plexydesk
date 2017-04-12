#ifndef TEXT_VIEW_H
#define TEXT_VIEW_H

#include <plexydesk_ui_exports.h>
#include <ck_widget.h>

class QGraphicsSceneMouseEvent;
namespace cherry_kit {


class DECL_UI_KIT_EXPORT text_view : public widget
{
public:
    text_view(widget *a_parent = 0);
    virtual ~text_view();

    void set_text(const std::string &a_text);

protected:
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);

  virtual void keyPressEvent(QKeyEvent *a_event_ptr);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *a_event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event);

private:
    class text_view_context;
    text_view_context * const ctx;
};

}
#endif // TEXT_VIEW_H
