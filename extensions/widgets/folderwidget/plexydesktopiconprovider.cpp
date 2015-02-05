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

#include "plexydesktopiconprovider.h"
#include <QDebug>
#include <QFileInfo>

PlexyDesktopIconProvider::PlexyDesktopIconProvider() {}

PlexyDesktopIconProvider::~PlexyDesktopIconProvider() {}

QIcon PlexyDesktopIconProvider::icon(QFileIconProvider::IconType type) const
{
  QIcon rv = QFileIconProvider::icon(type);
  return QIcon::fromTheme(rv.name(), rv);
}

QIcon PlexyDesktopIconProvider::icon(const QFileInfo &info) const
{
  QIcon rv = QFileIconProvider::icon(info);
  return QIcon::fromTheme(rv.name(), rv);
}

QString PlexyDesktopIconProvider::type(const QFileInfo &info) const
{
  return QFileIconProvider::type(info);
}
