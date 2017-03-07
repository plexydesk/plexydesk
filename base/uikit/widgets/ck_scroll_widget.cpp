/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>

#ifdef __QT5_TOOLKIT__
#include <QScroller>
#endif

#include <QGraphicsSceneWheelEvent>

#include "ck_scroll_widget.h"

namespace cherry_kit {

class scroll_widget::Private {

public:
  Private() {}
  ~Private() {}

  QSizeF m_size;
  QGraphicsObject *m_viewport;
};

scroll_widget::scroll_widget(const QRectF &a_rect, widget *parent)
    : widget(parent), o_scroll_widget(new Private) {
  o_scroll_widget->m_viewport = 0;
#ifdef __QT5_TOOLKIT__
  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
#endif
  o_scroll_widget->m_size = a_rect.size();
}

scroll_widget::~scroll_widget() { delete o_scroll_widget; }

void scroll_widget::set_viewport(QGraphicsObject *a_widget_ptr) {
  if (!a_widget_ptr) {
    return;
  }

  if (a_widget_ptr == o_scroll_widget->m_viewport) {
    return;
  }

  if (o_scroll_widget->m_viewport != 0) {
    o_scroll_widget->m_viewport->setParentItem(0);
    delete o_scroll_widget->m_viewport;
  }

  if (a_widget_ptr) {
    a_widget_ptr->setParentItem(this);
    o_scroll_widget->m_viewport = a_widget_ptr;
  }
}

void scroll_widget::scroll_by(int x, int y) {
  if (o_scroll_widget->m_viewport) {
    // resetric to viewport
    int y_pos = o_scroll_widget->m_viewport->y() + y;
    int view_height = this->boundingRect().height();
    int y_max = (-1) * (o_scroll_widget->m_viewport->boundingRect().height() -
                        view_height);

    if (y_pos < (this->y()) && y_pos > y_max) {
      o_scroll_widget->m_viewport->setPos(o_scroll_widget->m_viewport->x() + x,
                                          o_scroll_widget->m_viewport->y() + y);
    } else if (!(y_pos < (this->y()))) {
      o_scroll_widget->m_viewport->setPos(o_scroll_widget->m_viewport->x() + x,
                                          this->y());
    }
  }
}

void scroll_widget::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {}

void scroll_widget::wheelEvent(QGraphicsSceneWheelEvent *a_event_ptr) {
  if (a_event_ptr->delta() < 0) {
    scroll_by(0, -10);
  } else {
    scroll_by(0, 10);
  }
  a_event_ptr->accept();

  //?
  widget::wheelEvent(a_event_ptr);
}

void scroll_widget::dragMoveEvent(QGraphicsSceneDragDropEvent *a_event_ptr) {
  widget::dragMoveEvent(a_event_ptr);
}

bool scroll_widget::event(QEvent *a_event_ptr) {
#ifdef __QT5_TOOLKIT__
  if (!o_scroll_widget->m_viewport) {
    return QGraphicsObject::event(a_event_ptr);
  }

  switch (a_event_ptr->type()) {
  case QScrollPrepareEvent::ScrollPrepare: {
    qDebug() << Q_FUNC_INFO << "Prepare :";
    QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(a_event_ptr);
    se->setViewportSize(o_scroll_widget->m_size);
    QRectF br = o_scroll_widget->m_viewport->boundingRect();
    se->setContentPosRange(QRectF(
        0, 0, qMax(qreal(0), br.width() - o_scroll_widget->m_size.width()),
        qMax(qreal(0), br.height() - o_scroll_widget->m_size.height())));
    se->setContentPos(-o_scroll_widget->m_viewport->pos());
    se->accept();
    return QGraphicsObject::event(a_event_ptr);
  }
  case QScrollEvent::Scroll: {
    qDebug() << Q_FUNC_INFO << "Scroll";
    QScrollEvent *se = static_cast<QScrollEvent *>(a_event_ptr);
    o_scroll_widget->m_viewport->setPos(-se->contentPos() -
                                        se->overshootDistance());
    return true;
  }

  default:
    break;
  }
#endif
  return QGraphicsObject::event(a_event_ptr);
}

bool scroll_widget::sceneEvent(QEvent *a_e_ptr) {
  /*
  switch (e->type()) {
  case QEvent::TouchBegin: {
      // We need to return true for the TouchBegin here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set due to our setAcceptTouchEvents(true) call in the c'tor
      return true;

  }
  case QEvent::GraphicsSceneMousePress: {
      // We need to return true for the MousePress here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set to true by Qt)
      return true;

  }
  default:
      break;
  }
  */
  return QGraphicsObject::sceneEvent(a_e_ptr);
}

} // namespace PlexyDesk
