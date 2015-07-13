#ifndef CHOOSERCELLFACTORY_H
#define CHOOSERCELLFACTORY_H

#include <ck_table_model.h>

using namespace cherry_kit;

class ChooserActionDelegate : public table_model {
  Q_OBJECT

public:
  ChooserActionDelegate(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ChooserActionDelegate();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  virtual bool init();

  virtual TableRenderMode render_type() const;

  void removeAll();

  void addDataItem(const QString &label, const QPixmap pixmap,
                   bool selected = false,
                   const QVariantMap &properties = QVariantMap());

private:
  class PrivateChooserActionDelegate;
  PrivateChooserActionDelegate *const d;
};

#endif // CHOOSERCELLFACTORY_H
