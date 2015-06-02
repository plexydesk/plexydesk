#include "lineedit.h"

#include <style.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include <style.h>
#include <themepackloader.h>

namespace UIKit {

class LineEdit::PrivateLineEdit {
public:
  typedef enum {
    NORMAL,
    FOCUSED,
    SELECTED,
    HOVER
  } EditState;

  PrivateLineEdit() {}

  ~PrivateLineEdit() {}

  void paintNormalEdit(QPainter *painter,
                       const QStyleOptionGraphicsItem *option);

  void paintFocusedEdit(QPainter *painter,
                        const QStyleOptionGraphicsItem *option);

  EditState mState;
  StylePtr mStyle;
  QString m_editor_text;

  int mLastKey;
  int m_text_cursor;
  int m_text_selection_cursor;
  bool m_text_selection_mode;

  QList<std::function<void(const QString &)> > m_text_handler_list;
};

LineEdit::LineEdit(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateLineEdit) {
  d->mState = PrivateLineEdit::NORMAL;
  d->m_text_selection_mode = false;
  d->mStyle = Theme::style();
  d->m_text_cursor = 0;
  d->m_text_selection_cursor = 0;

  set_size(
      QSize(Theme::style()->attribute("widget", "line_edit_width").toInt(),
            Theme::style()->attribute("widget", "line_edit_height").toInt()));

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setAcceptHoverEvents(true);
}

LineEdit::~LineEdit() { delete d; }

void LineEdit::set_text(const QString &a_txt) { d->m_editor_text = a_txt; }

QString LineEdit::text() const { return d->m_editor_text; }

void LineEdit::style(StylePtr a_style) { d->mStyle = a_style; }

void LineEdit::set_size(const QSizeF &a_size) {
  setGeometry(QRectF(0, 0, a_size.width(), a_size.height()));
}

QSizeF LineEdit::sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint) const {
  return boundingRect().size();
}

void LineEdit::on_insert(std::function<void(const QString &)> a_handler) {
  d->m_text_handler_list.append(a_handler);
}

QString LineEdit::current_text_selection() const {
  QString rv;

  if (!d->m_text_selection_mode)
    return rv;

  if (d->m_text_selection_cursor > d->m_text_cursor) {
    int start = d->m_text_cursor;
    int length = d->m_text_selection_cursor - d->m_text_cursor;
    rv = d->m_editor_text.mid(start, length);
  } else if (d->m_text_selection_cursor < d->m_text_cursor) {
    int start = d->m_text_selection_cursor;
    int length = d->m_text_cursor - d->m_text_selection_cursor;
    rv = d->m_editor_text.mid(start, length);
  }

  return rv;
}

QRectF LineEdit::current_text_rect(QPainter *a_painter) const {
  int text_cursor_x = 10;
  int selection_cursor_x = 10;

  QFontMetrics m = a_painter->fontMetrics();
  int text_pixel_width = m.width(d->m_editor_text);

  if (d->m_text_cursor == d->m_editor_text.count()) {
    text_cursor_x += text_pixel_width;
  } else {
    text_cursor_x += m.width(d->m_editor_text.left(d->m_text_cursor));
  }

  if (d->m_text_selection_cursor == d->m_editor_text.count()) {
    selection_cursor_x += text_pixel_width;
  } else {
    selection_cursor_x +=
        m.width(d->m_editor_text.left(d->m_text_selection_cursor));
  }

  int diff = text_cursor_x - selection_cursor_x;

  return QRectF(selection_cursor_x, 0, diff, geometry().height());
}

void LineEdit::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  StyleFeatures feature;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.text_data = d->m_editor_text;

  feature.attributes["cursor_location"] = d->m_text_cursor;
  feature.attributes["selection_cursor"] = d->m_text_selection_cursor;

  switch (d->mState) {
  case PrivateLineEdit::NORMAL:
    feature.render_state = StyleFeatures::kRenderElement;
    break;
  case PrivateLineEdit::FOCUSED:
    feature.render_state = StyleFeatures::kRenderRaised;
    break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown State";
  }

  if (d->m_text_selection_mode) {
    feature.render_state = StyleFeatures::kRenderPressed;
  } else {
    feature.render_state = StyleFeatures::kRenderRaised;
  }

  a_painter_ptr->save();
  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("line_edit", feature, a_painter_ptr);
  }
  a_painter_ptr->restore();
}

void LineEdit::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  a_event_ptr->accept();
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  Widget::mousePressEvent(a_event_ptr);
}

void LineEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Widget::mouseReleaseEvent(a_event_ptr);
}

void LineEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  Widget::mouseMoveEvent(a_event_ptr);
}

void LineEdit::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  grabKeyboard();
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  Widget::hoverEnterEvent(a_event_ptr);
}

void LineEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ungrabKeyboard();
  d->mState = PrivateLineEdit::NORMAL;
  setCursor(Qt::ArrowCursor);
  update();
  Widget::hoverLeaveEvent(event);
}

void LineEdit::hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  Widget::hoverMoveEvent(a_event_ptr);
}

bool LineEdit::eventFilter(QObject *object, QEvent *a_event_ptr) {
  if (a_event_ptr->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(a_event_ptr);
    if (keyEvent->key() == Qt::Key_Enter) {
      // Special tab handling
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void LineEdit::keyPressEvent(QKeyEvent *a_event_ptr) {
  if (a_event_ptr->key() == Qt::Key_Shift) {
    Widget::keyPressEvent(a_event_ptr);
    d->m_text_selection_mode = true;
    d->m_text_selection_cursor = d->m_text_cursor;
    return;
  }

  if (a_event_ptr->key() == Qt::Key_Backspace) {
    d->m_editor_text.remove(d->m_text_cursor - 1, 1);
    if (d->m_text_cursor != 0) {
      d->m_text_cursor--;
    }
    Q_EMIT text(d->m_editor_text);

    foreach(std::function<void(const QString &)> _func,
            d->m_text_handler_list) {
      if (_func) {
        _func(d->m_editor_text);
      }
    }

    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Left) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      d->m_text_selection_mode = true;
      if (d->m_text_selection_cursor > 0)
        d->m_text_selection_cursor--;
    } else {
      d->m_text_selection_mode = false;

      if (d->m_text_cursor > 0) {
        d->m_text_cursor--;
        d->m_text_selection_cursor = d->m_text_cursor;
      }
    }
    update();
    return;

  } else if (a_event_ptr->key() == Qt::Key_Right) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      d->m_text_selection_mode = true;
      if (d->m_text_selection_cursor < d->m_editor_text.count())
        d->m_text_selection_cursor++;
    } else {
      d->m_text_selection_mode = false;

      if (d->m_text_cursor < d->m_editor_text.count()) {
        d->m_text_cursor++;
        d->m_text_selection_cursor = d->m_text_cursor;
      }
    }
    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Enter ||
             a_event_ptr->key() == Qt::Key_Return) {
    Q_EMIT submit();
    return;
  }
  d->m_editor_text.insert(d->m_text_cursor, a_event_ptr->text());

  d->m_text_cursor++;

  Q_EMIT text(d->m_editor_text);

  foreach(std::function<void(const QString &)> _func, d->m_text_handler_list) {
    if (_func) {
      _func(d->m_editor_text);
    }
  }
  update();
}

void LineEdit::PrivateLineEdit::paintNormalEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  StyleFeatures feature;
  feature.geometry = option->exposedRect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.text_data = m_editor_text;

  painter->save();
  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}

void LineEdit::PrivateLineEdit::paintFocusedEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  StyleFeatures feature;
  feature.geometry = option->exposedRect;

  if (m_text_selection_mode) {
    feature.render_state = StyleFeatures::kRenderPressed;
  } else {
    feature.render_state = StyleFeatures::kRenderRaised;
  }

  feature.text_data = m_editor_text;

  painter->save();
  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}
}
