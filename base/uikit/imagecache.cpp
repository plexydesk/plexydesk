/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : Siraj Razick <siraj@plexydesk.org>
*                 PhobosK <phobosk@kbfx.net>
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

#include "imagecache.h"

#include <QDebug>
#include <QObject>
#include <QDir>
#include <QPixmap>

namespace UIKit {

class ImageCache::Private {
public:
  Private() {}
  ~Private() {}
  QHash<QString, QString> fileHash;
  QList<std::function<void()> > m_load_handler;
};

void ImageCache::clear() { d->fileHash.clear(); }

ImageCache::ImageCache() : QObject(0), d(new Private) { load("default"); }

ImageCache::~ImageCache() { delete d; }

QPixmap ImageCache::pixmap(const QString &a_id, QSize *a_size_ptr,
                           const QSize &a_requested_size) {
  if (a_size_ptr->width() <= 0 && a_size_ptr->height() <= 0) {
    return get(a_id);
  }
  QPixmap rv = get(a_id).scaled(
      a_requested_size.width(), a_requested_size.height(),
      Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  return rv;
}

void ImageCache::load(const QString &ta_themename) {
  QString prefix = QDir::toNativeSeparators(
      Config::instance()->prefix() + QLatin1String("/share/plexy/themepack/") +
      ta_themename + QLatin1String("/resources/"));

  QDir dir(prefix);
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.size(); i++) {
    QFileInfo file = list.at(i);
    d->fileHash[file.completeBaseName()] = file.absoluteFilePath();
  }

  Q_EMIT ready();
}

void ImageCache::add_to_cache(const QString &imgfile, const QString &filename,
                              const QString &themename) {
  QString prefix = QDir::toNativeSeparators(
      Config::instance()->prefix() + QLatin1String("/share/plexy/themepack/") +
      themename + QLatin1String("/resources/"));

  QFileInfo file = prefix + imgfile;
  d->fileHash[filename] = file.absoluteFilePath();
}

QPixmap ImageCache::get(const QString &a_name) {
  return QPixmap(d->fileHash[a_name]);
}

void ImageCache::on_loader_ready(std::function<void()> a_handler) {
  d->m_load_handler.append(a_handler);
}

bool ImageCache::is_cached(QString &a_filename) const {
  if ((d->fileHash[a_filename]).isNull()) {
    return false;
  }

  return true;
}

/*
bool ImageCache::render_svg(QPainter *a_painter_ptr, QRectF a_rect,
                            const QString &a_str, const QString &a_elementId) {
  QString svgFile = d->fileHash[a_str];
  qDebug() << Q_FUNC_INFO << svgFile;
  QFileInfo fileInfo(svgFile);
  if (fileInfo.exists()) {
    d->render.load(svgFile);
    d->render.render(a_painter_ptr, a_elementId, a_rect);
    return true;
  }

  return false;
}
*/
} // namespace
