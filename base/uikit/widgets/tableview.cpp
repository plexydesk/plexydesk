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

namespace UI
{

class TableView::PrivateTableView
{

public:
  PrivateTableView() {}

  ~PrivateTableView()
  {
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
};

TableView::TableView(QGraphicsObject *parent)
  : Window(parent), d(new PrivateTableView)
{
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

TableView::~TableView()
{
  QScroller::ungrabGesture(this);
  delete d;
}

void TableView::setModel(TableModel *iface)
{
  if (d->m_table_delegate_ptr) {
    delete d->m_table_delegate_ptr;
  }

  if (!iface) {
    return;
  }

  d->m_table_delegate_ptr = iface;

  iface->setParent(this);

  if (iface->renderType() == TableModel::kRenderAsListView) {
    d->m_base_list_layout = new QGraphicsLinearLayout(d->m_table_viewport);
    d->m_base_list_layout->setOrientation(Qt::Vertical);
    d->m_base_list_layout->setContentsMargins(0, 0, 0, 0);
    d->m_base_list_layout->setSpacing(0.0);
  } else {
    d->m_base_grid_layout = new QGraphicsGridLayout(d->m_table_viewport);
    d->m_base_grid_layout->setSpacing(d->m_table_delegate_ptr->padding());
    d->m_base_grid_layout->setContentsMargins(
      d->m_table_delegate_ptr->leftMargin(), 0.0,
      d->m_table_delegate_ptr->rightMargin(), 0.0);
  }

  connect(iface, SIGNAL(cleared()), this, SLOT(onClear()));
  connect(iface, SIGNAL(add(UI::TableViewItem *)), this,
          SLOT(onAddViewItem(UI::TableViewItem *)));

  if (iface) {
    iface->init();
  }
}

TableModel *TableView::model() { return d->m_table_delegate_ptr; }

void TableView::clearSelection()
{
  if (!d->m_table_delegate_ptr) {
    return;
  }

  if (d->m_table_delegate_ptr->renderType() == TableModel::kRenderAsListView) {
    // todo
  } else if (d->m_table_delegate_ptr->renderType() ==
             TableModel::kRenderAsGridView) {
    for (int i = 0; i < d->m_base_grid_layout->rowCount(); i++) {
      for (int j = 0; j < d->m_base_grid_layout->columnCount(); j++) {
        TableViewItem *cell =
          (TableViewItem *)d->m_base_grid_layout->itemAt(i, j);
        if (cell) {
          cell->clearSelection();
        }
      }
    }
  }
}

QRectF TableView::boundingRect() const
{
  QRectF rv;
  rv.setWidth(d->m_table_view_geometry.width());
  rv.setHeight(d->m_table_view_geometry.height());
  return rv;
}

void TableView::setGeometry(const QRectF &rect)
{
  qDebug() << Q_FUNC_INFO << rect;
  d->m_table_viewport->setGeometry(rect);
  d->m_table_viewport->setPos(QPointF());
  d->m_table_view_geometry.setWidth(rect.width());
  d->m_table_view_geometry.setHeight(rect.height());
  setPos(rect.topLeft());
  Window::setGeometry(rect);
}

QSizeF TableView::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
  return d->m_table_view_geometry.size();
}

void TableView::scrollBy(int x, int y)
{
  // todo: manual scrolling probably with scrollbars ?
}

void TableView::paintView(QPainter *painter, const QRectF &exposeRect) {}

TableViewItem *TableView::itemAt(uint i)
{
  if (i >= (uint)d->m_current_table_view_item_list.count()) {
    return 0;
  }

  return d->m_current_table_view_item_list.at(i);
}

StylePtr TableView::style() const { return Theme::style(); }

uint TableView::count() const
{
  if (!d->m_table_delegate_ptr) {
    return 0;
  }

  if (d->m_table_delegate_ptr->renderType() == TableModel::kRenderAsGridView) {
    if (!d->m_base_grid_layout) {
      return 0;
    }

    return d->m_base_grid_layout->count();
  } else if (d->m_table_delegate_ptr->renderType() ==
             TableModel::kRenderAsListView) {
    if (!d->m_base_list_layout) {
      return 0;
    }

    return d->m_base_list_layout->count();
  }

  return 0;
}

void TableView::onItemClick(TableViewItem *component)
{
  clearSelection();

  if (component) {
    component->setSelected();
  }

  Q_EMIT activated(component);

  qDebug() << Q_FUNC_INFO << " Activated :";
}

void TableView::onAddViewItem(UI::TableViewItem *item)
{
  if (!d->m_table_delegate_ptr) {
    return;
  }

  d->m_current_item_count++;

  if (d->m_table_delegate_ptr->renderType() == TableModel::kRenderAsListView) {
    item->setParentItem(d->m_table_viewport);
    item->setParent(this);
    d->m_base_list_layout->addItem(item);
    d->m_base_list_layout->setAlignment(item, Qt::AlignHCenter);
    connect(item, SIGNAL(clicked(TableViewItem *)), this,
            SLOT(onItemClick(TableViewItem *)));
    d->m_base_list_layout->activate();
    d->m_base_list_layout->updateGeometry();
  }

  if (d->m_table_delegate_ptr->renderType() == TableModel::kRenderAsGridView) {
    item->setParentItem(d->m_table_viewport);
    item->setParent(this);

    // calculate the number of items per row
    int _item_per_row = (this->boundingRect().width() -
                         (d->m_table_delegate_ptr->leftMargin() +
                          d->m_table_delegate_ptr->rightMargin())) /
                        item->boundingRect().width();

    if (_item_per_row <= 0) {
      _item_per_row = 1;
    }

    int totalSpace = ((_item_per_row - 1) * d->m_table_delegate_ptr->padding());

    _item_per_row = (this->boundingRect().width() -
                     (d->m_table_delegate_ptr->leftMargin() + totalSpace +
                      d->m_table_delegate_ptr->rightMargin())) /
                    item->boundingRect().width();

    if (_item_per_row <= 0) {
      _item_per_row = 1;
    }

    d->m_base_grid_layout->addItem(item,
                                   d->m_current_item_count / _item_per_row,
                                   d->m_current_item_count % _item_per_row);
    d->m_base_grid_layout->setAlignment(item, Qt::AlignHCenter);
    d->m_base_grid_layout->activate();
    d->m_base_grid_layout->updateGeometry();

    connect(item, SIGNAL(clicked(TableViewItem *)), this,
            SLOT(onItemClick(TableViewItem *)));
  }
}

void TableView::onClear()
{
  if (d->m_table_delegate_ptr->renderType() == TableModel::kRenderAsListView) {
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
  } else if (d->m_table_delegate_ptr->renderType() ==
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

void TableView::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  if (!d->m_table_viewport) {
    return;
  }

  QPointF scrollToPos(0.0, 0.0);

  if (event->delta() < 0) {
    scrollToPos.setY(d->m_table_viewport->pos().y() +
                     d->m_table_viewport->boundingRect().height());
  } else {
    scrollToPos.setY(d->m_table_viewport->pos().y() -
                     d->m_table_viewport->boundingRect().height());
  }

  event->accept();

  if (QScroller::hasScroller(this)) {
    QScroller *scroller = QScroller::scroller(this);
    if (scroller) {
      scroller->scrollTo(scrollToPos);
    }
  }
}

bool TableView::event(QEvent *e)
{
  switch (e->type()) {
  case QEvent::GraphicsSceneMouseDoubleClick: {
    return QGraphicsObject::event(e);
  }
  case QScrollPrepareEvent::ScrollPrepare: {
    if (!d->m_table_viewport) {
      return false;
    }

    QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(e);
    se->setViewportSize(d->m_table_view_geometry.size());
    QRectF br = d->m_table_viewport->boundingRect();

    qDebug() << Q_FUNC_INFO << " Content Range : Width"
             << qMax(qreal(0), br.width() - d->m_table_view_geometry.size().width())
             << " Height : "
             <<  qMax(qreal(0), br.height() - d->m_table_view_geometry.size().height())
             << "Content Pos : " << d->m_table_viewport->pos();

    se->setContentPosRange(QRectF(
                             0, 0,
                             qMax(qreal(0), br.width() - d->m_table_view_geometry.size().width()),
                             qMax(qreal(0), br.height() - d->m_table_view_geometry.size().height())));
    se->setContentPos(-d->m_table_viewport->pos());

    se->accept();
    return QGraphicsObject::event(e);
  }
  case QScrollEvent::Scroll: {
    if (!d->m_table_viewport) {
      return false;
    }
    QScrollEvent *se = static_cast<QScrollEvent *>(e);
    d->m_table_viewport->setPos(-se->contentPos() - se->overshootDistance());
    return true;
  }

  default:
    break;
  }
  return QGraphicsObject::event(e);
}

bool TableView::sceneEvent(QEvent *e)
{
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
  return QGraphicsObject::sceneEvent(e);
}

void TableView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  clearSelection();
  QGraphicsItem::mousePressEvent(event);
}

void TableView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsObject::mouseReleaseEvent(event);
}
}
