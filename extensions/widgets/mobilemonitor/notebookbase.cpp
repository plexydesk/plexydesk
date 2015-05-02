#include "notebookbase.h"
#include "note.h"

#include <button.h>

class NoteBookBase::PrivateNoteBase {
public:
  PrivateNoteBase() {}
  ~PrivateNoteBase() {}

  QHash<QString, Note *> mNoteData;
};

NoteBookBase::NoteBookBase(const QRectF &rect, QGraphicsObject *parent)
    : ScrollWidget(rect, parent), d(new PrivateNoteBase) {}

NoteBookBase::~NoteBookBase() { delete d; }

void NoteBookBase::addNote(const QString &title, const QString &content) {

  Note *note = new Note(QRect(0.0, 0.0, 240.0, 240.0));
  note->setTitle(title);
  note->setNoteContent(content);

  if (scene()) {
    scene()->addItem(note);
    note->show();
  }

  d->mNoteData[title] = note;
}

void NoteBookBase::createNavBar() {}
