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
#ifndef PHOTOSEARCH_ACTIVITY_H
#define PHOTOSEARCH_ACTIVITY_H

#include <QtCore>

#include <ck_window.h>
#include <ck_data_source.h>
#include <QtNetwork>
#include <ck_desktop_dialog.h>
#include <ck_table_view_item.h>

using namespace cherry_kit;
class desktop_settings_dialog : public cherry_kit::desktop_dialog {
  Q_OBJECT

public:
  desktop_settings_dialog(QGraphicsObject *object = 0);
  virtual ~desktop_settings_dialog();

  void create_window(const QRectF &a_window_geometry,
                     const QString &a_window_title, const QPointF &window_pos);

  virtual QRectF geometry() const;

  QVariantMap result() const;

  window *dialog_window() const;
  void cleanup();

  void insert_image_to_grid(const QImage &ck_preview_pixmap) const;

private Q_SLOTS:
  void load_images() const;
  void load_from_system_path() const;

private:
  class PrivatePhotoSearch;
  PrivatePhotoSearch *const d;
};

#endif
