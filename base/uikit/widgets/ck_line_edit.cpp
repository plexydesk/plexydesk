#include "ck_line_edit.h"

#include <ck_style.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include <ck_style.h>
#include <ck_resource_manager.h>

namespace cherry_kit {

class line_edit::PrivateLineEdit {
public:
  typedef enum {
    NORMAL,
    FOCUSED,
    SELECTED,
    HOVER
  } EditState;

  PrivateLineEdit() : m_is_readonly(0) {}

  ~PrivateLineEdit() {}

  void paintNormalEdit(QPainter *painter,
                       const QStyleOptionGraphicsItem *option);

  void paintFocusedEdit(QPainter *painter,
                        const QStyleOptionGraphicsItem *option);

  EditState mState;
  style_ref mStyle;
  QString m_editor_text;

  int mLastKey;
  int m_text_cursor;
  int m_text_selection_cursor;
  bool m_text_selection_mode;

  QList<std::function<void(const QString &)> > m_text_handler_list;

  bool m_is_readonly;
};

line_edit::line_edit(widget *parent)
    : widget(parent), priv(new PrivateLineEdit) {
  priv->mState = PrivateLineEdit::NORMAL;
  priv->m_text_selection_mode = false;
  priv->mStyle = resource_manager::style();
  priv->m_text_cursor = 0;
  priv->m_text_selection_cursor = 0;

  set_size(QSize(
      resource_manager::style()->attribute("widget", "line_edit_width").toInt(),
      resource_manager::style()
          ->attribute("widget", "line_edit_height")
          .toInt()));

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setAcceptHoverEvents(true);
}

line_edit::~line_edit() { delete priv; }

void line_edit::set_text(const QString &a_txt) {
  priv->m_editor_text = a_txt;
  update();
}

QString line_edit::text() const { return priv->m_editor_text; }

void line_edit::style(style_ref a_style) { priv->mStyle = a_style; }

void line_edit::set_size(const QSizeF &a_size) {
  set_geometry(QRectF(0, 0, a_size.width(), a_size.height()));
}

QSizeF line_edit::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  return contents_geometry().size();
}

void line_edit::on_insert(std::function<void(const QString &)> a_handler) {
  priv->m_text_handler_list.append(a_handler);
}

QString line_edit::current_text_selection() const {
  QString rv;

  if (!priv->m_text_selection_mode)
    return rv;

  if (priv->m_text_selection_cursor > priv->m_text_cursor) {
    int start = priv->m_text_cursor;
    int length = priv->m_text_selection_cursor - priv->m_text_cursor;
    rv = priv->m_editor_text.mid(start, length);
  } else if (priv->m_text_selection_cursor < priv->m_text_cursor) {
    int start = priv->m_text_selection_cursor;
    int length = priv->m_text_cursor - priv->m_text_selection_cursor;
    rv = priv->m_editor_text.mid(start, length);
  }

  return rv;
}

bool line_edit::readonly() { return priv->m_is_readonly; }

void line_edit::set_readonly(bool a_value) { priv->m_is_readonly = a_value; }

QRectF line_edit::current_text_rect(QPainter *a_painter) const {
  int text_cursor_x = 10;
  int selection_cursor_x = 10;

  QFontMetrics m = a_painter->fontMetrics();
  int text_pixel_width = m.width(priv->m_editor_text);

  if (priv->m_text_cursor == priv->m_editor_text.count()) {
    text_cursor_x += text_pixel_width;
  } else {
    text_cursor_x += m.width(priv->m_editor_text.left(priv->m_text_cursor));
  }

  if (priv->m_text_selection_cursor == priv->m_editor_text.count()) {
    selection_cursor_x += text_pixel_width;
  } else {
    selection_cursor_x +=
        m.width(priv->m_editor_text.left(priv->m_text_selection_cursor));
  }

  int diff = text_cursor_x - selection_cursor_x;

  return QRectF(selection_cursor_x, 0, diff, geometry().height());
}

void line_edit::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  style_data feature;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderElement;
  feature.text_data = priv->m_editor_text;

  feature.attributes["cursor_location"] = priv->m_text_cursor;
  feature.attributes["selection_cursor"] = priv->m_text_selection_cursor;
  feature.style_object = this;

  switch (priv->mState) {
  case PrivateLineEdit::NORMAL:
    feature.render_state = style_data::kRenderElement;
    break;
  case PrivateLineEdit::FOCUSED:
    feature.render_state = style_data::kRenderRaised;
    break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown State";
  }

  if (priv->m_text_selection_mode) {
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

void line_edit::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  a_event_ptr->accept();
  if (!priv->m_is_readonly) {
    priv->mState = PrivateLineEdit::FOCUSED;
    update();
  }

  widget::mousePressEvent(a_event_ptr);
}

void line_edit::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  widget::mouseReleaseEvent(a_event_ptr);
}

void line_edit::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  if (!priv->m_is_readonly) {
    setCursor(Qt::IBeamCursor);
    priv->mState = PrivateLineEdit::FOCUSED;
    update();
  }
  widget::mouseMoveEvent(a_event_ptr);
}

void line_edit::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  if (!priv->m_is_readonly) {
    setCursor(Qt::IBeamCursor);
    grabKeyboard();
    priv->mState = PrivateLineEdit::FOCUSED;
    update();
  }

  widget::hoverEnterEvent(a_event_ptr);
}

void line_edit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  if (!priv->m_is_readonly) {
    ungrabKeyboard();
    priv->mState = PrivateLineEdit::NORMAL;
    setCursor(Qt::ArrowCursor);
    update();
  }
  widget::hoverLeaveEvent(event);
}

void line_edit::hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  if (!priv->m_is_readonly) {
    setCursor(Qt::IBeamCursor);
    priv->mState = PrivateLineEdit::FOCUSED;
    update();
  }
  widget::hoverMoveEvent(a_event_ptr);
}

bool line_edit::eventFilter(QObject *object, QEvent *a_event_ptr) {
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

void line_edit::keyPressEvent(QKeyEvent *a_event_ptr) {
  if (priv->m_is_readonly)
    return;

  if (a_event_ptr->key() == Qt::Key_Shift) {
    widget::keyPressEvent(a_event_ptr);
    priv->m_text_selection_mode = true;
    priv->m_text_selection_cursor = priv->m_text_cursor;
    return;
  }

  if (a_event_ptr->key() == Qt::Key_Backspace) {
    priv->m_editor_text.remove(priv->m_text_cursor - 1, 1);
    if (priv->m_text_cursor != 0) {
      priv->m_text_cursor--;
    }
    Q_EMIT text(priv->m_editor_text);

    foreach(std::function<void(const QString &)> _func,
            priv->m_text_handler_list) {
      if (_func) {
        _func(priv->m_editor_text);
      }
    }

    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Left) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      priv->m_text_selection_mode = true;
      if (priv->m_text_selection_cursor > 0)
        priv->m_text_selection_cursor--;
    } else {
      priv->m_text_selection_mode = false;

      if (priv->m_text_cursor > 0) {
        priv->m_text_cursor--;
        priv->m_text_selection_cursor = priv->m_text_cursor;
      }
    }
    update();
    return;

  } else if (a_event_ptr->key() == Qt::Key_Right) {
    if (a_event_ptr->modifiers() & Qt::ShiftModifier) {
      priv->m_text_selection_mode = true;
      if (priv->m_text_selection_cursor < priv->m_editor_text.count())
        priv->m_text_selection_cursor++;
    } else {
      priv->m_text_selection_mode = false;

      if (priv->m_text_cursor < priv->m_editor_text.count()) {
        priv->m_text_cursor++;
        priv->m_text_selection_cursor = priv->m_text_cursor;
      }
    }
    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Enter ||
             a_event_ptr->key() == Qt::Key_Return) {
    Q_EMIT submit();
    return;
  }
  priv->m_editor_text.insert(priv->m_text_cursor, a_event_ptr->text());

  priv->m_text_cursor++;

  Q_EMIT text(priv->m_editor_text);

  foreach(std::function<void(const QString &)> _func,
          priv->m_text_handler_list) {
    if (_func) {
      _func(priv->m_editor_text);
    }
  }
  update();
}

void line_edit::PrivateLineEdit::paintNormalEdit(
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

void line_edit::PrivateLineEdit::paintFocusedEdit(
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
