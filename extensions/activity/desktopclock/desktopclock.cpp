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
#include "desktopclock.h"
#include <widget.h>
#include <plexyconfig.h>
#include <view_controller.h>
#include <toolbar.h>

#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <themepackloader.h>
#include <label.h>
#include <tableview.h>
#include <space.h>
#include <item_view.h>

#include "clockwidget.h"

class DesktopClockActivity::PrivateDesktopClock {
public:
  PrivateDesktopClock() {}
  ~PrivateDesktopClock() {}

  void add_time_zone(const QString &a_time_zone,
                     const QByteArray &a_time_zone_id);

  UIKit::Window *mMainWindow;
  UIKit::Widget *mWindowContentWidget;

  QGraphicsWidget *mLayoutWidget;
  QGraphicsLinearLayout *m_main_layout;

  UIKit::ToolBar *m_tool_bar;
  UIKit::Label *m_timezone_label;
  UIKit::ClockWidget *m_clock_widget;

  UIKit::ItemView *m_timezone_list;

  QString m_country;
  QString m_city;
};

DesktopClockActivity::DesktopClockActivity(QGraphicsObject *object)
    : UIKit::DesktopActivity(object), d(new PrivateDesktopClock) {}

DesktopClockActivity::~DesktopClockActivity() { delete d; }

void DesktopClockActivity::createFrameWindow(const QRectF &window_geometry,
                                             const QString &window_title) {
  d->mMainWindow = new UIKit::Window();
  d->mMainWindow->setGeometry(window_geometry);

  set_geometry(window_geometry);
}

void DesktopClockActivity::create_window(const QRectF &window_geometry,
                                         const QString &window_title,
                                         const QPointF &window_pos) {
  QRectF _layout_geometry = window_geometry;
  QRectF _clock_geometry(0.0, 0.0, 240, 240);

  createFrameWindow(window_geometry, window_title);

  d->mWindowContentWidget = new UIKit::Widget(d->mMainWindow);
  d->mWindowContentWidget->setGeometry(window_geometry);

  d->mLayoutWidget = new QGraphicsWidget(d->mWindowContentWidget);
  d->m_main_layout = new QGraphicsLinearLayout(d->mLayoutWidget);

  d->mLayoutWidget->setGeometry(window_geometry);

  d->m_main_layout->setGeometry(_layout_geometry);
  d->m_main_layout->setOrientation(Qt::Vertical);

  d->mMainWindow->set_window_content(d->mWindowContentWidget);

  /* add clock widget */
  d->m_clock_widget = new UIKit::ClockWidget(d->mLayoutWidget);
  d->m_clock_widget->setGeometry(_clock_geometry);
  d->m_clock_widget->setMinimumSize(_clock_geometry.size());
  d->m_clock_widget->setMaximumHeight(_clock_geometry.height());
  d->m_clock_widget->setMaximumWidth(_clock_geometry.height());
  d->m_main_layout->addItem(d->m_clock_widget);

  /* Add Toolbar Widget */
  d->m_tool_bar = new UIKit::ToolBar(d->mLayoutWidget);

  d->m_timezone_label = new UIKit::Label(d->m_tool_bar);
  d->m_timezone_label->set_size(QSizeF(window_geometry.width() - 64, 32.0));
  d->m_timezone_label->set_label(tr("Time Zone"));

  d->m_tool_bar->set_icon_resolution("hdpi");
  d->m_tool_bar->insert_widget(d->m_timezone_label);
  d->m_tool_bar->add_action(tr("TimeZone"), "pd_add_new_icon", false);
  d->m_tool_bar->setGeometry(d->m_tool_bar->frame_geometry());

  d->m_main_layout->addItem(d->m_tool_bar);

  /* Add table view */
  QRectF _timezone_table_rect(0.0, 0.0, window_geometry.width(), 128);

  d->m_timezone_list = new UIKit::ItemView(d->mLayoutWidget);
  d->m_timezone_list->set_view_geometry(_timezone_table_rect);

  d->m_main_layout->addItem(d->m_timezone_list);

  exec(window_pos);
  connect(d->m_tool_bar, SIGNAL(action(QString)), this,
          SLOT(onToolBarAction(QString)));

  d->mMainWindow->on_window_discarded([this](UIKit::Window *aWindow) {
    discard_activity();
  });
}

void DesktopClockActivity::PrivateDesktopClock::add_time_zone(
    const QString &a_time_zone_label, const QByteArray &a_time_zone_id) {
  UIKit::ModelViewItem *_item_ptr = new UIKit::ModelViewItem();
  UIKit::Label *_item_label_ptr = new UIKit::Label(m_timezone_list);

  _item_label_ptr->set_label(a_time_zone_label);
  _item_label_ptr->set_alignment(Qt::AlignLeft);
  _item_label_ptr->set_size(
      QSizeF(m_timezone_list->boundingRect().width(), 48));
  _item_ptr->set_view(_item_label_ptr);

  _item_ptr->on_activated([&](UIKit::ModelViewItem *a_item) {
    if (!a_item)
      return;

    if (m_clock_widget)
      m_clock_widget->set_timezone_id(a_item->data("zone_id").toByteArray());
    if (mMainWindow) {
      mMainWindow->set_window_title(a_item->data("zone_id").toString());
      mMainWindow->update();
    }
  });

  _item_ptr->set_data("zone_id", a_time_zone_id);

  m_timezone_list->insert(_item_ptr);
  // m_timezone_model->insertItem(a_time_zone_label, QPixmap(), false);
}

QVariantMap DesktopClockActivity::result() const { return QVariantMap(); }

UIKit::Window *DesktopClockActivity::window() const { return d->mMainWindow; }

void DesktopClockActivity::cleanup() {
  if (d->mMainWindow) {
    delete d->mMainWindow;
  }

  d->mMainWindow = 0;
}

void DesktopClockActivity::onToolBarAction(const QString &str) {
  if (str.toLower() == "timezone") {
    qDebug() << Q_FUNC_INFO << "Create timezone activity";

    if (viewport()) {
      UIKit::Space *_space = qobject_cast<UIKit::Space *>(viewport());
      if (_space) {
        UIKit::DesktopActivityPtr _timezone = viewport()->create_activity(
            "timezone", tr("TimeZone"), _space->cursor_pos(),
            QRectF(0.0, 0.0, 240, 320.0), QVariantMap());
        _space->add_activity(_timezone);

        _timezone->on_action_completed([this](const QVariantMap &a_data) {
          d->add_time_zone(a_data["timezone"].toString(),
                           a_data["zone_id"].toByteArray());
        });
      }
    } else {
      qWarning() << Q_FUNC_INFO << "Viewport not found for this activity";
    }
  }
}
