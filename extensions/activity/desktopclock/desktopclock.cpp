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

#include "clockwidget.h"
#include "timezone_model.h"

class DesktopClockActivity::PrivateDesktopClock
{
public:
  PrivateDesktopClock() {}

  ~PrivateDesktopClock() {}

  UI::Widget *m_main_window;
  QGraphicsWidget *m_layout_widget;
  QGraphicsLinearLayout *m_main_layout;
  UI::ToolBar *m_tool_bar;
  UI::Label *m_timezone_label;
  ClockWidget *m_clock_widget;
  UI::TableView *m_timezone_table;
  TimeZoneModel *m_timezone_model;
  QString m_country;
  QString m_city;
};

DesktopClockActivity::DesktopClockActivity(QGraphicsObject *object)
  : UI::DesktopActivity(object), d(new PrivateDesktopClock) {}

DesktopClockActivity::~DesktopClockActivity() { delete d; }

void DesktopClockActivity::createFrameWindow(const QRectF &window_geometry,
    const QString &window_title)
{
  d->m_main_window = new UI::Widget();
  d->m_main_window->setGeometry(window_geometry);

  d->m_main_window->setWindowFlag(UI::Widget::kRenderBackground);
  d->m_main_window->setWindowFlag(UI::Widget::kConvertToWindowType);
  d->m_main_window->setWindowFlag(UI::Widget::kRenderDropShadow);

  setGeometry(window_geometry);
}

void DesktopClockActivity::createWindow(const QRectF &window_geometry,
                                        const QString &window_title,
                                        const QPointF &window_pos)
{
  QRectF _layout_geometry = window_geometry;
  QRectF _clock_geometry(0.0, 0.0, 240, 240);

  createFrameWindow(window_geometry, window_title);

  d->m_layout_widget = new QGraphicsWidget(d->m_main_window);
  d->m_main_layout = new QGraphicsLinearLayout(d->m_layout_widget);

  if (UI::Theme::style()) {
    float _window_title_height = UI::Theme::style()
                                 ->attrbute("frame", "window_title_height")
                                 .toFloat();
    _layout_geometry.setY(_window_title_height);
    _layout_geometry.setHeight(window_geometry.height() - _window_title_height);
  }

  d->m_layout_widget->setGeometry(_layout_geometry);

  d->m_main_layout->setGeometry(_layout_geometry);
  d->m_main_layout->setOrientation(Qt::Vertical);

  /* add clock widget */
  d->m_clock_widget = new ClockWidget(d->m_layout_widget);
  d->m_clock_widget->setGeometry(_clock_geometry);
  d->m_clock_widget->setMinimumSize(_clock_geometry.size());
  d->m_clock_widget->setMaximumHeight(_clock_geometry.height());
  d->m_clock_widget->setMaximumWidth(_clock_geometry.height());
  d->m_main_layout->addItem(d->m_clock_widget);

  /* Add Toolbar Widget */
  d->m_tool_bar = new UI::ToolBar(d->m_layout_widget);

  d->m_timezone_label = new UI::Label(d->m_tool_bar);
  d->m_timezone_label->setSize(QSizeF(window_geometry.width() - 64, 32.0));
  d->m_timezone_label->setLabel(tr("Time Zone"));

  d->m_tool_bar->setIconResolution("hdpi");
  d->m_tool_bar->insertWidget(d->m_timezone_label);
  d->m_tool_bar->addAction(tr("TimeZone"), "pd_add_new_icon", false);
  d->m_tool_bar->setGeometry(d->m_tool_bar->frameGeometry());

  d->m_main_layout->addItem(d->m_tool_bar);

  /* Add table view */
  QRectF _timezone_table_rect(0.0, 0.0, window_geometry.width(), 128);

  d->m_timezone_table = new UI::TableView(d->m_layout_widget);
  d->m_timezone_table->setGeometry(_timezone_table_rect);

  d->m_timezone_model = new TimeZoneModel(d->m_timezone_table);
  d->m_timezone_table->setModel(d->m_timezone_model);
  // d->m_timezone_table->setPos(0.0, 0.0);
  d->m_timezone_model->insertItem("OMG", QPixmap(), false);

  d->m_main_layout->addItem(d->m_timezone_table);

  exec(window_pos);

  connect(d->m_main_window, SIGNAL(closed(UI::Widget *)), this,
          SLOT(onWidgetClosed(UI::Widget *)));
  connect(d->m_tool_bar, SIGNAL(action(QString)), this,
          SLOT(onToolBarAction(QString)));
}

QVariantMap DesktopClockActivity::result() const { return QVariantMap(); }

UI::Widget *DesktopClockActivity::window() const
{
  return d->m_main_window;
}

void DesktopClockActivity::onWidgetClosed(UI::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void DesktopClockActivity::onHideAnimationFinished() { Q_EMIT finished(); }

void DesktopClockActivity::onToolBarAction(const QString &str)
{
  if (str.toLower() == "timezone") {
    qDebug() << Q_FUNC_INFO << "Create timezone activity";

    if (viewport()) {
      UI::Space *_space = qobject_cast<UI::Space *>(viewport());
      if (_space) {
        UI::DesktopActivityPtr _timezone = viewport()->createActivity(
                                             "timezone", tr("TimeZone"), _space->mousePointerPos(),
                                             QRectF(0.0, 0.0, 240, 320.0), QVariantMap());
        _space->addActivity(_timezone);
      }
    } else {
      qWarning() << Q_FUNC_INFO << "Viewport not found for this activity";
    }
  }
}
