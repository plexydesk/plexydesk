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
#ifndef INPUTDIALOGACTIVITY_DATA_H
#define INPUTDIALOGACTIVITY_DATA_H

#include <QtCore>

#include <ck_data_source.h>
#include <QtNetwork>
#include <ck_desktop_dialog.h>
#include <ck_widget.h>
#include <ck_window.h>

class text_input_dialog : public cherry_kit::desktop_dialog {
  Q_OBJECT

public:
  text_input_dialog(QGraphicsObject *object = 0);
  virtual ~text_input_dialog();

  void create_window(const QRectF &window_geometry, const QString &window_title,
                     const QPointF &window_pos);

  virtual QString error_message() const;

  virtual QVariantMap activityResult() const;
  virtual QRectF geometry() const;

  void containsArg();

  QVariantMap result() const;

  cherry_kit::window *dialog_window() const;
  void purge();

private
Q_SLOTS:
  void onWidgetClosed(cherry_kit::widget *widget);
  void onMotionAnimFinished();
  void onOkButtonPressed();

private:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

  class PrivateInputDialogActivity;
  PrivateInputDialogActivity *const priv;
};

#endif
