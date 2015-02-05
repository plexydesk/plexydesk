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
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <desktopwidget.h>
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
  UI::ActionList m_supported_action_list;
};

DesktopNotesControllerImpl::DesktopNotesControllerImpl(QObject *object)
  : UI::ViewController(object), d(new PrivateDesktopNotes)
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

UI::ActionList DesktopNotesControllerImpl::actions() const
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

void DesktopNotesControllerImpl::handleDropEvent(UI::UIWidget *widget,
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
  NoteWidget *note = new NoteWidget(0);
  note->resize(QSizeF(320, 320));
  note->setController(this);
  insert(note);
}

void DesktopNotesControllerImpl::createReminderUI()
{
  ReminderWidget *reminder = new ReminderWidget(0);
  reminder->setController(this);
  insert(reminder);
}
