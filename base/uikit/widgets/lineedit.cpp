#include "lineedit.h"

#include <style.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include <style.h>
#include <themepackloader.h>

namespace PlexyDesk {

class LineEdit::PrivateLineEdit {
public:
  typedef enum { NORMAL, FOCUSED, HOVER } EditState;

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
};

LineEdit::LineEdit(QGraphicsObject *parent)
    : UIWidget(parent), d(new PrivateLineEdit) {
  d->mState = PrivateLineEdit::NORMAL;
  d->mStyle = Theme::instance()->defaultDesktopStyle();

  setSize(
      QSize(Theme::style()->attrbute("widget", "line_edit_width").toInt(),
            Theme::style()->attrbute("widget", "line_edit_height").toInt()));

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setAcceptHoverEvents(true);
  d->mKeyCursorLeftLoc = 0;
}

LineEdit::~LineEdit() { delete d; }

void LineEdit::setText(const QString &txt) { d->mText = txt; }

QString LineEdit::text() const { return d->mText; }

void LineEdit::style(StylePtr style) { d->mStyle = style; }

void LineEdit::setSize(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF LineEdit::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return boundingRect().size();
}

void LineEdit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget * /*widget*/) {
  switch (d->mState) {
    case PrivateLineEdit::NORMAL:
      d->paintNormalEdit(painter, option);
      break;
    case PrivateLineEdit::FOCUSED:
      d->paintFocusedEdit(painter, option);
      break;
    default:
      qDebug() << Q_FUNC_INFO << "Unknown State";
  }
  QFontMetrics m = painter->fontMetrics();
  int _text_pixel_width = m.width(d->mText) + 1;

  int _text_cursor_width = 10;

  if (d->mText.count() == d->mKeyCursorLeftLoc) {
    _text_cursor_width += _text_pixel_width;
  } else {
    _text_cursor_width += m.width(d->mText.left(d->mKeyCursorLeftLoc));
  }

  QPointF line1(_text_cursor_width, 10);
  QPointF line2(_text_cursor_width, option->exposedRect.height() - 10);

  QPen pen =
      QPen(QColor(98, 101, 108), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(line1, line2);
}

void LineEdit::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QGraphicsObject::mousePressEvent(event);
  event->accept();
  d->mState = PrivateLineEdit::FOCUSED;
  update();
}

void LineEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  qDebug() << Q_FUNC_INFO << event->pos();
  QGraphicsObject::mouseReleaseEvent(event);
}

void LineEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  setCursor(Qt::IBeamCursor);
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  QGraphicsObject::mouseMoveEvent(event);
}

void LineEdit::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  setCursor(Qt::IBeamCursor);
  grabKeyboard();
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  QGraphicsItem::hoverEnterEvent(event);
}

void LineEdit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  ungrabKeyboard();
  d->mState = PrivateLineEdit::NORMAL;
  setCursor(Qt::ArrowCursor);
  update();
  QGraphicsObject::hoverLeaveEvent(event);
}

void LineEdit::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  setCursor(Qt::IBeamCursor);
  d->mState = PrivateLineEdit::FOCUSED;
  update();
  QGraphicsObject::hoverMoveEvent(event);
}

bool LineEdit::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Enter) {
      // Special tab handling
      return true;
    } else
      return false;
  }
  return false;
}

void LineEdit::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Backspace) {
    d->mText.remove(d->mKeyCursorLeftLoc - 1, 1);
    if (d->mKeyCursorLeftLoc != 0)
      d->mKeyCursorLeftLoc--;
    Q_EMIT text(d->mText);
    update();
    return;
  } else if (event->key() == Qt::Key_Left) {
    if (d->mKeyCursorLeftLoc != 0)
      d->mKeyCursorLeftLoc--;
    update();
    return;

  } else if (event->key() == Qt::Key_Right) {
    if (d->mKeyCursorLeftLoc < d->mText.count())
      d->mKeyCursorLeftLoc++;
    update();
    return;
  } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    Q_EMIT submit();
    return;
  }
  d->mText.insert(d->mKeyCursorLeftLoc, event->text());
  d->mKeyCursorLeftLoc++;
  Q_EMIT text(d->mText);
  update();
}

void LineEdit::PrivateLineEdit::paintNormalEdit(
    QPainter *painter, const QStyleOptionGraphicsItem *option) {
  StyleFeatures feature;
  feature.geometry = option->exposedRect;
  feature.render_state = StyleFeatures::kRenderElement;
  feature.text_data = mText;

  painter->save();
  if (PlexyDesk::Theme::style()) {
    PlexyDesk::Theme::style()->draw("line_edit", feature, painter);
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
  if (PlexyDesk::Theme::style()) {
    PlexyDesk::Theme::style()->draw("line_edit", feature, painter);
  }
  painter->restore();
}
}
