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

#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <config.h>

#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNamedNodeMap>
#include <extensionmanager.h>
#include <plexyconfig.h>

#include "themepackloader.h"

namespace UIKit
{

Theme *Theme::s_theme_instance = 0;

class Theme::ThemepackLoaderPrivate
{
public:
  ThemepackLoaderPrivate() {}
  ~ThemepackLoaderPrivate()
  {
    if (mXmlRawFile) {
      delete mXmlRawFile;
    }

    if (mStyle) {
      mStyle.clear();
    }
  }

  QString mThemeName;
  QString mThemePackPath;
  QRectF mScreenRect;

  QString mXmlConfigFile;
  QDomDocument mXmlDocumentRoot;
  QFile *mXmlRawFile;

  QHash<QString, QPixmap> mImageCache;

  // style
  StylePtr mStyle;
};

Theme::Theme(const QString &themeName, QObject *parent)
  : QObject(parent), d(new ThemepackLoaderPrivate)
{
  d->mThemePackPath = QDir::toNativeSeparators(
                        QString("%1/%2").arg(UIKit::Config::instance()->prefix()).arg(
                          "/share/plexy/themepack"));

  d->mThemeName = themeName;
  QDir mainConfig(QDir::toNativeSeparators(
                    QString("%1/%2/").arg(d->mThemePackPath).arg(themeName)));

  d->mXmlConfigFile = mainConfig.absoluteFilePath("layout.xml");
  d->mXmlRawFile = new QFile(d->mXmlConfigFile);

  if (!d->mXmlRawFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << Q_FUNC_INFO << "Failed to open " << d->mXmlConfigFile;
  } else {
    if (!d->mXmlDocumentRoot.setContent(d->mXmlRawFile)) {
      qWarning() << Q_FUNC_INFO << "Failed to load the xml file";
    }
  }

  d->mStyle = UIKit::ExtensionManager::instance()->style("cocoastyle");
}

Theme::~Theme()
{

  /*
  if (staticLoader) {
      delete staticLoader;
      staticLoader = 0;
  }
  */

  delete d;
}

StylePtr Theme::default_desktop_style() { return d->mStyle; }

StylePtr Theme::style() { return instance()->default_desktop_style(); }

Theme *Theme::instance()
{
  if (!s_theme_instance) {
    s_theme_instance = new Theme("default", 0);
    return s_theme_instance;
  } else {
    return s_theme_instance;
  }
}

QPixmap Theme::icon(const QString &name, const QString &resolution)
{
  return instance()->drawable(name, resolution);
}

QPixmap Theme::drawable(const QString &fileName, const QString &resoution)
{
  QPixmap rv;

  QString iconThemePath =
    QDir::toNativeSeparators(d->mThemePackPath + "/" + d->mThemeName +
                             "/resources/" + resoution + "/" + fileName);

  QFileInfo fileInfo(iconThemePath);

  if (d->mImageCache.keys().contains(iconThemePath)) {
    return d->mImageCache[iconThemePath];
  }

  if (!fileInfo.exists()) {
    qWarning() << Q_FUNC_INFO << "No file found: " << iconThemePath;
    return rv;
  }

  rv = QPixmap(iconThemePath);
  d->mImageCache[iconThemePath] = rv;

  return rv;
}

void Theme::set_theme_name(const QString &name) { Q_UNUSED(name); }

} // namespace plexydesk
