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

#ifndef PLEXY_CONIFG_LINUX_QT_H
#define PLEXY_CONIFG_LINUX_QT_H

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkProxy>
#include <QtCore/QStringList>

#include <plexy.h>
#include <imagecache.h>

#include <QNetworkAccessManager>

#include "plexydeskuicore_global.h"
#include <plexydesk_ui_exports.h>

namespace UIKit
{
class DECL_UI_KIT_EXPORT Config : public QObject
{
  Q_OBJECT

public:
  static Config *getInstance();

  virtual ~Config();

  QString prefix();

  static QString cacheDir(const QString &folder = QString());

  static QNetworkAccessManager *getNetworkAccessManager();

private:
  Config(const QString &organization, const QString &application = QString(),
         QObject *a_parent_ptr = 0);
  Config();
  Config(Config &config);
  static Config *config;
  static QNetworkAccessManager *networkaccessmanager;

  class Private;
  Private *const d;

  Config &operator=(const Config &);
};
} // namespace PlexyDesk
#endif
