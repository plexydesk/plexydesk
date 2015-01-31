#ifndef CHOOSERCELLFACTORY_H
#define CHOOSERCELLFACTORY_H

#include <tableviewcellinterface.h>

using namespace PlexyDesk;

class ChooserActionDelegate : public TableModel {
  Q_OBJECT

public:
  ChooserActionDelegate(QGraphicsObject *parent = 0);
  virtual ~ChooserActionDelegate();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  virtual bool init();

  virtual TableRenderMode renderType() const;

  void removeAll();

  void addDataItem(const QString &label, const QPixmap pixmap,
                   bool selected = false,
                   const QVariantMap &properties = QVariantMap());

private:
  class PrivateChooserActionDelegate;
  PrivateChooserActionDelegate *const d;
};

#endif // CHOOSERCELLFACTORY_H
