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
#ifndef DATA_PLUGININTERFACE_H
#define DATA_PLUGININTERFACE_H

#include <QSharedPointer>
#include <QObject>

#include <datasource.h>
#include <plexydesk_ui_exports.h>

namespace CherryKit {

class DECL_UI_KIT_EXPORT DataPluginInterface {
public:
  DataPluginInterface() {}

  virtual ~DataPluginInterface() {}

  virtual QSharedPointer<DataSource> model() = 0;
};
}

Q_DECLARE_INTERFACE(CherryKit::DataPluginInterface,
                    "org.plexydesk.DataPluginInterface")
#endif // DATA_PLUGININTERFACE_H
