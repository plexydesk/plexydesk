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
#include "takenote.h"
#include <widget.h>
#include <plexyconfig.h>
#include <imagebutton.h>
#include <QTimer>
#include <view_controller.h>
#include <QGraphicsLinearLayout>
#include <themepackloader.h>
#include <QGraphicsWidget>

class TakeNoteActivity::PrivateTakeNote
{
public:
  PrivateTakeNote() {}

  ~PrivateTakeNote() {}

  UI::Window *mFrame;
  QGraphicsWidget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;

  UI::ImageButton *mAddNoteBtn;
  UI::ImageButton *mAddTaskBtn;
  UI::ImageButton *mAddReminderBtn;

  QString mSelection;
};

TakeNoteActivity::TakeNoteActivity(QGraphicsObject *object)
  : UI::DesktopActivity(object), d(new PrivateTakeNote)
{
  d->mFrame = 0;
}

TakeNoteActivity::~TakeNoteActivity() { delete d; }

void TakeNoteActivity::createWindow(const QRectF &window_geometry,
                                    const QString &window_title,
                                    const QPointF &window_pos)
{
  // todo: invoke UI
  d->mFrame = new UI::Window();
  d->mFrame->setGeometry(window_geometry);
  d->mFrame->setVisible(true);
  setGeometry(window_geometry);

  d->mFrame->setLabelName("Message Dialog");

  if (hasAttribute("title")) {
  }

  d->mFrame->setWindowFlag(UI::Window::kRenderBackground);
  d->mFrame->setWindowFlag(UI::Window::kConvertToWindowType);
  d->mFrame->setWindowFlag(UI::Window::kRenderDropShadow);

  d->mLayoutBase = new QGraphicsWidget(0);
  d->mLayoutBase->setParentItem(d->mFrame);

  d->mLayout = new QGraphicsLinearLayout(d->mLayoutBase);
  d->mLayoutBase->setGeometry(this->geometry());
  d->mLayoutBase->moveBy(0.0, 64.0);

  d->mAddNoteBtn = new UI::ImageButton(d->mLayoutBase);
  d->mAddNoteBtn->setPixmap(UI::Theme::instance()->drawable(
                              "pd_note_add_button_green.png", "hdpi"));

  d->mAddTaskBtn = new UI::ImageButton(d->mLayoutBase);
  d->mAddTaskBtn->setPixmap(UI::Theme::instance()->drawable(
                              "pd_note_add_button_blue.png", "hdpi"));

  d->mAddReminderBtn = new UI::ImageButton(d->mLayoutBase);
  d->mAddReminderBtn->setPixmap(UI::Theme::instance()->drawable(
                                  "pd_note_add_button_orange.png", "hdpi"));

  d->mLayout->addItem(d->mAddNoteBtn);
  d->mLayout->addItem(d->mAddTaskBtn);
  d->mLayout->addItem(d->mAddReminderBtn);

  if (hasAttribute("pos")) {
    QPoint point = attributes()["pos"].toPoint();
    exec(point);
  } else {
    exec(QCursor::pos());
  }

  connect(d->mFrame, SIGNAL(closed(UI::Window *)), this,
          SLOT(onWidgetClosed(UI::Window *)));
  // todo: invoke UI

  d->mAddNoteBtn->setLable(QLatin1String("Note"));
  d->mAddReminderBtn->setLable(QLatin1String("Reminder"));
  d->mAddTaskBtn->setLable(QLatin1String("Task"));

  connect(d->mAddNoteBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(d->mAddTaskBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(d->mAddReminderBtn, SIGNAL(clicked()), this, SLOT(onClicked()));
}

QRectF TakeNoteActivity::geometry() const
{
  return QRectF(0.0, 0.0, 240.0, 160.0);
}

QVariantMap TakeNoteActivity::result() const
{
  QVariantMap rv;
  rv["action"] = QVariant(d->mSelection);
  return rv;
}

UI::Window *TakeNoteActivity::window() const { return d->mFrame; }

void TakeNoteActivity::onWidgetClosed(UI::Window *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void TakeNoteActivity::onHideAnimationFinished() { Q_EMIT finished(); }

void TakeNoteActivity::onClicked()
{
  UI::ImageButton *button =
    qobject_cast<UI::ImageButton *>(sender());

  if (button) {
    d->mSelection = button->label();
    Q_EMIT finished();
  }
}
