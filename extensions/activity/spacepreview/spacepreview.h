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
#ifndef SPACEPREVIEW_ACTIVITY_H
#define SPACEPREVIEW_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>

class SpacePreviewActivity : public UI::DesktopActivity
{
  Q_OBJECT

public:
  SpacePreviewActivity(QGraphicsObject *object = 0);

  virtual ~SpacePreviewActivity();

  void createWindow(const QRectF &window_geometry, const QString &window_title,
                    const QPointF &window_pos);

  QVariantMap result() const;

  virtual void updateAttribute(const QString &name, const QVariant &data);

  UI::UIWidget *window() const;

private Q_SLOTS:
  void onWidgetClosed(UI::UIWidget *widget);
  void onHideAnimationFinished();

private:
  class PrivateSpacePreview;
  PrivateSpacePreview *const d;
};

#endif
