/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : Siraj Razick <siraj@plexydesk.org>
*                 PhobosK <phobosk@kbfx.net>
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include "tableview.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QScroller>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsScene>
#include <QDebug>
#include <themepackloader.h>

namespace UIKit {

class TableView::PrivateTableView {

public:
  PrivateTableView() {}

  ~PrivateTableView() {
    if (m_base_list_layout) {
      delete m_table_delegate_ptr;
    }

    if (m_table_viewport) {
      delete m_table_viewport;
    }
  }

public:
  int m_current_item_count;
  QRectF m_table_view_geometry;
  QGraphicsWidget *m_table_viewport;
  TableModel *m_table_delegate_ptr;
  QGraphicsLinearLayout *m_base_list_layout;
  QGraphicsGridLayout *m_base_grid_layout;
  QList<TableViewItem *> m_current_table_view_item_list;

  std::function<void(TableViewItem *item)> mItemActivationCallback;
};

TableView::TableView(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateTableView) {
  d->m_table_delegate_ptr = 0;
  d->m_current_item_count = -1;

  d->m_table_viewport = new QGraphicsWidget(this);
  d->m_table_viewport->setFiltersChildEvents(false);

  d->m_table_viewport->setGeometry(QRectF(0, 0, 0, 0));
  d->m_table_view_geometry = QRectF(0.0, 0.0, 0, 0);
  d->m_table_viewport->setPos(QPointF());

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  setFiltersChildEvents(false);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);

  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
}

TableView::~TableView() {
  QScroller::ungrabGesture(this);
  delete d;
}

void TableView::set_model(TableModel *a_model_ptr) {
  if (d->m_table_delegate_ptr) {
    delete d->m_table_delegate_ptr;
  }

  if (!a_model_ptr) {
    return;
  }

  d->m_table_delegate_ptr = a_model_ptr;

  a_model_ptr->setParent(this);

  if (a_model_ptr->render_type() == TableModel::kRenderAsListView) {
    d->m_base_list_layout = new QGraphicsLinearLayout(d->m_table_viewport);
    d->m_base_list_layout->setOrientation(Qt::Vertical);
    d->m_base_list_layout->setContentsMargins(0, 0, 0, 0);
    d->m_base_list_layout->setSpacing(0.0);
  } else {
    d->m_base_grid_layout = new QGraphicsGridLayout(d->m_table_viewport);
    d->m_base_grid_layout->setSpacing(d->m_table_delegate_ptr->padding());
    d->m_base_grid_layout->setContentsMargins(
        d->m_table_delegate_ptr->left_margin(), 0.0,
        d->m_table_delegate_ptr->right_margin(), 0.0);
  }

  connect(a_model_ptr, SIGNAL(cleared()), this, SLOT(on_clear()));
  connect(a_model_ptr, SIGNAL(add(UIKit::TableViewItem *)), this,
          SLOT(on_add_viewItem(UIKit::TableViewItem *)));

  if (a_model_ptr) {
    a_model_ptr->init();
  }
}

TableModel *TableView::model() { return d->m_table_delegate_ptr; }

void TableView::clear_selection() {
  if (!d->m_table_delegate_ptr) {
    return;
  }

  if (d->m_table_delegate_ptr->render_type() == TableModel::kRenderAsListView) {
    // todo
  } else if (d->m_table_delegate_ptr->render_type() ==
             TableModel::kRenderAsGridView) {
    for (int i = 0; i < d->m_base_grid_layout->rowCount(); i++) {
      for (int j = 0; j < d->m_base_grid_layout->columnCount(); j++) {
        TableViewItem *cell =
            (TableViewItem *)d->m_base_grid_layout->itemAt(i, j);
        if (cell) {
          cell->clear_selection();
        }
      }
    }
  }
}

QRectF TableView::boundingRect() const {
  QRectF rv;
  rv.setWidth(d->m_table_view_geometry.width());
  rv.setHeight(d->m_table_view_geometry.height());
  return rv;
}

void TableView::setGeometry(const QRectF &a_rect) {
  qDebug() << Q_FUNC_INFO << a_rect;
  d->m_table_viewport->setGeometry(a_rect);
  d->m_table_viewport->setPos(QPointF());
  d->m_table_view_geometry.setWidth(a_rect.width());
  d->m_table_view_geometry.setHeight(a_rect.height());
  setPos(a_rect.topLeft());
  Widget::setGeometry(a_rect);
}

QSizeF TableView::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  return d->m_table_view_geometry.size();
}

void TableView::scrollBy(int a_x, int a_y) {
  // todo: manual scrolling probably with scrollbars ?
}

void TableView::paint_view(QPainter *painter, const QRectF &a_exposeRect) {}

TableViewItem *TableView::itemAt(uint i) {
  if (i >= (uint)d->m_current_table_view_item_list.count()) {
    return 0;
  }

  return d->m_current_table_view_item_list.at(i);
}

StylePtr TableView::style() const { return Theme::style(); }

uint TableView::count() const {
  if (!d->m_table_delegate_ptr) {
    return 0;
  }

  if (d->m_table_delegate_ptr->render_type() == TableModel::kRenderAsGridView) {
    if (!d->m_base_grid_layout) {
      return 0;
    }

    return d->m_base_grid_layout->count();
  } else if (d->m_table_delegate_ptr->render_type() ==
             TableModel::kRenderAsListView) {
    if (!d->m_base_list_layout) {
      return 0;
    }

    return d->m_base_list_layout->count();
  }

  return 0;
}

void TableView::set_item_activation_callback(
    std::function<void(TableViewItem *)> a_callback) {
  d->mItemActivationCallback = a_callback;
}

void TableView::on_item_click(TableViewItem *a_component_ptr) {
  clear_selection();

  if (a_component_ptr) {
    a_component_ptr->set_selected();
  }

  // todo: Remove this depricated method.
  Q_EMIT activated(a_component_ptr);

  if (d->mItemActivationCallback) {
    d->mItemActivationCallback(a_component_ptr);
  }

  qDebug() << Q_FUNC_INFO << " Activated :";
}

void TableView::on_add_viewItem(UIKit::TableViewItem *a_item_ptr) {
  if (!d->m_table_delegate_ptr) {
    return;
  }

  d->m_current_item_count++;

  if (d->m_table_delegate_ptr->render_type() == TableModel::kRenderAsListView) {
    a_item_ptr->setParentItem(d->m_table_viewport);
    a_item_ptr->setParent(this);
    d->m_base_list_layout->addItem(a_item_ptr);
    d->m_base_list_layout->setAlignment(a_item_ptr, Qt::AlignHCenter);
    connect(a_item_ptr, SIGNAL(clicked(TableViewItem *)), this,
            SLOT(on_item_click(TableViewItem *)));
    d->m_base_list_layout->activate();
    d->m_base_list_layout->updateGeometry();
  }

  if (d->m_table_delegate_ptr->render_type() == TableModel::kRenderAsGridView) {
    a_item_ptr->setParentItem(d->m_table_viewport);
    a_item_ptr->setParent(this);

    // calculate the number of items per row
    int _item_per_row = (this->boundingRect().width() -
                         (d->m_table_delegate_ptr->left_margin() +
                          d->m_table_delegate_ptr->right_margin())) /
                        a_item_ptr->boundingRect().width();

    if (_item_per_row <= 0) {
      _item_per_row = 1;
    }

    int totalSpace = ((_item_per_row - 1) * d->m_table_delegate_ptr->padding());

    _item_per_row = (this->boundingRect().width() -
                     (d->m_table_delegate_ptr->left_margin() + totalSpace +
                      d->m_table_delegate_ptr->right_margin())) /
                    a_item_ptr->boundingRect().width();

    if (_item_per_row <= 0) {
      _item_per_row = 1;
    }

    d->m_base_grid_layout->addItem(a_item_ptr,
                                   d->m_current_item_count / _item_per_row,
                                   d->m_current_item_count % _item_per_row);
    d->m_base_grid_layout->setAlignment(a_item_ptr, Qt::AlignHCenter);
    d->m_base_grid_layout->activate();
    d->m_base_grid_layout->updateGeometry();

    connect(a_item_ptr, SIGNAL(clicked(TableViewItem *)), this,
            SLOT(on_item_click(TableViewItem *)));
  }
}

void TableView::on_clear() {
  if (d->m_table_delegate_ptr->render_type() == TableModel::kRenderAsListView) {
    for (int i = 0; i < d->m_base_list_layout->count(); i++) {
      QGraphicsLayoutItem *item = d->m_base_list_layout->itemAt(i);

      if (!item) {
        continue;
      }

      QGraphicsItem *_graphics_item = item->graphicsItem();

      if (!_graphics_item) {
        continue;
      }

      _graphics_item->hide();

      d->m_base_list_layout->removeItem(item);

      if (this->scene()) {
        scene()->removeItem(item->graphicsItem());
        delete item;
      }
      d->m_current_item_count--;
      d->m_base_list_layout->activate();
      d->m_base_list_layout->updateGeometry();
    }
  } else if (d->m_table_delegate_ptr->render_type() ==
             TableModel::kRenderAsGridView) {
    for (int i = 0; i < d->m_base_grid_layout->rowCount(); i++) {
      for (int j = 0; j < d->m_base_grid_layout->columnCount(); j++) {
        QGraphicsLayoutItem *item = d->m_base_grid_layout->itemAt(i, j);

        if (!item) {
          continue;
        }

        QGraphicsItem *_graphics_item = item->graphicsItem();

        if (!_graphics_item) {
          continue;
        }

        _graphics_item->hide();
        d->m_base_grid_layout->removeItem(item);

        if (this->scene()) {
          scene()->removeItem(item->graphicsItem());
          delete item;
        }
        d->m_current_item_count--;
      }
    }
    d->m_base_grid_layout->activate();
    d->m_base_grid_layout->updateGeometry();
  }

  d->m_current_item_count = -1;
}

void TableView::wheelEvent(QGraphicsSceneWheelEvent *a_event_ptr) {
  if (!d->m_table_viewport) {
    return;
  }

  QPointF scrollToPos(0.0, 0.0);

  if (a_event_ptr->delta() < 0) {
    scrollToPos.setY(d->m_table_viewport->pos().y() +
                     d->m_table_viewport->boundingRect().height());
  } else {
    scrollToPos.setY(d->m_table_viewport->pos().y() -
                     d->m_table_viewport->boundingRect().height());
  }

  a_event_ptr->accept();

  if (QScroller::hasScroller(this)) {
    QScroller *scroller = QScroller::scroller(this);
    if (scroller) {
      scroller->scrollTo(scrollToPos);
    }
  }
}

bool TableView::event(QEvent *a_event_ptr) {
  switch (a_event_ptr->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick: {
      return QGraphicsObject::event(a_event_ptr);
    }
    case QScrollPrepareEvent::ScrollPrepare: {
      if (!d->m_table_viewport) {
        return false;
      }

      QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(a_event_ptr);
      se->setViewportSize(d->m_table_view_geometry.size());
      QRectF br = d->m_table_viewport->boundingRect();

      qDebug() << Q_FUNC_INFO << " Content Range : Width"
               << qMax(qreal(0),
                       br.width() - d->m_table_view_geometry.size().width())
               << " Height : "
               << qMax(qreal(0),
                       br.height() - d->m_table_view_geometry.size().height())
               << "Content Pos : " << d->m_table_viewport->pos();

      se->setContentPosRange(QRectF(
          0, 0,
          qMax(qreal(0), br.width() - d->m_table_view_geometry.size().width()),
          qMax(qreal(0),
               br.height() - d->m_table_view_geometry.size().height())));
      se->setContentPos(-d->m_table_viewport->pos());

      se->accept();
      return QGraphicsObject::event(a_event_ptr);
    }
    case QScrollEvent::Scroll: {
      if (!d->m_table_viewport) {
        return false;
      }
      QScrollEvent *se = static_cast<QScrollEvent *>(a_event_ptr);
      d->m_table_viewport->setPos(-se->contentPos() - se->overshootDistance());
      return true;
    }

    default:
      break;
  }
  return QGraphicsObject::event(a_event_ptr);
}

bool TableView::sceneEvent(QEvent *a_event_ptr) {
  switch (a_event_ptr->type()) {
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
  return QGraphicsObject::sceneEvent(a_event_ptr);
}

void TableView::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  clear_selection();
  QGraphicsItem::mousePressEvent(a_event_ptr);
}

void TableView::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  QGraphicsObject::mouseReleaseEvent(a_event_ptr);
}
}
