#ifndef LABEL_H
#define LABEL_H

#include <widget.h>

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>

#include <style.h>
#include <plexydesk_ui_exports.h>

namespace UIKit {

class DECL_UI_KIT_EXPORT Label : public Widget {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  explicit Label(QGraphicsObject *a_parent_ptr = 0);

  virtual ~Label();

  virtual void set_label(const QString &a_txt);
  virtual QString label() const;

  virtual void set_size(const QSizeF &_asize);
  virtual void set_font_size(uint a_pixelSize);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;
  virtual QRectF boundingRect() const;
  virtual void setGeometry(const QRectF &a_rect);
  virtual QRectF contents_bounding_rect() const;

  virtual void set_label_style(const QColor &a_backgroundColor,
                               const QColor &a_textColor);

  virtual int alignment();
  virtual void set_alignment(int a_alignment);

Q_SIGNALS:
  void contentBoundingRectChaned();

protected:
  virtual void paint(QPainter *a_painter_ptr,
                     const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget *a_widget_ptr = 0);

private:
  class PrivateLabel;
  PrivateLabel *const d;
};
}

#endif // LABEL_H
