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
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>

class SpacePreviewActivity::PrivateSpacePreview
{
public:
  PrivateSpacePreview() {}
  ~PrivateSpacePreview() {}

  UI::UIWidget *m_main_window;
};

SpacePreviewActivity::SpacePreviewActivity(QGraphicsObject *object)
  : UI::DesktopActivity(object), d(new PrivateSpacePreview) {}

SpacePreviewActivity::~SpacePreviewActivity() { delete d; }

void SpacePreviewActivity::createWindow(const QRectF &window_geometry,
                                        const QString &window_title,
                                        const QPointF &window_pos)
{
  d->m_main_window = new UI::UIWidget();

  d->m_main_window->setWindowFlag(UI::Window::kRenderBackground);
  d->m_main_window->setWindowFlag(UI::Window::kConvertToWindowType);
  d->m_main_window->setWindowFlag(UI::Window::kRenderDropShadow);

  setGeometry(window_geometry);
  updateContentGeometry(d->m_main_window);

  exec(window_pos);

  connect(d->m_main_window, SIGNAL(closed(UI::UIWidget *)), this,
          SLOT(onWidgetClosed(UI::UIWidget *)));
}

QVariantMap SpacePreviewActivity::result() const { return QVariantMap(); }

void SpacePreviewActivity::updateAttribute(const QString &name,
    const QVariant &data) {}

UI::UIWidget *SpacePreviewActivity::window() const { return d->m_main_window; }

void SpacePreviewActivity::onWidgetClosed(UI::UIWidget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void SpacePreviewActivity::onHideAnimationFinished() { Q_EMIT finished(); }
