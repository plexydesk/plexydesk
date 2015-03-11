#ifndef PHOTO_CEL_H
#define PHOTO_CEL_H

#include <abstractcellcomponent.h>

class PhotoCell : public UIKit::TableViewItem
{
  Q_OBJECT
public:
  typedef enum { List = 0, Grid } ItemLayout;

  PhotoCell(const QRectF &rect, ItemLayout = List, QGraphicsItem *a_parent_ptr = 0);
  virtual ~PhotoCell();

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void set_selected();

  void setLabelVisibility(bool visible);

  void clear_selection();

  void setLabel(const QString &txt);

  void setIcon(const QPixmap &pixmap);

  void setDefault(bool selection);

  QString label() const;

  QVariantMap metaData() const;

  void addDataItem(const QImage &img, const QString &label,
                   const QVariantMap &metaData = QVariantMap());

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
  void onClicked();

private:
  class PrivatePhotoCell;
  PrivatePhotoCell *const d;
};

#endif // PHOTO_CEL_H
