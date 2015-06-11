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

#ifndef DESKTOPMANAGER_H
#define DESKTOPMANAGER_H

#include <plexy.h>
#include <workspace.h>
#include <view_controller.h>

using namespace UIKit;

class DesktopManager : public UIKit::WorkSpace {
public:
  explicit DesktopManager(QWidget *a_parent_ptr = 0);
  virtual ~DesktopManager();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:
  class PrivateDesktopManager;
  PrivateDesktopManager *const m_priv_impl;
};

#endif // DESKTOPMANAGER_H
