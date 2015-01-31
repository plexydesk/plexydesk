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

namespace PlexyDesk {

Theme *Theme::s_theme_instance = 0;

class Theme::ThemepackLoaderPrivate {
public:
  ThemepackLoaderPrivate() {}
  ~ThemepackLoaderPrivate() {
    if (mXmlRawFile)
      delete mXmlRawFile;

    if (mStyle)
      mStyle.clear();
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
    : QObject(parent), d(new ThemepackLoaderPrivate) {
  d->mThemePackPath = QDir::toNativeSeparators(
      QString("%1/%2").arg(PlexyDesk::Config::getInstance()->prefix()).arg(
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

  d->mStyle = PlexyDesk::ExtensionManager::instance()->style("cocoastyle");
}

Theme::~Theme() {

  /*
  if (staticLoader) {
      delete staticLoader;
      staticLoader = 0;
  }
  */

  delete d;
}

StylePtr Theme::defaultDesktopStyle() { return d->mStyle; }

StylePtr Theme::style() { return instance()->defaultDesktopStyle(); }

Theme *Theme::instance() {
  if (!s_theme_instance) {
    s_theme_instance = new Theme("default", 0);
    return s_theme_instance;
  } else {
    return s_theme_instance;
  }
}

QPixmap Theme::icon(const QString &name, const QString &resolution) {
  return instance()->drawable(name, resolution);
}

QString Theme::desktopSessionData() const {
  QString homePath = QDir::homePath();
  QString result;

#ifdef Q_WS_MAC
  homePath.append("/Library/Containers/org.plexydesk.desktop/session.xml");
#else
  homePath.append("/.plexydesk/session.xml");
#endif

  QFileInfo info(homePath);

  if (info.exists()) {
    QFile sessionFile(homePath);

    if (sessionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

      while (!sessionFile.atEnd()) {
        QByteArray line = sessionFile.readLine();
        result.append(line);
      }
    }
  }

  return result;
}

void Theme::commitSessionData(const QString &data) {
  QString homePath =
      QDir::toNativeSeparators(QDir::homePath() + "/.plexydesk/");
  QFileInfo fileInfo(homePath);

  if (!fileInfo.exists()) {
    QDir::home().mkpath(homePath);
  }
  QFile file(
      QDir::toNativeSeparators(QDir::homePath() + "/.plexydesk/session.xml"));

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << Q_FUNC_INFO
             << "Failed to open the file : " << file.errorString();
    return;
  }

  QTextStream out(&file);
  out << data;

  file.close();
}

QPixmap Theme::drawable(const QString &fileName, const QString &resoution) {
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

void Theme::setThemeName(const QString &name) { Q_UNUSED(name); }

QStringList Theme::desktopWidgets() const {
  QStringList rv;
  if (!d->mXmlDocumentRoot.hasChildNodes())
    return rv;

  QDomNodeList widgetNodeList =
      d->mXmlDocumentRoot.documentElement().elementsByTagName("widget");

  for (int index = 0; index < widgetNodeList.count(); index++) {
    QDomElement widgetElement = widgetNodeList.at(index).toElement();

    QString currentWidgetStringName = widgetElement.attribute("name");
    rv.append(currentWidgetStringName);
  }

  return rv;
}

QString Theme::desktopBackgroundController() const {
  QString rv;
  if (!d->mXmlDocumentRoot.hasChildNodes())
    return rv;

  QDomNodeList widgetNodeList =
      d->mXmlDocumentRoot.documentElement().elementsByTagName("background");

  if (widgetNodeList.count() <= 0)
    return rv;

  QDomElement widgetElement = widgetNodeList.at(0).toElement();

  rv = widgetElement.attribute("name");
  return rv;
}

// TODO:
// Refactor these two methods
QRectF Theme::widgetPos(const QString &name, const QRectF &screen_rect) {
  QDomNodeList widgetNodeList =
      d->mXmlDocumentRoot.documentElement().elementsByTagName("widget");
  QRectF rect;

  for (int index = 0; index < widgetNodeList.count(); index++) {
    QDomElement widgetElement = widgetNodeList.at(index).toElement();

    if (widgetElement.attribute("name") != name)
      continue;

    if (widgetElement.hasChildNodes()) {
      QDomElement rectElement = widgetElement.firstChildElement("rect");

      QDomAttr x = rectElement.attributeNode("x");
      QDomAttr y = rectElement.attributeNode("y");

      QDomAttr widthAttr = rectElement.attributeNode("width");
      QDomAttr heightAttr = rectElement.attributeNode("height");

      QString widthString = widthAttr.value();
      QString heightString = heightAttr.value();

      float width = 0.0f;
      float height = 0.0f;
      float x_coord = 0.0f;
      float y_coord = 0.0f;

      if (widthString.contains("%")) {
        width = toScreenValue(widthString, screen_rect.width());
      } else
        x_coord = x.value().toFloat();

      if (heightString.contains("%")) {
        height = toScreenValue(heightString, screen_rect.height());
      } else
        y_coord = x.value().toFloat();

      if (x.value().contains("%")) {
        x_coord = toScreenValue(x.value(), screen_rect.width());
      }

      if (y.value().contains("%")) {
        y_coord = toScreenValue(y.value(), screen_rect.height());
      }

      if (widthString == "device-width") {
        width = screen_rect.width();
      }

      if (heightString == "device-height") {
        height = screen_rect.height();
      }

      rect = QRectF(x_coord + screen_rect.x(), y_coord + screen_rect.y(), width,
                    height);
    }
  }

  return rect;
}

QRectF Theme::backgroundPos(const QString &name, const QRectF &screen_rect) {
  QDomNodeList widgetNodeList =
      d->mXmlDocumentRoot.documentElement().elementsByTagName("background");
  QRectF rect;

  for (int index = 0; index < widgetNodeList.count(); index++) {
    QDomElement widgetElement = widgetNodeList.at(index).toElement();

    if (widgetElement.attribute("name") != name)
      continue;

    if (widgetElement.hasChildNodes()) {
      QDomElement rectElement = widgetElement.firstChildElement("rect");

      QDomAttr x = rectElement.attributeNode("x");
      QDomAttr y = rectElement.attributeNode("y");

      QDomAttr widthAttr = rectElement.attributeNode("width");
      QDomAttr heightAttr = rectElement.attributeNode("height");

      QString widthString = widthAttr.value();
      QString heightString = heightAttr.value();

      float width = 0.0f;
      float height = 0.0f;
      float x_coord = 0.0f;
      float y_coord = 0.0f;

      if (widthString.contains("%")) {
        width = toScreenValue(widthString, screen_rect.width());
      } else
        x_coord = x.value().toFloat();

      if (heightString.contains("%")) {
        height = toScreenValue(heightString, screen_rect.height());
      } else
        y_coord = x.value().toFloat();

      if (x.value().contains("%")) {
        x_coord = toScreenValue(x.value(), screen_rect.width());
      }

      if (y.value().contains("%")) {
        y_coord = toScreenValue(y.value(), screen_rect.height());
      }

      if (widthString == "device-width") {
        width = screen_rect.width();
      }

      if (heightString == "device-height") {
        height = screen_rect.height();
      }

      rect = QRectF(x_coord + screen_rect.x(), y_coord + screen_rect.y(), width,
                    height);
    }
  }

  return rect;
}

int Theme::toScreenValue(const QString &val, int max_distance) {
  QRegExp rx("(\\d+)");

  rx.indexIn(val, 0);
  int _x = rx.cap(1).toUInt();
  _x = (max_distance / 100) * _x;

  return _x;
}

void Theme::requestNoteSideImageFromWebService(const QString &key) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = key;
  args["per_page"] = QString::number(1);
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers,banners";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void Theme::requestPhotoSizes(const QString &photoID) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onSizeServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void Theme::onServiceCompleteJson(QuetzalSocialKit::WebService *service) {
  QList<QVariantMap> photoList = service->methodData("photo");

  Q_FOREACH(const QVariantMap & map, photoList) {
    requestPhotoSizes(map["id"].toString());
  }

  service->deleteLater();
  ;
}

void Theme::onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service) {
  Q_FOREACH(const QVariantMap & map, service->methodData("size")) {
    if (map["label"].toString() == "Large" ||
        map["label"].toString() == "Large 1600" ||
        map["label"].toString() == "Original") {
      qDebug() << Q_FUNC_INFO << map["label"].toString() << "->"
               << map["source"].toString();
      QuetzalSocialKit::AsyncDataDownloader *downloader =
          new QuetzalSocialKit::AsyncDataDownloader(this);

      QVariantMap metaData;
      metaData["method"] = service->methodName();
      metaData["id"] =
          service->inputArgumentForMethod(service->methodName())["photo_id"];
      metaData["data"] = service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }
  }

  service->deleteLater();
}

void Theme::onDownloadCompleteJson(QuetzalSocialKit::WebService *service) {}

void Theme::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReadyJson()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), PlexyDesk::Config::cacheDir(), true);
    imageSave->setCrop(QRectF(0.0, 0.0, 440, 440.0));
    imageSave->start();

    downloader->deleteLater();
  }
}

void Theme::onImageSaveReadyJson() {
  qDebug() << Q_FUNC_INFO;
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    // d->mBackgroundPixmap = c->image();
    Q_EMIT imageSearchDone(c->image());
    c->quit();
    c->deleteLater();
  }
}

void Theme::onImageReadyJson(const QString &fileName) {
  qDebug() << Q_FUNC_INFO << fileName;
}

} // namespace plexydesk
