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
#ifndef ACTIVITY_INTERFACE_H
#define ACTIVITY_INTERFACE_H

#include <plexydesk_ui_exports.h>

namespace CherryKit {

class DesktopActivity;

class DECL_UI_KIT_EXPORT ActivityInterface {
public:
  explicit ActivityInterface() {}

  virtual QSharedPointer<CherryKit::DesktopActivity> activity() = 0;
};
} // namespace PlexyDesk

Q_DECLARE_INTERFACE(CherryKit::ActivityInterface,
                    "org.plexydesk.ActivityInterface")
#endif // ACTIVITY_INTERFACE_H
