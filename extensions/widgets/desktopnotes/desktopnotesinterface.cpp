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
#include "desktopnotes.h"
#include "desktopnotesinterface.h"
#include <ck_widget.h>
#include <ck_config.h>

QSharedPointer<cherry_kit::desktop_controller_interface>
task_plugin_impl::controller() {
  QSharedPointer<cherry_kit::desktop_controller_interface> obj =
      QSharedPointer<cherry_kit::desktop_controller_interface>(
          new desktop_task_controller_impl(this), &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(desktopnotesengine, DesktopNotesInterface)
