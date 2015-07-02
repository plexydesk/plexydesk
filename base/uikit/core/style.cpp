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

#include <QVariant>
#include <style.h>

namespace CherryKit {

Style::Style() {}

Style::~Style() {}

QVariant Style::attribute(const QString &a_type, const QString &a_name) const {
  return attribute_map(a_type)[a_name];
}

QString Style::color(const QString &a_element) const {
  return color_scheme_map()[a_element].toString();
}

float Style::scale_factor() { return 1.0f; }
}
