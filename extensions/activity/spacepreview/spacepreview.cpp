/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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

class SpacePreviewActivity::PrivateSpacePreview
{
public:
  PrivateSpacePreview() {}
  ~PrivateSpacePreview() {}

  UIKit::Window *m_main_window;
};

SpacePreviewActivity::SpacePreviewActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateSpacePreview) {}

SpacePreviewActivity::~SpacePreviewActivity() { delete d; }

void SpacePreviewActivity::create_window(const QRectF &window_geometry,
                                        const QString &window_title,
                                        const QPointF &window_pos)
{
  d->m_main_window = new UIKit::Window();

  d->m_main_window->set_widget_flag(UIKit::Widget::kRenderBackground);
  d->m_main_window->set_widget_flag(UIKit::Widget::kConvertToWindowType);
  d->m_main_window->set_widget_flag(UIKit::Widget::kRenderDropShadow);

  set_geometry(window_geometry);
  update_content_geometry(d->m_main_window);

  exec(window_pos);

  d->m_main_window->on_window_discarded([this](UIKit::Window * aWindow) {
    discard_activity();
  });
}

QVariantMap SpacePreviewActivity::result() const { return QVariantMap(); }

void SpacePreviewActivity::update_attribute(const QString &name,
    const QVariant &data) {}

UIKit::Window *SpacePreviewActivity::window() const { return d->m_main_window; }

void SpacePreviewActivity::cleanup()
{
  if (d->m_main_window) {
    delete d->m_main_window;
  }

  d->m_main_window = 0;
}

void SpacePreviewActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discard_activity();
}

void SpacePreviewActivity::onHideAnimationFinished() { Q_EMIT finished(); }
