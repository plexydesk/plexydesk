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

#include <QVariant>
#include <ck_style.h>

namespace cherry_kit {

style::style() {}

style::~style() {}

QVariant style::attribute(const QString &a_type, const QString &a_name) const {
  return attribute_map(a_type)[a_name];
}

QString style::color(const QString &a_element) const {
  return color_scheme_map()[a_element].toString();
}

float style::scale_factor() { return 1.0f; }
}
