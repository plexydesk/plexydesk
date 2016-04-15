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
#include "webcontentinterface.h"

#include <QDateTime>

namespace social_kit {

WebContentInterface::WebContentInterface() {}

QString WebContentInterface::content() {
  QString rv("HTTP/1.0 200 Ok\r\n"
             "Content-Type: text/html; charset=\"utf-8\"\r\n"
             "\r\n"
             "<html><head></head><body onLoad=\"\"><h1>Authentication "
             "Completed, Please Return to PlexyDesk</h1></body></html>\n");

  return rv;
}
}
