/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexyplanet.org>
*  Authored By  : Siraj Razick <siraj@plexyplanet.org>
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

#ifndef PLEXYDESKTOPICONPROVIDER_H
#define PLEXYDESKTOPICONPROVIDER_H

#include <QFileIconProvider>

class PlexyDesktopIconProvider : public QFileIconProvider {
public:
  PlexyDesktopIconProvider();
  virtual ~PlexyDesktopIconProvider();
  virtual QIcon icon(IconType type) const;
  virtual QIcon icon(const QFileInfo &info) const;
  virtual QString type(const QFileInfo &info) const;
};

#endif // PLEXYDESKTOPICONPROVIDER_H
