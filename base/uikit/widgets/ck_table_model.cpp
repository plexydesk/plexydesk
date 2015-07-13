/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : Siraj Razick <siraj@plexydesk.com>
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

#include "ck_table_model.h"

namespace cherry_kit {

table_model::table_model(QObject *parent) : QObject(parent) {}

table_model::~table_model() {}

float table_model::padding() const { return 0.0; }

float table_model::left_margin() const { return 0.0; }

float table_model::right_margin() const { return 0.0; }

table_model::TableRenderMode table_model::render_type() const {
  return kRenderAsListView;
}

float table_model::margin() const { return 0.0; }
}
