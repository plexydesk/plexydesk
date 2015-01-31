/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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

#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <config.h>

#include <QtCore>

#include <iconjob.h>
#include <plexy.h>

namespace PlexyDesk {

class IconProvider : public QObject {
  Q_OBJECT
public:
  IconProvider();
  IconJobPtr requestIcon(const QString &name, const QString &);

private:
  class Private;
  Private *const d;
};

typedef QSharedPointer<IconProvider> IconProviderPtr;
}

#endif // ICONPROVIDER_H
