/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : Siraj Razick <siraj@plexydesk.org>
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

#include "tableviewcellinterface.h"

namespace UI
{

TableModel::TableModel(QObject *parent) : QObject(parent) {}

TableModel::~TableModel() {}

float TableModel::padding() const { return 0.0; }

float TableModel::leftMargin() const { return 0.0; }

float TableModel::rightMargin() const { return 0.0; }

TableModel::TableRenderMode TableModel::renderType() const
{
  return kRenderAsListView;
}

float TableModel::margin() const { return 0.0; }
}
