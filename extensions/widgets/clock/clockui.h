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
#ifndef CLOCKUI_H
#define CLOCKUI_H

#include "session_sync.h"

#include <clockwidget.h>
#include <label.h>
#include <toolbar.h>
#include <view_controller.h>

class ClockUI
{
public:
  ClockUI(SessionSync *a_sync, UIKit::ViewController *a_controller);
  virtual ~ClockUI();

private:
  UIKit::ViewController *m_controller;
  UIKit::Window *m_clock_session_window;
  UIKit::Widget *m_content_view;
  UIKit::ClockWidget *m_clock_widget;
  UIKit::ToolBar *m_toolbar;
  UIKit::Label *m_timezone_label;
  SessionSync *m_session;
};

#endif // CLOCKUI_H
