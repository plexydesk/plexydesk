/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#ifndef PLEXY_CLOCK_I
#define PLEXY_CLOCK_I

#include <QtCore>

#include "clock.h"
#include "ck_clock_view.h"
#include <ck_desktop_controller_interface.h>
#include <ck_desktop_plugin_interface.h>

class time_controller_impl : public QObject,
                       public cherry_kit::desktop_plugin_interface {
  Q_OBJECT
  Q_INTERFACES(cherry_kit::desktop_plugin_interface)
  
#ifdef __QT5_TOOLKIT__
  Q_PLUGIN_METADATA(IID "org.qt-project.clock")
#endif

public:
  cherry_kit::desktop_controller_ref controller();

private:
  time_controller *mClock;
};
#endif
