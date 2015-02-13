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
#include "timezone.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>

class TimeZoneActivity::PrivateTimeZone
{
public:
  PrivateTimeZone() {}
  ~PrivateTimeZone()
  {
    if (m_main_window) {
      delete m_main_window;
    }
  }

  UIKit::Window *m_main_window;
};

TimeZoneActivity::TimeZoneActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateTimeZone) {}

TimeZoneActivity::~TimeZoneActivity() { delete d; }

void TimeZoneActivity::createWindow(const QRectF &window_geometry,
                                    const QString &window_title,
                                    const QPointF &window_pos)
{
  d->m_main_window = new UIKit::Window();

  d->m_main_window->setWindowFlag(UIKit::Widget::kRenderBackground);
  d->m_main_window->setWindowFlag(UIKit::Widget::kConvertToWindowType);
  d->m_main_window->setWindowFlag(UIKit::Widget::kRenderDropShadow);

  setGeometry(window_geometry);
  updateContentGeometry(d->m_main_window);

  exec(window_pos);

  connect(d->m_main_window, SIGNAL(closed(UIKit::Widget *)), this,
          SLOT(onWidgetClosed(UIKit::Widget *)));
}

QVariantMap TimeZoneActivity::result() const { return QVariantMap(); }

void TimeZoneActivity::updateAttribute(const QString &name,
                                       const QVariant &data) {}

UIKit::Window *TimeZoneActivity::window() const { return d->m_main_window; }

void TimeZoneActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void TimeZoneActivity::onHideAnimationFinished()
{
  delete d->m_main_window;
  d->m_main_window = 0;
  Q_EMIT finished();
}
