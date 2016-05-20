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
#include "classicinterface.h"
#include "background_controller.h"
#include <ck_widget.h>

#include <QtPlugin>

ClassicBackdrop::ClassicBackdrop(QObject * /*parent*/)
    : cherry_kit::desktop_plugin_interface() {}

ClassicBackdrop::~ClassicBackdrop() {
    m_instance_ref.reset();
}

static void remove_obj(cherry_kit::desktop_controller_interface *obj) {
	delete obj;
}

cherry_kit::desktop_controller_ref
ClassicBackdrop::controller() {
  m_instance_ref =
      cherry_kit::desktop_controller_ref(
          new desktop_controller_impl());

  return m_instance_ref;
}
