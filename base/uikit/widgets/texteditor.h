#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <plexy.h>
#include <widget.h>

#include <plexydesk_ui_exports.h>

namespace UIKit {
class DECL_UI_KIT_EXPORT TextEditor : public Widget {
  Q_OBJECT
public:
  TextEditor(Widget *a_parent_ptr = 0);
  virtual ~TextEditor();

  virtual void set_text(const QString &a_text);
  virtual void set_placeholder_text(const QString &a_placeholderText);

  virtual void set_font_point_size(qreal a_s);

  virtual QString text() const;

  virtual void style(const QString &a_style);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;
  virtual void setGeometry(const QRectF &a_rect);

  virtual void set_text_scale_factor(qreal a_scale_factor);
  virtual qreal text_scale_factor() const;
  virtual void update_text_scale();

  virtual void begin_list();
  virtual void end_list();

  virtual void convert_to_link();
public
Q_SLOTS:
  void on_text_updated();
  void on_block_count_changed(int a_count);
Q_SIGNALS:
  void text_updated(const QString &a_text);
  void document_title_available(const QString &a_title);

private:
  class PrivateTextEditor;
  PrivateTextEditor *const d;
};
}

#endif // TEXTEDITOR_H
