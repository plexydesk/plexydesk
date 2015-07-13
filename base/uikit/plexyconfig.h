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

#ifndef PLEXY_CONIFG_LINUX_QT_H
#define PLEXY_CONIFG_LINUX_QT_H

#include <QString>

#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT config {
public:
  static config *instance();
  virtual ~config();

  QString prefix();

  static QString cache_dir(const QString &a_folder = QString());

private:
  config();
  config(config &a_config);
  static config *g_config;

  class Private;
  Private *const d;

  config &operator=(const config &);
};
} // namespace PlexyDesk
#endif
