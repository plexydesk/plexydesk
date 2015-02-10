/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : *
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

#include "chooser.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <tableviewcellinterface.h>
#include <tableview.h>
#include <widget.h>
#include <imageview.h>
#include <themepackloader.h>
#include "chooseritem.h"
#include "chooseractiondelegate.h"

class IconGridActivity::PrivateIconGrid
{
public:
  PrivateIconGrid() : mFrame(0) {}
  ~PrivateIconGrid() {}

  UI::Window *mFrame;
  UI::TableView *mTable;
  QRectF mBoundingRect;
  QString mSelection;

  QVariantMap m_activity_result;

  ChooserActionDelegate *m_action_delegate;
  bool m_auto_scale_frame;
};

IconGridActivity::IconGridActivity(QGraphicsObject *object)
  : UI::DesktopActivity(object), d(new PrivateIconGrid) {}

IconGridActivity::~IconGridActivity()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void IconGridActivity::createWindow(const QRectF &window_geometry,
                                    const QString &window_title,
                                    const QPointF &window_pos)
{
  if (d->mFrame) {
    return;
  }

  d->mBoundingRect = window_geometry;
  d->m_auto_scale_frame = false;

  d->mFrame = new UI::Window();

  /*
  d->mFrame->setGeometry(window_geometry);
  d->mFrame->setWindowFlag(UI::Widget::kRenderBackground);
  d->mFrame->setWindowFlag(UI::Widget::kConvertToWindowType);
  d->mFrame->setWindowFlag(UI::Widget::kRenderDropShadow);
  */

  setGeometry(window_geometry);

  d->mTable = new UI::TableView(d->mFrame);

  d->mTable->setGeometry(window_geometry);
  d->m_action_delegate = new ChooserActionDelegate(d->mFrame);
  connect(d->mTable, SIGNAL(activated(TableViewItem *)), this,
          SLOT(onClicked(TableViewItem *)));
  d->mTable->setModel(d->m_action_delegate);
  connect(d->mFrame, SIGNAL(closed(UI::Widget *)), this,
          SLOT(onWidgetClosed(UI::Widget *)));


  if (hasAttribute("data")) {
    QVariantMap data = attributes()["data"].toMap();

    foreach(const QVariant & var, data.keys()) {
      d->m_action_delegate->addDataItem(
        var.toString(), UI::Theme::instance()->drawable(
          data[var.toString()].toString(), "hdpi"),
        false);
    }
  }

  if (hasAttribute("auto_scale")) {
    d->m_auto_scale_frame = attributes()["auto_scale"].toBool();
  }

  if (d->m_auto_scale_frame) {
    QRectF _content_rect = d->mTable->geometry();
    _content_rect.setHeight(_content_rect.height() + 64);
    setGeometry(_content_rect);
    d->mFrame->setGeometry(_content_rect);
  }

  d->mFrame->setWindowContent(d->mTable);

  exec(window_pos);

  connect(this, SIGNAL(attributeChanged()), this, SLOT(onArgumentChanged()));
}

/*
QRectF IconGridActivity::geometry() const
{
    return d->mBoundingRect;
}
*/

QVariantMap IconGridActivity::result() const
{
  d->m_activity_result["action"] = d->mSelection;
  return d->m_activity_result;
}

Window *IconGridActivity::window() const { return d->mFrame; }

void IconGridActivity::onWidgetClosed(UI::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onDiscard()));
  discardActivity();
}

void IconGridActivity::onDiscard() { Q_EMIT finished(); }

void IconGridActivity::onClicked(TableViewItem *item)
{
  if (item) {
    GridIcon *i = qobject_cast<GridIcon *>(item);
    if (i) {
      d->mSelection = i->label();

      d->m_activity_result.clear();

      d->m_activity_result["label"] = i->label();

      foreach(const QString & key, i->itemProperties().keys()) {
        d->m_activity_result[key] = i->itemProperties()[key];
      }

      if (d->mTable) {
        d->mTable->clearSelection();
      }

      updateAction();
    }
  }
}

void IconGridActivity::onArgumentChanged()
{
  if (!d->m_action_delegate) {
    return;
  }

  if (hasAttribute("data")) {
    QVariantMap data = attributes()["data"].toMap();

    foreach(const QVariant & var, data.values()) {
      QVariantMap _item = var.toMap();
      d->m_action_delegate->addDataItem(_item["label"].toString(),
                                        UI::Theme::instance()->drawable(
                                          _item["icon"].toString(), "hdpi"),
                                        false, _item);
    }
  }

  if (hasAttribute("auto_scale")) {
    d->m_auto_scale_frame = attributes()["auto_scale"].toBool();
  }

  if (d->m_auto_scale_frame) {
    QRectF _content_rect = d->mTable->geometry();
    _content_rect.setHeight(_content_rect.height() + 64);
    setGeometry(_content_rect);
    d->mFrame->setGeometry(_content_rect);
  }
}
