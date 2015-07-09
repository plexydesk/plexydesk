/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "spacepreview.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>

class SpacePreviewActivity::PrivateSpacePreview {
public:
  PrivateSpacePreview() {}
  ~PrivateSpacePreview() {}

  CherryKit::Window *m_main_window;
};

SpacePreviewActivity::SpacePreviewActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateSpacePreview) {}

SpacePreviewActivity::~SpacePreviewActivity() { delete o_desktop_activity; }

void SpacePreviewActivity::create_window(const QRectF &window_geometry,
                                         const QString &window_title,
                                         const QPointF &window_pos) {
  o_desktop_activity->m_main_window = new CherryKit::Window();
  o_desktop_activity->m_main_window->set_window_type(CherryKit::Window::kPopupWindow);

  set_geometry(window_geometry);
  update_content_geometry(o_desktop_activity->m_main_window);

  exec(window_pos);
}

QVariantMap SpacePreviewActivity::result() const { return QVariantMap(); }

void SpacePreviewActivity::update_attribute(const QString &name,
                                            const QVariant &data) {}

CherryKit::Window *SpacePreviewActivity::window() const {
  return o_desktop_activity->m_main_window;
}

void SpacePreviewActivity::cleanup() {
  if (o_desktop_activity->m_main_window) {
    delete o_desktop_activity->m_main_window;
  }

  o_desktop_activity->m_main_window = 0;
}
