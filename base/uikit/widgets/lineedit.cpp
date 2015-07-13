#include "lineedit.h"

#include <style.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include <style.h>
#include <resource_manager.h>

namespace cherry_kit {

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

LineEdit::LineEdit(widget *parent)
    : widget(parent), o_line_edit(new PrivateLineEdit) {
  o_line_edit->mState = PrivateLineEdit::NORMAL;
  o_line_edit->m_text_selection_mode = false;
  o_line_edit->mStyle = resource_manager::style();
  o_line_edit->m_text_cursor = 0;
  o_line_edit->m_text_selection_cursor = 0;

  set_size(QSize(
      resource_manager::style()->attribute("widget", "line_edit_width").toInt(),
      resource_manager::style()
          ->attribute("widget", "line_edit_height")
          .toInt()));

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setAcceptHoverEvents(true);
}

LineEdit::~LineEdit() { delete o_line_edit; }

void LineEdit::set_text(const QString &a_txt) {
  o_line_edit->m_editor_text = a_txt;
}

QString LineEdit::text() const { return o_line_edit->m_editor_text; }

void LineEdit::style(StylePtr a_style) { o_line_edit->mStyle = a_style; }

void LineEdit::set_size(const QSizeF &a_size) {
  setGeometry(QRectF(0, 0, a_size.width(), a_size.height()));
}

QSizeF LineEdit::sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint) const {
  return boundingRect().size();
}

void LineEdit::on_insert(std::function<void(const QString &)> a_handler) {
  o_line_edit->m_text_handler_list.append(a_handler);
}

QString LineEdit::current_text_selection() const {
  QString rv;

  if (!o_line_edit->m_text_selection_mode)
    return rv;

  if (o_line_edit->m_text_selection_cursor > o_line_edit->m_text_cursor) {
    int start = o_line_edit->m_text_cursor;
    int length =
        o_line_edit->m_text_selection_cursor - o_line_edit->m_text_cursor;
    rv = o_line_edit->m_editor_text.mid(start, length);
  } else if (o_line_edit->m_text_selection_cursor <
             o_line_edit->m_text_cursor) {
    int start = o_line_edit->m_text_selection_cursor;
    int length =
        o_line_edit->m_text_cursor - o_line_edit->m_text_selection_cursor;
    rv = o_line_edit->m_editor_text.mid(start, length);
  }

  return rv;
}

QRectF LineEdit::current_text_rect(QPainter *a_painter) const {
  int text_cursor_x = 10;
  int selection_cursor_x = 10;

  QFontMetrics m = a_painter->fontMetrics();
  int text_pixel_width = m.width(o_line_edit->m_editor_text);

  if (o_line_edit->m_text_cursor == o_line_edit->m_editor_text.count()) {
    text_cursor_x += text_pixel_width;
  } else {
    text_cursor_x +=
        m.width(o_line_edit->m_editor_text.left(o_line_edit->m_text_cursor));
  }

  if (o_line_edit->m_text_selection_cursor ==
      o_line_edit->m_editor_text.count()) {
    selection_cursor_x += text_pixel_width;
  } else {
    selection_cursor_x += m.width(
        o_line_edit->m_editor_text.left(o_line_edit->m_text_selection_cursor));
  }

  int diff = text_cursor_x - selection_cursor_x;

  return QRectF(selection_cursor_x, 0, diff, geometry().height());
}

void LineEdit::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  style_data feature;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderElement;
  feature.text_data = o_line_edit->m_editor_text;

  feature.attributes["cursor_location"] = o_line_edit->m_text_cursor;
  feature.attributes["selection_cursor"] = o_line_edit->m_text_selection_cursor;

  switch (o_line_edit->mState) {
  case PrivateLineEdit::NORMAL:
    feature.render_state = style_data::kRenderElement;
    break;
  case PrivateLineEdit::FOCUSED:
    feature.render_state = style_data::kRenderRaised;
    break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown State";
  }

  if (o_line_edit->m_text_selection_mode) {
    feature.render_state = style_data::kRenderPressed;
  } else {
    feature.render_state = style_data::kRenderRaised;
  }

  a_painter_ptr->save();
  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("line_edit", feature,
                                                a_painter_ptr);
  }
  a_painter_ptr->restore();
}

void LineEdit::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  a_event_ptr->accept();
  o_line_edit->mState = PrivateLineEdit::FOCUSED;
  update();
  widget::mousePressEvent(a_event_ptr);
}

void LineEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  widget::mouseReleaseEvent(a_event_ptr);
}

void LineEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  o_line_edit->mState = PrivateLineEdit::FOCUSED;
  update();
  widget::mouseMoveEvent(a_event_ptr);
}

void LineEdit::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  grabKeyboard();
  o_line_edit->mState = PrivateLineEdit::FOCUSED;
  update();
  widget::hoverEnterEvent(a_event_ptr);
}

void LineEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ungrabKeyboard();
  o_line_edit->mState = PrivateLineEdit::NORMAL;
  setCursor(Qt::ArrowCursor);
  update();
  widget::hoverLeaveEvent(event);
}

void LineEdit::hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  setCursor(Qt::IBeamCursor);
  o_line_edit->mState = PrivateLineEdit::FOCUSED;
  update();
  widget::hoverMoveEvent(a_event_ptr);
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
    widget::keyPressEvent(a_event_ptr);
    o_line_edit->m_text_selection_mode = true;
    o_line_edit->m_text_selection_cursor = o_line_edit->m_text_cursor;
    return;
  }

  if (a_event_ptr->key() == Qt::Key_Backspace) {
    o_line_edit->m_editor_text.remove(o_line_edit->m_text_cursor - 1, 1);
    if (o_line_edit->m_text_cursor != 0) {
      o_line_edit->m_text_cursor--;
    }
    Q_EMIT text(o_line_edit->m_editor_text);

    foreach(std::function<void(const QString &)> _func,
            o_line_edit->m_text_handler_list) {
      if (_func) {
        _func(o_line_edit->m_editor_text);
      }
    }

    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Left) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      o_line_edit->m_text_selection_mode = true;
      if (o_line_edit->m_text_selection_cursor > 0)
        o_line_edit->m_text_selection_cursor--;
    } else {
      o_line_edit->m_text_selection_mode = false;

      if (o_line_edit->m_text_cursor > 0) {
        o_line_edit->m_text_cursor--;
        o_line_edit->m_text_selection_cursor = o_line_edit->m_text_cursor;
      }
    }
    update();
    return;

  } else if (a_event_ptr->key() == Qt::Key_Right) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      o_line_edit->m_text_selection_mode = true;
      if (o_line_edit->m_text_selection_cursor <
          o_line_edit->m_editor_text.count())
        o_line_edit->m_text_selection_cursor++;
    } else {
      o_line_edit->m_text_selection_mode = false;

      if (o_line_edit->m_text_cursor < o_line_edit->m_editor_text.count()) {
        o_line_edit->m_text_cursor++;
        o_line_edit->m_text_selection_cursor = o_line_edit->m_text_cursor;
      }
    }
    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Enter ||
             a_event_ptr->key() == Qt::Key_Return) {
    Q_EMIT submit();
    return;
  }
  o_line_edit->m_editor_text.insert(o_line_edit->m_text_cursor,
                                    a_event_ptr->text());

  o_line_edit->m_text_cursor++;

  Q_EMIT text(o_line_edit->m_editor_text);

  foreach(std::function<void(const QString &)> _func,
          o_line_edit->m_text_handler_list) {
    if (_func) {
      _func(o_line_edit->m_editor_text);
    }
  }
  update();
}

void LineEdit::PrivateLineEdit::paintNormalEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  style_data feature;
  feature.geometry = option->exposedRect;
  feature.render_state = style_data::kRenderElement;
  feature.text_data = m_editor_text;

  painter->save();
  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}

void LineEdit::PrivateLineEdit::paintFocusedEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  style_data feature;
  feature.geometry = option->exposedRect;

  if (m_text_selection_mode) {
    feature.render_state = style_data::kRenderPressed;
  } else {
    feature.render_state = style_data::kRenderRaised;
  }

  feature.text_data = m_editor_text;

  painter->save();
  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}
}
