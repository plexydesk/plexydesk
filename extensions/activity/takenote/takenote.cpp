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

class note_dialog::PrivateTakeNote {
public:
  PrivateTakeNote() {}

  ~PrivateTakeNote() {}

  cherry_kit::window *mFrame;
  cherry_kit::widget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;

  cherry_kit::ImageButton *mAddNoteBtn;
  cherry_kit::ImageButton *mAddTaskBtn;
  cherry_kit::ImageButton *mAddReminderBtn;

  QString mSelection;
};

note_dialog::note_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateTakeNote) {
  o_desktop_dialog->mFrame = 0;
}

note_dialog::~note_dialog() { delete o_desktop_dialog; }

void note_dialog::create_window(const QRectF &window_geometry,
                                     const QString &window_title,
                                     const QPointF &window_pos) {
  // todo: invoke UI
  o_desktop_dialog->mFrame = new cherry_kit::window();
  o_desktop_dialog->mFrame->setGeometry(window_geometry);
  o_desktop_dialog->mFrame->setVisible(true);
  set_geometry(window_geometry);

  o_desktop_dialog->mFrame->set_widget_name("Message Dialog");

  if (has_attribute("title")) {
  }

  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  o_desktop_dialog->mLayoutBase = new cherry_kit::widget(0);
  o_desktop_dialog->mLayoutBase->setParentItem(o_desktop_dialog->mFrame);

  o_desktop_dialog->mLayout =
      new QGraphicsLinearLayout(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mLayoutBase->setGeometry(this->geometry());
  o_desktop_dialog->mLayoutBase->moveBy(0.0, 64.0);

  o_desktop_dialog->mAddNoteBtn =
      new cherry_kit::ImageButton(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mAddNoteBtn->set_pixmap(
      cherry_kit::resource_manager::instance()->drawable(
          "pd_note_add_button_green.png", "hdpi"));

  o_desktop_dialog->mAddTaskBtn =
      new cherry_kit::ImageButton(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mAddTaskBtn->set_pixmap(
      cherry_kit::resource_manager::instance()->drawable(
          "pd_note_add_button_blue.png", "hdpi"));

  o_desktop_dialog->mAddReminderBtn =
      new cherry_kit::ImageButton(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mAddReminderBtn->set_pixmap(
      cherry_kit::resource_manager::instance()->drawable(
          "pd_note_add_button_orange.png", "hdpi"));

  o_desktop_dialog->mLayout->addItem(o_desktop_dialog->mAddNoteBtn);
  o_desktop_dialog->mLayout->addItem(o_desktop_dialog->mAddTaskBtn);
  o_desktop_dialog->mLayout->addItem(o_desktop_dialog->mAddReminderBtn);

  if (has_attribute("pos")) {
    QPoint point = attributes()["pos"].toPoint();
    exec(point);
  } else {
    exec(QCursor::pos());
  }

  // todo: invoke UI
  o_desktop_dialog->mAddNoteBtn->set_lable(QLatin1String("Note"));
  o_desktop_dialog->mAddReminderBtn->set_lable(QLatin1String("Reminder"));
  o_desktop_dialog->mAddTaskBtn->set_lable(QLatin1String("Task"));

  connect(o_desktop_dialog->mAddNoteBtn, SIGNAL(clicked()), this,
          SLOT(onClicked()));
  connect(o_desktop_dialog->mAddTaskBtn, SIGNAL(clicked()), this,
          SLOT(onClicked()));
  connect(o_desktop_dialog->mAddReminderBtn, SIGNAL(clicked()), this,
          SLOT(onClicked()));
}

QRectF note_dialog::geometry() const {
  return QRectF(0.0, 0.0, 240.0, 160.0);
}

QVariantMap note_dialog::result() const {
  QVariantMap rv;
  rv["action"] = QVariant(o_desktop_dialog->mSelection);
  return rv;
}

cherry_kit::window *note_dialog::activity_window() const {
  return o_desktop_dialog->mFrame;
}

void note_dialog::cleanup() {
  if (o_desktop_dialog->mFrame) {
    delete o_desktop_dialog->mFrame;
  }
  o_desktop_dialog->mFrame = 0;
}

void note_dialog::onWidgetClosed(cherry_kit::widget *widget) {
  discard_activity();
}

void note_dialog::onHideAnimationFinished() {}

void note_dialog::onClicked() {
  cherry_kit::ImageButton *button =
      qobject_cast<cherry_kit::ImageButton *>(sender());

  if (button) {
    o_desktop_dialog->mSelection = button->label();
    notify_done();
  }
}
