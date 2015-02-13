#ifndef DEFAULT_TABLE_COMPONENT_H
#define DEFAULT_TABLE_COMPONENT_H

#include <abstractcellcomponent.h>
#include <plexydesk_ui_exports.h>

class DECL_UI_KIT_EXPORT DefaultTableComponent
  : public UIKit::TableViewItem
{
  Q_OBJECT

public:
  typedef enum { kListLayout = 0, kGridLayout } LayoutType;

  DefaultTableComponent(const QRectF &rect, LayoutType = kListLayout,
                        QGraphicsItem *parent = 0);
  virtual ~DefaultTableComponent();

  QRectF boundingRect() const;

  virtual QSizeF sizeHint(Qt::SizeHint hint, const QSizeF &size) const;

  void setSelected();

  void setLabelVisibility(bool visible);

  void clearSelection();

  void setLabel(const QString &txt);

  void setIcon(const QPixmap &pixmap);

  QPixmap icon();

  void setSelected(bool selection);

  QString label() const;

  QString name();

  void setData(const QPixmap &pixmap, const QString &label);

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private Q_SLOTS:
  void onClicked();

private:
  class PrivateTableComponent;
  PrivateTableComponent *const d;
};

#endif
