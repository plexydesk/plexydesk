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
#ifndef TAKENOTE_DATA_I
#define TAKENOTE_DATA_I

#include <QtCore>
#include <plexy.h>
#include <activity_interface.h>
#include <desktopactivity.h>

class TakeNoteInterface : public QObject, public UIKit::ActivityInterface
{
  Q_OBJECT
  Q_INTERFACES(UIKit::ActivityInterface)
  Q_PLUGIN_METADATA(IID "org.qt-project.takenote")

public:
  virtual ~TakeNoteInterface() {}

  /* this will return a valid data plugin pointer*/
  QSharedPointer<UIKit::DesktopActivity> activity();
};

#endif
