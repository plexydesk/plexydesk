#ifndef IMAGE_CEL_H
#define IMAGE_CEL_H

#include <abstractcellcomponent.h>

class ImageCell : public UI::TableViewItem {
  Q_OBJECT
public:
  typedef enum { List = 0, Grid } ItemLayout;

  ImageCell(const QRectF &rect, ItemLayout = List, QGraphicsItem *parent = 0);
  virtual ~ImageCell();

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void setSelected();

  void setLabelVisibility(bool visible);

  void clearSelection();

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
  class PrivateImageCell;
  PrivateImageCell *const d;
};

#endif // IMAGE_CEL_H
