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
#ifndef PLEXY_WIDGET_IMAGEPILE_H
#define PLEXY_WIDGET_IMAGEPILE_H

#include <plexy.h>

#include <view_controller.h>

#include <themepackloader.h>
#include "photowidget.h"
#include "imageitem.h"

class PhotoFrameController : public UI::ViewController {
  Q_OBJECT

public:
  PhotoFrameController(QObject *object = 0);
  virtual ~PhotoFrameController();

  void init();

  virtual void revokeSession(const QVariantMap &args);

  virtual void handleDropEvent(UI::UIWidget *widget, QDropEvent *event);

  virtual void setViewRect(const QRectF &rect);

  bool removeWidget(UI::UIWidget *widget);

  UI::ActionList actions() const;

  void requestAction(const QString &actionName, const QVariantMap &args);

  QString icon() const;

  void prepareRemoval();

private:
  PhotoWidget *mFrameParentitem;
  QList<PhotoWidget *> mPhotoList;
  QString mImageSource;
  UI::ActionList m_supported_action_list;
  QStringList m_current_url_list;
};

#endif
