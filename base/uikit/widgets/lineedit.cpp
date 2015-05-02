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
  QString mText;
  int mLastKey;
  int mKeyCursorLeftLoc;
  QList<std::function<void(const QString &)> > m_text_handler_list;
};

LineEdit::LineEdit(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateLineEdit) {
  d->mState = PrivateLineEdit::NORMAL;
  d->mStyle = Theme::style();

  set_size(
      QSize(Theme::style()->attribute("widget", "line_edit_width").toInt(),
            Theme::style()->attribute("widget", "line_edit_height").toInt()));

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setAcceptHoverEvents(true);
  d->mKeyCursorLeftLoc = 0;
}

LineEdit::~LineEdit() { delete d; }

void LineEdit::set_text(const QString &a_txt) { d->mText = a_txt; }

QString LineEdit::text() const { return d->mText; }

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

void LineEdit::paint(QPainter *a_painter_ptr,
                     const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget * /*widget*/) {
  switch (d->mState) {
    case PrivateLineEdit::NORMAL:
      d->paintNormalEdit(a_painter_ptr, a_option_ptr);
      break;
    case PrivateLineEdit::FOCUSED:
      d->paintFocusedEdit(a_painter_ptr, a_option_ptr);
      break;
    default:
      qDebug() << Q_FUNC_INFO << "Unknown State";
  }
  QFontMetrics m = a_painter_ptr->fontMetrics();
  int _text_pixel_width = m.width(d->mText) + 1;

  int _text_cursor_width = 10;

  if (d->mText.count() == d->mKeyCursorLeftLoc) {
    _text_cursor_width += _text_pixel_width;
  } else {
    _text_cursor_width += m.width(d->mText.left(d->mKeyCursorLeftLoc));
  }

  QPointF line1(_text_cursor_width, 10);
  QPointF line2(_text_cursor_width, a_option_ptr->exposedRect.height() - 10);

  QPen pen =
      QPen(QColor(98, 101, 108), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  a_painter_ptr->setPen(pen);
  a_painter_ptr->drawLine(line1, line2);
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
  if (a_event_ptr->key() == Qt::Key_Backspace) {
    d->mText.remove(d->mKeyCursorLeftLoc - 1, 1);
    if (d->mKeyCursorLeftLoc != 0) {
      d->mKeyCursorLeftLoc--;
    }
    Q_EMIT text(d->mText);

    foreach(std::function<void(const QString &)> _func,
            d->m_text_handler_list) {
      if (_func) {
        _func(d->mText);
      }
    }

    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Left) {
    if (d->mKeyCursorLeftLoc != 0) {
      d->mKeyCursorLeftLoc--;
    }
    update();
    return;

  } else if (a_event_ptr->key() == Qt::Key_Right) {
    if (d->mKeyCursorLeftLoc < d->mText.count()) {
      d->mKeyCursorLeftLoc++;
    }
    update();
    return;
  } else if (a_event_ptr->key() == Qt::Key_Enter ||
             a_event_ptr->key() == Qt::Key_Return) {
    Q_EMIT submit();
    return;
  }
  d->mText.insert(d->mKeyCursorLeftLoc, a_event_ptr->text());
  d->mKeyCursorLeftLoc++;

  Q_EMIT text(d->mText);

  foreach(std::function<void(const QString &)> _func, d->m_text_handler_list) {
    if (_func) {
      _func(d->mText);
    }
  }
  update();
}

void LineEdit::PrivateLineEdit::paintNormalEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  StyleFeatures feature;
  feature.geometry = option->exposedRect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.text_data = mText;

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
  feature.render_state = StyleFeatures::kRenderRaised;
  feature.text_data = mText;

  painter->save();
  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}
}
