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
#include "takenote.h"
#include <widget.h>
#include <plexyconfig.h>
#include <imagebutton.h>
#include <QTimer>
#include <view_controller.h>
#include <QGraphicsLinearLayout>
#include <resource_manager.h>
#include <QGraphicsWidget>

class TakeNoteActivity::PrivateTakeNote {
public:
  PrivateTakeNote() {}

  ~PrivateTakeNote() {}

  CherryKit::Window *mFrame;
  CherryKit::Widget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;

  CherryKit::ImageButton *mAddNoteBtn;
  CherryKit::ImageButton *mAddTaskBtn;
  CherryKit::ImageButton *mAddReminderBtn;

  QString mSelection;
};

TakeNoteActivity::TakeNoteActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateTakeNote) {
  o_desktop_activity->mFrame = 0;
}

TakeNoteActivity::~TakeNoteActivity() { delete o_desktop_activity; }

void TakeNoteActivity::create_window(const QRectF &window_geometry,
                                     const QString &window_title,
                                     const QPointF &window_pos) {
  // todo: invoke UI
  o_desktop_activity->mFrame = new CherryKit::Window();
  o_desktop_activity->mFrame->setGeometry(window_geometry);
  o_desktop_activity->mFrame->setVisible(true);
  set_geometry(window_geometry);

  o_desktop_activity->mFrame->set_widget_name("Message Dialog");

  if (has_attribute("title")) {
  }

  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderBackground);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kConvertToWindowType);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderDropShadow);

  o_desktop_activity->mLayoutBase = new CherryKit::Widget(0);
  o_desktop_activity->mLayoutBase->setParentItem(o_desktop_activity->mFrame);

  o_desktop_activity->mLayout = new QGraphicsLinearLayout(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mLayoutBase->setGeometry(this->geometry());
  o_desktop_activity->mLayoutBase->moveBy(0.0, 64.0);

  o_desktop_activity->mAddNoteBtn = new CherryKit::ImageButton(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mAddNoteBtn->set_pixmap(CherryKit::ResourceManager::instance()->drawable(
      "pd_note_add_button_green.png", "hdpi"));

  o_desktop_activity->mAddTaskBtn = new CherryKit::ImageButton(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mAddTaskBtn->set_pixmap(CherryKit::ResourceManager::instance()->drawable(
      "pd_note_add_button_blue.png", "hdpi"));

  o_desktop_activity->mAddReminderBtn = new CherryKit::ImageButton(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mAddReminderBtn->set_pixmap(
      CherryKit::ResourceManager::instance()->drawable(
          "pd_note_add_button_orange.png", "hdpi"));

  o_desktop_activity->mLayout->addItem(o_desktop_activity->mAddNoteBtn);
  o_desktop_activity->mLayout->addItem(o_desktop_activity->mAddTaskBtn);
  o_desktop_activity->mLayout->addItem(o_desktop_activity->mAddReminderBtn);

  if (has_attribute("pos")) {
    QPoint point = attributes()["pos"].toPoint();
    exec(point);
  } else {
    exec(QCursor::pos());
  }

  // todo: invoke UI
  o_desktop_activity->mAddNoteBtn->set_lable(QLatin1String("Note"));
  o_desktop_activity->mAddReminderBtn->set_lable(QLatin1String("Reminder"));
  o_desktop_activity->mAddTaskBtn->set_lable(QLatin1String("Task"));

  connect(o_desktop_activity->mAddNoteBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(o_desktop_activity->mAddTaskBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(o_desktop_activity->mAddReminderBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
}

QRectF TakeNoteActivity::geometry() const {
  return QRectF(0.0, 0.0, 240.0, 160.0);
}

QVariantMap TakeNoteActivity::result() const {
  QVariantMap rv;
  rv["action"] = QVariant(o_desktop_activity->mSelection);
  return rv;
}

CherryKit::Window *TakeNoteActivity::window() const { return o_desktop_activity->mFrame; }

void TakeNoteActivity::cleanup() {
  if (o_desktop_activity->mFrame) {
    delete o_desktop_activity->mFrame;
  }
  o_desktop_activity->mFrame = 0;
}

void TakeNoteActivity::onWidgetClosed(CherryKit::Widget *widget) {
  discard_activity();
}

void TakeNoteActivity::onHideAnimationFinished() {}

void TakeNoteActivity::onClicked() {
  CherryKit::ImageButton *button =
      qobject_cast<CherryKit::ImageButton *>(sender());

  if (button) {
    o_desktop_activity->mSelection = button->label();
    notify_done();
  }
}
