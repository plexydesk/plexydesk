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
#ifndef PHOTOSEARCH_DATA_I
#define PHOTOSEARCH_DATA_I

#include <QtCore>

#include <ck_dialog_plugin_interface.h>
#include <ck_desktop_dialog.h>

class desktop_settings_plugin_impl : public QObject,
                             public cherry_kit::dialog_plugin_interface {
  Q_OBJECT
  Q_INTERFACES(cherry_kit::dialog_plugin_interface)
  Q_PLUGIN_METADATA(IID "org.qt-project.photosearch")

public:
  virtual ~desktop_settings_plugin_impl() {}

  /* this will return a valid data plugin pointer*/
  cherry_kit::desktop_dialog_ref activity();
};

#endif
