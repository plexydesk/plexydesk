#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <style.h>
#include <widget.h>

#include <plexydesk_ui_exports.h>

namespace CherryKit {
class DECL_UI_KIT_EXPORT ProgressBar : public Widget {
  Q_OBJECT
public:
  explicit ProgressBar(Widget *a_parent_ptr = 0);
  virtual ~ProgressBar();

  virtual void set_label(const QString &a_txt);
  virtual QString label() const;

  virtual void set_size(const QSizeF &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;

  virtual int max_range();
  virtual int min_range();
public
Q_SLOTS:
  void set_range(int a_min, int a_max);
  void set_value(int a_value);
  void on_value_changed(const QVariant &a_value);
Q_SIGNALS:
  void contentBoundingRectChaned();

protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void paint(QPainter *a_painter_ptr,
                     const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget *a_widget_ptr = 0);

private:
  class PrivateProgressBar;
  PrivateProgressBar *const o_progress_bar;
};
}

#endif // PROGRESS_BAR_H
