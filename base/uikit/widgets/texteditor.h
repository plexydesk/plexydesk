#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <plexy.h>
#include <widget.h>
#include <plexydesk_ui_exports.h>

namespace UIKit
{

class DECL_UI_KIT_EXPORT TextEditor : public UIKit::Widget
{
  Q_OBJECT
public:
  TextEditor(QGraphicsObject *a_parent_ptr = 0);

  ~TextEditor();

  virtual void setText(const QString &text);

  virtual void setPlaceholderText(const QString &placeholderText);

  virtual void setFontPointSize(qreal s);

  virtual QString text() const;

  virtual void style(const QString &style);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

  virtual void setGeometry(const QRectF &rect);

  virtual void setTextScaleFactor(qreal scaleFactor);

  virtual qreal textScaleFactor() const;

  virtual void updateTextScale();

  virtual void beginList();

  virtual void endList();

  virtual void convertToLink();

public Q_SLOTS:
  void onTextUpdated();
  void onBlockCountChanged(int count);

Q_SIGNALS:
  void textUpdated(const QString &text);
  void documentTitleAvailable(const QString &title);

private:
  class PrivateTextEditor;
  PrivateTextEditor *const d;
};
}

#endif // TEXTEDITOR_H
