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

void SpacePreviewActivity::createWindow(const QRectF &window_geometry,
                                        const QString &window_title,
                                        const QPointF &window_pos)
{
  d->m_main_window = new UIKit::Window();

  d->m_main_window->setWindowFlag(UIKit::Widget::kRenderBackground);
  d->m_main_window->setWindowFlag(UIKit::Widget::kConvertToWindowType);
  d->m_main_window->setWindowFlag(UIKit::Widget::kRenderDropShadow);

  setGeometry(window_geometry);
  updateContentGeometry(d->m_main_window);

  exec(window_pos);

  d->m_main_window->onWindowDiscarded([this](UIKit::Window * aWindow) {
    discardActivity();
  });
}

QVariantMap SpacePreviewActivity::result() const { return QVariantMap(); }

void SpacePreviewActivity::updateAttribute(const QString &name,
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
  discardActivity();
}

void SpacePreviewActivity::onHideAnimationFinished() { Q_EMIT finished(); }
