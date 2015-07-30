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

#include <config.h>

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QtDebug>
#include "ck_config.h"
#include <ck_widget.h>
#include <QDesktopWidget>

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace cherry_kit {

class config::Private {
public:
  Private() {}
  ~Private() {}
};

config *config::g_config = 0;

config *config::instance() {
  if (g_config == 0) {
    g_config = new config();
    return g_config;
  } else {
    return g_config;
  }
}

config::~config() { delete d; }

QString config::prefix() {
#ifdef Q_OS_WIN32
  QDir binaryPath(QCoreApplication::applicationDirPath());
  return QDir::toNativeSeparators(binaryPath.canonicalPath());
#endif

#ifdef Q_OS_LINUX
  QString basePath(qgetenv("PLEXYDESK_DIR"));
  if (basePath.isEmpty() || basePath.isNull()) {
    return PLEXYPREFIX;
  }

  return basePath;
#endif

#ifdef Q_OS_MAC
  CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath =
      CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
  const char *pathPtr =
      CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  CFRelease(appUrlRef);
  CFRelease(macPath);
  return QLatin1String(pathPtr) + QString("/Contents/");
#endif

  return QString();
}

QString config::cache_dir(const QString &a_folder) {
  QString rv = QDir::toNativeSeparators(QDir::homePath() + "/" +
                                        ".plexydesk/cache/" + a_folder);
  QDir(QDir::homePath()).mkpath(rv);
  return rv;
}

config::config() : d(new Private) {}
}
