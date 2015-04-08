/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  :
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
// internal
#include "clock.h"
#include "clockwidget.h"

#include <QDebug>

// uikit
#include <view_controller.h>
#include <extensionmanager.h>

// Qt
#include <QAction>

class Clock::PrivateClockController
{
public:
  PrivateClockController() : m_clock_activity_count(0) {}
  ~PrivateClockController() {}

  UIKit::ActionList m_supported_action_list;
  int m_clock_activity_count;
};

Clock::Clock(QObject *parent)
  : UIKit::ViewController(parent), d(new PrivateClockController)
{
}

Clock::~Clock()
{
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete d;
}

QAction *Clock::createAction(int id, const QString &action_name,
                             const QString &icon_name)
{
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 0);

  return _add_clock_action;
}

void Clock::init()
{
  d->m_supported_action_list << createAction(1, tr("Clock"),
                                          "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(2, tr("Timer"),
                                          "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(3, tr("Alarm"),
                                          "pd_clock_frame_icon.png");
}

void Clock::revoke_session(const QVariantMap & /*args*/) {}

void Clock::set_view_rect(const QRectF &rect)
{
}

bool Clock::remove_widget(UIKit::Widget *widget)
{
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  return 1;
}

UIKit::ActionList Clock::actions() const { return d->m_supported_action_list; }

void Clock::request_action(const QString &actionName, const QVariantMap &args)
{
  if (!viewport()) {
    return;
  }

  if (actionName == tr("Clock")) {
    QRectF _view_geomeetry(0.0, 0.0, 260.0, 512.0);

    UIKit::DesktopActivityPtr _clock_activity =
      UIKit::ExtensionManager::instance()->activity("desktopclock");

    _clock_activity->create_window(_view_geomeetry, args["zone_id"].toString(),
                                  viewport()->center(_view_geomeetry));
    _clock_activity->update_attribute("id",
                                      QString("clock-%1").arg(
                                        d->m_clock_activity_count));
    d->m_clock_activity_count++;
    _clock_activity->on_discarded([&](const UIKit::DesktopActivity *a_activity) {
       //remove from current session.
        if (!a_activity)
          return;

        QVariantMap attrib = a_activity->attributes();
        qDebug() << Q_FUNC_INFO << attrib;
        if (viewport()) {
            viewport()->update_session_value(controller_name(), "id", "");
        }
      });

    if (viewport()) {
      viewport()->add_activity(_clock_activity);
    }
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data)
{
}
