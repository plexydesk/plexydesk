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

#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <config.h>

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QDomDocument>
#include <QDomNamedNodeMap>
#include <ck_extension_manager.h>
#include <ck_config.h>

#include <ck_data_sync.h>
#include <ck_disk_engine.h>
#include <webservice.h>

#include "ck_resource_manager.h"

namespace cherry_kit {

resource_manager *resource_manager::s_theme_instance = 0;

class resource_manager::ThemepackLoaderPrivate {
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

resource_manager::resource_manager(const QString &a_theme_name)
    : o_resource_manager(new ThemepackLoaderPrivate) {
  o_resource_manager->mThemePackPath = QDir::toNativeSeparators(
      QString("%1/%2").arg(cherry_kit::config::instance()->prefix()).arg(
          "/share/plexy/themepack"));

  o_resource_manager->mThemeName = a_theme_name;
  QDir mainConfig(QDir::toNativeSeparators(
      QString("%1/%2/").arg(o_resource_manager->mThemePackPath).arg(
          a_theme_name)));

  o_resource_manager->mXmlConfigFile =
      mainConfig.absoluteFilePath("layout.xml");
  o_resource_manager->mXmlRawFile =
      new QFile(o_resource_manager->mXmlConfigFile);

  if (!o_resource_manager->mXmlRawFile->open(QIODevice::ReadOnly |
                                             QIODevice::Text)) {
    qWarning() << Q_FUNC_INFO << "Failed to open "
               << o_resource_manager->mXmlConfigFile;
  } else {
    if (!o_resource_manager->mXmlDocumentRoot.setContent(
             o_resource_manager->mXmlRawFile)) {
      qWarning() << Q_FUNC_INFO << "Failed to load the xml file";
    }
  }

  set_color_scheme("default");
  o_resource_manager->mStyle =
      cherry_kit::extension_manager::instance()->style("cocoastyle");
}

resource_manager::~resource_manager() {

  /*
  if (staticLoader) {
      delete staticLoader;
      staticLoader = 0;
  }
  */

  delete o_resource_manager;
}

StylePtr resource_manager::default_desktop_style() {
  return o_resource_manager->mStyle;
}

StylePtr resource_manager::style() {
  return instance()->default_desktop_style();
}

resource_manager *resource_manager::instance() {
  if (!s_theme_instance) {
    s_theme_instance = new resource_manager("default");
    return s_theme_instance;
  } else {
    return s_theme_instance;
  }
}

const char *resource_manager::color(resource_manager::ColorName a_name) {
  return instance()->color_code(a_name);
}

QPixmap resource_manager::icon(const QString &a_name,
                               const QString &a_resolution) {
  return instance()->drawable(a_name, a_resolution);
}

QPixmap resource_manager::drawable(const QString &a_fileName,
                                   const QString &a_dpi) {
  QPixmap rv;

  QString iconThemePath =
      QDir::toNativeSeparators(o_resource_manager->mThemePackPath + "/" +
                               o_resource_manager->mThemeName + "/resources/" +
                               a_dpi + "/" + a_fileName);

  QFileInfo fileInfo(iconThemePath);

  if (o_resource_manager->mImageCache.keys().contains(iconThemePath)) {
    return o_resource_manager->mImageCache[iconThemePath];
  }

  if (!fileInfo.exists()) {
    qWarning() << Q_FUNC_INFO << "No file found: " << iconThemePath;
    return rv;
  }

  rv = QPixmap(iconThemePath);
  o_resource_manager->mImageCache[iconThemePath] = rv;

  return rv;
}

void resource_manager::load_default_color_values() {
  o_resource_manager->m_color_map[kDarkPrimaryColor] = "#1976D2";
  o_resource_manager->m_color_map[kPrimaryColor] = "#2196F3";
  o_resource_manager->m_color_map[kLightPrimaryColor] = "#FFFFFF";
  o_resource_manager->m_color_map[kTextBackground] = "#212121";
  o_resource_manager->m_color_map[kAccentColor] = "#FF4081";
  o_resource_manager->m_color_map[kTextColor] = "#646464";
  o_resource_manager->m_color_map[kSecondryTextColor] = "#FFFFFF";
  o_resource_manager->m_color_map[kDividerColor] = "#B6B6B6";
}

void resource_manager::set_color_scheme(const std::string &a_name) {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Palette");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (!a_found) {
      load_default_color_values();
      return;
    }

    o_resource_manager->m_color_map[kDarkPrimaryColor] =
        a_object.property("dark_primary_color");
    o_resource_manager->m_color_map[kPrimaryColor] =
        std::string(a_object.property("primary_color"));
    o_resource_manager->m_color_map[kLightPrimaryColor] =
        a_object.property("light_primary_color");
    o_resource_manager->m_color_map[kTextBackground] =
        a_object.property("text_background_color");
    o_resource_manager->m_color_map[kAccentColor] =
        a_object.property("accent_color");
    o_resource_manager->m_color_map[kTextColor] =
        a_object.property("text_color");
    o_resource_manager->m_color_map[kSecondryTextColor] =
        a_object.property("secondry_text_color");
    o_resource_manager->m_color_map[kDividerColor] =
        a_object.property("divider_color");
  });

  sync->find("color", "scheme_id", a_name);

  delete sync;
}

std::string resource_manager::color_scheme() const { return std::string(); }

const char *resource_manager::color_code(resource_manager::ColorName a_name) {
  std::string error_rv = "#000000";
  const char *rv;

  if (o_resource_manager->m_color_map.find(a_name) ==
      o_resource_manager->m_color_map.end()) {
    rv = error_rv.c_str();
    return rv;
  }

  rv = o_resource_manager->m_color_map.at(a_name).c_str();

  return rv;
}

void resource_manager::set_theme_name(const QString &a_name) {
  Q_UNUSED(a_name);
}

} // namespace plexydesk
