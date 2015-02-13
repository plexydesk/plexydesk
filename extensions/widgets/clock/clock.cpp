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
  PrivateClockController() {}

  ~PrivateClockController() {}
};

Clock::Clock(QObject *parent)
  : UIKit::ViewController(parent), d(new PrivateClockController)
{
  clock = 0;
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
  m_supported_action_list << createAction(1, tr("Clock"),
                                          "pd_clock_frame_icon.png");
  m_supported_action_list << createAction(2, tr("Timer"),
                                          "pd_clock_frame_icon.png");
  m_supported_action_list << createAction(3, tr("Alarm"),
                                          "pd_clock_frame_icon.png");
}

void Clock::revokeSession(const QVariantMap & /*args*/) {}

void Clock::setViewRect(const QRectF &rect)
{
  if (clock) {
    clock->setPos(rect.x(), rect.y());
  }
}

bool Clock::removeWidget(UIKit::Widget *widget)
{
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  if (widget == clock) {
    delete clock;
    clock = 0;
    return 1;
  }

  Q_FOREACH(ClockWidget * _clock, mClocks) {
    if (_clock && _clock == widget) {
      mClocks.removeAt(index);
      delete _clock;
      _clock = 0;
      return 1;
    }

    index++;
  }

  return 1;
}

UIKit::ActionList Clock::actions() const { return m_supported_action_list; }

void Clock::requestAction(const QString &actionName, const QVariantMap &args)
{
  if (!viewport()) {
    return;
  }

  if (actionName == tr("Clock")) {
    QRectF _view_geomeetry(0.0, 0.0, 260.0, 512.0);

    UIKit::DesktopActivityPtr _clock_activity =
      UIKit::ExtensionManager::instance()->activity("desktopclock");

    _clock_activity->createWindow(_view_geomeetry, "Montreal",
                                  viewport()->center(_view_geomeetry));
    if (viewport()) {
      viewport()->addActivity(_clock_activity);
    }
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data)
{
  if (clock) {
    clock->updateTime(data);
  }

  Q_FOREACH(ClockWidget * _clock, mClocks) {
    if (_clock) {
      _clock->updateTime(data);
    }
  }
}
