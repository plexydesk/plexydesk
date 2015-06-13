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

#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#include <QObject>
#include <plexy.h>
#include <plexyconfig.h>
#include <plexydesk_ui_exports.h>

#include <functional>

namespace UIKit {
class DECL_UI_KIT_EXPORT ImageCache : public QObject {
  Q_OBJECT
public:
  ImageCache();
  virtual ~ImageCache();

  QPixmap pixmap(const QString &a_id, QSize *a_size_ptr,
                 const QSize &a_requested_size);

  bool is_cached(QString &a_filename) const;
  void add_to_cache(const QString &a_imgfile, const QString &a_filename,
                    const QString &a_themename);

  QPixmap get(const QString &a_name);

  virtual void on_loader_ready(std::function<void()> a_handler);

protected:
  void load(const QString &ta_themename);
  void clear();
  /*
  bool render_svg(QPainter *a_painter_ptr, QRectF a_rect, const QString &a_str,
                  const QString &a_elementId);
  */
Q_SIGNALS:
  void ready();

private:
  class Private;
  Private *const d;
};
}
#endif
