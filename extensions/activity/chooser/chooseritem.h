#ifndef CHOOSERITEM_H
#define CHOOSERITEM_H

#include <abstractcellcomponent.h>

class GridIcon : public UIKit::TableViewItem
{
  Q_OBJECT
public:
  typedef enum { List = 0, Grid } ItemLayout;

  GridIcon(const QRectF &rect, ItemLayout = List, QGraphicsItem *a_parent_ptr = 0);
  ~GridIcon();

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void set_selected();

  void clear_selection();

  void setLabel(const QString &txt);

  void setIcon(const QPixmap &pixmap);

  void setDefault(bool selection);

  QString label() const;

  void setItemProperties(const QPixmap &pixmap, const QString &label,
                         const QVariantMap &prop);

  void setItemProperty(const QString &key, const QVariant &value);

  QVariantMap itemProperties() const;

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private Q_SLOTS:
  void onClicked();

private:
  class PrivateChooserItem;
  PrivateChooserItem *const d;
};

#endif // CHOOSERITEM_H
