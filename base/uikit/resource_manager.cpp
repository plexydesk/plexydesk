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

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QDomDocument>
#include <QDomNamedNodeMap>
#include <extensionmanager.h>
#include <plexyconfig.h>

#include <datasync.h>
#include <disksyncengine.h>
#include <webservice.h>

#include "resource_manager.h"

namespace UIKit {

ResourceManager *ResourceManager::s_theme_instance = 0;

class ResourceManager::ThemepackLoaderPrivate {
public:
  ThemepackLoaderPrivate() {}
  ~ThemepackLoaderPrivate() {
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

  std::map<ColorName, std::string> m_color_map;
};

ResourceManager::ResourceManager(const QString &a_theme_name)
    : d(new ThemepackLoaderPrivate) {
  d->mThemePackPath = QDir::toNativeSeparators(
      QString("%1/%2").arg(UIKit::Config::instance()->prefix()).arg(
          "/share/plexy/themepack"));

  d->mThemeName = a_theme_name;
  QDir mainConfig(QDir::toNativeSeparators(
      QString("%1/%2/").arg(d->mThemePackPath).arg(a_theme_name)));

  d->mXmlConfigFile = mainConfig.absoluteFilePath("layout.xml");
  d->mXmlRawFile = new QFile(d->mXmlConfigFile);

  if (!d->mXmlRawFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << Q_FUNC_INFO << "Failed to open " << d->mXmlConfigFile;
  } else {
    if (!d->mXmlDocumentRoot.setContent(d->mXmlRawFile)) {
      qWarning() << Q_FUNC_INFO << "Failed to load the xml file";
    }
  }

  set_color_scheme("default");
  d->mStyle = UIKit::ExtensionManager::instance()->style("cocoastyle");
}

ResourceManager::~ResourceManager() {

  /*
  if (staticLoader) {
      delete staticLoader;
      staticLoader = 0;
  }
  */

  delete d;
}

StylePtr ResourceManager::default_desktop_style() { return d->mStyle; }

StylePtr ResourceManager::style() {
  return instance()->default_desktop_style();
}

ResourceManager *ResourceManager::instance() {
  if (!s_theme_instance) {
    s_theme_instance = new ResourceManager("default");
    return s_theme_instance;
  } else {
    return s_theme_instance;
  }
}

const char *ResourceManager::color(ResourceManager::ColorName a_name) {
  return instance()->color_code(a_name);
}

QPixmap ResourceManager::icon(const QString &a_name,
                              const QString &a_resolution) {
  return instance()->drawable(a_name, a_resolution);
}

QPixmap ResourceManager::drawable(const QString &a_fileName,
                                  const QString &a_dpi) {
  QPixmap rv;

  QString iconThemePath =
      QDir::toNativeSeparators(d->mThemePackPath + "/" + d->mThemeName +
                               "/resources/" + a_dpi + "/" + a_fileName);

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

void ResourceManager::load_default_color_values() {
  d->m_color_map[kDarkPrimaryColor] = "#1976D2";
  d->m_color_map[kPrimaryColor] = "#2196F3";
  d->m_color_map[kLightPrimaryColor] = "#FFFFFF";
  d->m_color_map[kTextBackground] = "#212121";
  d->m_color_map[kAccentColor] = "#FF4081";
  d->m_color_map[kTextColor] = "#646464";
  d->m_color_map[kSecondryTextColor] = "#FFFFFF";
  d->m_color_map[kDividerColor] = "#B6B6B6";
}

void ResourceManager::set_color_scheme(const std::string &a_name) {
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Palette");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](QuetzalKit::SyncObject &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (!a_found) {
      load_default_color_values();
      return;
    }

    d->m_color_map[kDarkPrimaryColor] = std::string(
        a_object.attributeValue("dark_primary_color").toByteArray());
    d->m_color_map[kPrimaryColor] =
        std::string(a_object.attributeValue("primary_color").toByteArray());
    d->m_color_map[kLightPrimaryColor] = std::string(
        a_object.attributeValue("light_primary_color").toByteArray());
    d->m_color_map[kTextBackground] = std::string(
        a_object.attributeValue("text_background_color").toByteArray());
    d->m_color_map[kAccentColor] =
        std::string(a_object.attributeValue("accent_color").toByteArray());
    d->m_color_map[kTextColor] =
        std::string(a_object.attributeValue("text_color").toByteArray());
    d->m_color_map[kSecondryTextColor] = std::string(
        a_object.attributeValue("secondry_text_color").toByteArray());
    d->m_color_map[kDividerColor] =
        std::string(a_object.attributeValue("divider_color").toByteArray());
  });

  sync->find("color", "scheme_id", a_name);

  delete sync;
}

std::string ResourceManager::color_scheme() const { return std::string(); }

const char *ResourceManager::color_code(ResourceManager::ColorName a_name) {
  std::string error_rv = "#000000";
  const char *rv;

  if (d->m_color_map.find(a_name) == d->m_color_map.end()) {
    rv = error_rv.c_str();
    return rv;
  }

  rv = d->m_color_map.at(a_name).c_str();

  return rv;
}

void ResourceManager::set_theme_name(const QString &a_name) {
  Q_UNUSED(a_name);
}

} // namespace plexydesk
