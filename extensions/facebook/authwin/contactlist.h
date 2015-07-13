#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>

#include <scrollck_widget.h>
#include "contactlistitem.h"

class ContactList : public PlexyDesk::ScrollWidget {
  Q_OBJECT
public:
  ContactList(const QRectF &rect, QGraphicsObject *parent = 0);
  virtual ~ContactList();

  void addContact(const QString &id, const QString &contactName,
                  const QString &statusMessage = QString(),
                  const QPixmap &pixmap = QPixmap());

  void setStyle(StylePtr style);

Q_SIGNALS:
  void clicked(QString id);

public
Q_SLOTS:
  void clear();
  void filter(const QString &filterString);
  void onItemClicked(ContactListItem *item);

protected:
  virtual void paintFrontView(QPainter *painter, const QRectF &rect);

private:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  class PrivateContactList;
  PrivateContactList *const d;
};

#endif // CONTACTLIST_H
