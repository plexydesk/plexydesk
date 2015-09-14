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
    if (m_current_style_ref) {
      m_current_style_ref.clear();
    }
  }

  std::string get_dpi() const;


  QString m_resource_name;
  QString m_resource_group;

  QHash<QString, QPixmap> m_image_cache;
  std::map<ColorName, std::string> m_color_map;

  StylePtr m_current_style_ref;
};

resource_manager::resource_manager(const QString &a_theme_name)
    : priv(new ThemepackLoaderPrivate) {
  set_color_scheme("default");

  QString theme_prefix = config::instance()->prefix();


  //QString theme_base_path = "Resources/icons/";
  //QString theme_pack_path = theme_prefix + "/" + theme_base_path;
  QString theme_pack_path = config::icon_resource_prefix_path().c_str();

  priv->m_resource_group = QDir::toNativeSeparators(theme_pack_path);
  priv->m_resource_name = a_theme_name;

  priv->m_current_style_ref =
      cherry_kit::extension_manager::instance()->style("cocoastyle");
}

resource_manager::~resource_manager() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

StylePtr resource_manager::default_desktop_style() {
  return priv->m_current_style_ref;
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

QString resource_manager::drawable_file_name(const QString &a_dpi, const QString &a_fileName)
{
  QString iconThemePath =
      QDir::toNativeSeparators(priv->m_resource_group + "/" +
                               priv->m_resource_name + "/" +
                               a_dpi + "/" + a_fileName);

  return iconThemePath;
}

QPixmap resource_manager::drawable(const QString &a_fileName,
                                   const QString &a_dpi) {
  QPixmap rv;

  QString iconThemePath = drawable_file_name(a_dpi, a_fileName);

  QFileInfo fileInfo(iconThemePath);

  if (priv->m_image_cache.keys().contains(iconThemePath)) {
    return priv->m_image_cache[iconThemePath];
  }

  if (!fileInfo.exists()) {
    qWarning() << Q_FUNC_INFO << "No file found: " << iconThemePath;
    return rv;
  }

  rv = QPixmap(iconThemePath);
  priv->m_image_cache[iconThemePath] = rv;

  return rv;
}

void resource_manager::load_default_color_values() {
  priv->m_color_map[kDarkPrimaryColor] = "#1976D2";
  priv->m_color_map[kPrimaryColor] = "#2196F3";
  priv->m_color_map[kLightPrimaryColor] = "#F0F0F0";
  priv->m_color_map[kTextBackground] = "#212121";
  priv->m_color_map[kAccentColor] = "#FF4081";
  priv->m_color_map[kTextColor] = "#646464";
  priv->m_color_map[kSecondryTextColor] = "#FFFFFF";
  priv->m_color_map[kDividerColor] = "#B6B6B6";
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

    priv->m_color_map[kDarkPrimaryColor] =
        a_object.property("dark_primary_color");
    priv->m_color_map[kPrimaryColor] =
        std::string(a_object.property("primary_color"));
    priv->m_color_map[kLightPrimaryColor] =
        a_object.property("light_primary_color");
    priv->m_color_map[kTextBackground] =
        a_object.property("text_background_color");
    priv->m_color_map[kAccentColor] =
        a_object.property("accent_color");
    priv->m_color_map[kTextColor] =
        a_object.property("text_color");
    priv->m_color_map[kSecondryTextColor] =
        a_object.property("secondry_text_color");
    priv->m_color_map[kDividerColor] =
        a_object.property("divider_color");
  });

  sync->find("color", "scheme_id", a_name);

  delete sync;
}

std::string resource_manager::color_scheme() const { return std::string(); }

const char *resource_manager::color_code(resource_manager::ColorName a_name) {
  std::string error_rv = "#000000";
  const char *rv;

  if (priv->m_color_map.find(a_name) ==
      priv->m_color_map.end()) {
    rv = error_rv.c_str();
    return rv;
  }

  rv = priv->m_color_map.at(a_name).c_str();

  return rv;
}

void resource_manager::set_theme_name(const QString &a_name) {
  Q_UNUSED(a_name);
}

std::string resource_manager::ThemepackLoaderPrivate::get_dpi() const {
   std::string rv = "mdpi";
   int base_dpi = 160;

   if (base_dpi <= 120) {
     rv = "ldpi";
   } else if (base_dpi <= 160) {
     rv = "mdpi";
   } else if (base_dpi <= 240) {
     rv = "hdpi";
   } else if (base_dpi <= 320) {
     rv = "xhdpi";
   }

   return rv;
}

} // namespace plexydesk
