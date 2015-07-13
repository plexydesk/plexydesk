#ifndef DEFAULT_TABLE_COMPONENT_H
#define DEFAULT_TABLE_COMPONENT_H

#include <abstractcellcomponent.h>
#include <plexydesk_ui_exports.h>

class DECL_UI_KIT_EXPORT DefaultTableComponent
    : public cherry_kit::TableViewItem {
  Q_OBJECT
public:
  typedef enum {
    kListLayout = 0,
    kGridLayout
  } LayoutType;

  DefaultTableComponent(const QRectF &a_rect, LayoutType = kListLayout,
                        QGraphicsItem *a_parent_ptr = 0);
  virtual ~DefaultTableComponent();

  QRectF boundingRect() const;
  virtual QSizeF sizeHint(Qt::SizeHint a_hint, const QSizeF &a_size) const;

  void set_selected();
  void clear_selection();
  void set_selected(bool a_selection);

  void set_label_visibility(bool a_visible);

  void set_label(const QString &a_txt);
  QString name() const;
  QString label() const;

  void set_icon(const QPixmap &a_pixmap);
  QPixmap icon();

  void set_data(const QPixmap &a_pixmap, const QString &a_label);

protected:
  virtual void paint(QPainter *a_painter_ptr,
                     const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget *a_widget_ptr = 0);
private
Q_SLOTS:
  void onClicked();

private:
  class PrivateTableComponent;
  PrivateTableComponent *const m_priv_ptr;
};

#endif
