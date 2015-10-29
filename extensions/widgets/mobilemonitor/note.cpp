#include "note.h"
#include <nativestyle.h>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include <ck_button.h>
#include <ck_text_editor.h>

class Note::PrivateNote {
public:
  PrivateNote() {}
  ~PrivateNote() {}

  QPixmap genShadowImage(const QRect &rect, const QPainterPath &path,
                         const QPixmap &pixmap);

  PlexyDesk::Style *mStyle;
  QString mName;
  QString mStatusMessage;
  QPixmap mPixmap;
  QString mID;
  QPixmap mAvatar;

  PlexyDesk::Button *mButton;
  PlexyDesk::TextEditor *mTextEdit;
};

Note::Note(const QRectF &rect, QGraphicsObject *parent)
    : PlexyDesk::DesktopWidget(rect, parent), d(new PrivateNote) {

  d->mTextEdit = new PlexyDesk::TextEditor(rect, this);
  d->mTextEdit->setPos(1.0, 24.0);
  d->mTextEdit->setStyle("background: rgba(0,0,0,0)");
}

Note::~Note() { delete d; }

void Note::setTitle(const QString &name) {
  d->mName = name;
  update();
}

void Note::setNoteContent(const QString &status) {
  d->mStatusMessage = status;
  update();
}

void Note::setID(const QString &id) { d->mID = id; }

QString Note::title() const { return d->mName; }

QString Note::id() { return d->mID; }

QString Note::noteContent() const { return d->mStatusMessage; }

void Note::setPixmap(const QPixmap &pixmap) {
  d->mPixmap = pixmap;
  update();
}

void Note::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
  painter->fillRect(option->exposedRect, QColor(250, 243, 78));
}

QPixmap Note::PrivateNote::genShadowImage(const QRect &rect,
                                          const QPainterPath &path,
                                          const QPixmap &pixmap) {
  QPixmap canvasSource(rect.size());

  QPainter painter;
  painter.begin(&canvasSource);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.fillRect(rect, Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.fillPath(path, QColor(220, 220, 220));
  painter.setClipPath(path);

  QPainterPath avatarPath;
  avatarPath.addRoundedRect(2.0, 2.0, rect.width() - 4.0, rect.height() - 4.0,
                            6.0, 6.0);
  painter.setClipPath(avatarPath);
  painter.drawPixmap(0.0, 0.0, rect.width(), rect.height(), pixmap);

  painter.setBrush(QBrush(Qt::white));
  painter.setPen(QPen(QBrush(Qt::white), 0.01));
  painter.setOpacity(0.20);
  painter.drawEllipse(1, -(rect.height() / 2), rect.width() * 2,
                      (rect.height()) - 2);
  painter.end();

  return canvasSource;
}

void Note::onClicked() { Q_EMIT clicked(this); }
