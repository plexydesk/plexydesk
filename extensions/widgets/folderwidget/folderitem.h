#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include <plexy.h>

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <abstractcellcomponent.h>
#include <QFileInfo>
#include <QIcon>

class FolderItem : public PlexyDesk::TableViewItem {
  Q_OBJECT
public:
  explicit FolderItem(const QRectF &rect, QGraphicsItem *parent = 0);

  QRectF boundingRect() const;
  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void setIcon(const QIcon &icon);
  void setFileName(const QString &fileName);
  void setFilePath(const QString &path);
  void setFileInfo(const QFileInfo &info);
  QFileInfo fileInfo() const;
  QIcon icon() const;

  void setSelected();
  void clearSelection();

Q_SIGNALS:
  void clicked(FolderItem *);
  void doubleClicked(FolderItem *);

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
  class PrivateFolderItem;
  PrivateFolderItem *const d;
};

#endif // FOLDERITEM_H
