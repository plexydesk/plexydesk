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
#include "desktopnotes.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <widget.h>
#include <QAction>

#include "reminderwidget.h"
#include "notewidget.h"

class DesktopNotesControllerImpl::PrivateDesktopNotes
{
public:
  PrivateDesktopNotes() {}
  ~PrivateDesktopNotes() {}
  QTimer *mDesktopNotes;
  QMap<QString, int> mNoteActions;
  UIKit::ActionList m_supported_action_list;
};

DesktopNotesControllerImpl::DesktopNotesControllerImpl(QObject *object)
  : UIKit::ViewController(object), d(new PrivateDesktopNotes)
{
  d->mNoteActions["Note"] = 1;
  d->mNoteActions["Task"] = 2;
  d->mNoteActions["Reminder"] = 3;
}

DesktopNotesControllerImpl::~DesktopNotesControllerImpl() { delete d; }

void DesktopNotesControllerImpl::init()
{
  // todo
  QAction *_add_note_action = new QAction(this);
  _add_note_action->setText(tr("Note"));

  _add_note_action->setProperty("id", QVariant(1));
  _add_note_action->setProperty("icon_name", "pd_add_note_frame_icon.png");

  d->m_supported_action_list << _add_note_action;
}

void DesktopNotesControllerImpl::revokeSession(const QVariantMap &args) {}

void DesktopNotesControllerImpl::setViewRect(const QRectF &rect) {}

UIKit::ActionList DesktopNotesControllerImpl::actions() const
{
  // return d->mNoteActions.keys();
  return d->m_supported_action_list;
}

void DesktopNotesControllerImpl::requestAction(const QString &actionName,
    const QVariantMap &args)
{
  switch (d->mNoteActions[actionName]) {
  case 1:
    createNoteUI();
    break;
  case 2:
    break;
  case 3:
    createReminderUI();
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Action";
  }
}

void DesktopNotesControllerImpl::handleDropEvent(UIKit::Widget *widget,
    QDropEvent *event)
{
  const QString droppedFile = event->mimeData()->urls().value(0).toLocalFile();
  QFileInfo fileInfo(droppedFile);

  if (fileInfo.isFile()) {
    QPixmap image(droppedFile);
    NoteWidget *note = qobject_cast<NoteWidget *>(widget);
    if (note) {
      note->setPixmap(image);
    }
  }
}

QString DesktopNotesControllerImpl::icon() const
{
  return QString("pd_add_note_frame_icon.png");
}

void DesktopNotesControllerImpl::onDataUpdated(const QVariantMap &data) {}

void DesktopNotesControllerImpl::createNoteUI()
{
  UIKit::Window *window = new UIKit::Window();

  NoteWidget *note = new NoteWidget(window);
  note->resize(QSizeF(320, 320));
  note->setController(this);
  note->setViewport(viewport());

  window->setGeometry(note->geometry());
  window->set_window_title("Note");
  window->set_window_content(note);

  insert(window);

  window->on_window_discarded([this](UIKit::Window * aWindow) {
    delete aWindow;
  });
}

void DesktopNotesControllerImpl::createReminderUI()
{
  UIKit::Window *window = new UIKit::Window();

  ReminderWidget *reminder = new ReminderWidget(window);
  reminder->setController(this);

  window->setGeometry(reminder->geometry());
  window->set_window_title("Reminder");
  window->set_window_content(reminder);

  insert(window);

  window->on_window_discarded([this](UIKit::Window * aWindow) {
    delete aWindow;
  });
}
