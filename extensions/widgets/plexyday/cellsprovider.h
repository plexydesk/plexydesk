#ifndef CELLSPROVIDER_H
#define CELLSPROVIDER_H

#include <tableviewcellinterface.h>
#include <datasource.h>

class CellsProvider : public PlexyDesk::DelegateInterface {
  Q_OBJECT
public:
  CellsProvider(const QRectF &rect, QGraphicsObject *parent = 0);
  virtual ~CellsProvider();

  virtual bool init();

  QList<PlexyDesk::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  PlexyDesk::Widget *loadWidgetControlllerByName(const QString &controllerName)
      const;

  PlexyDesk::DataSourcePtr loadDataSourceEngine(const QString &engine);

public
Q_SLOTS:
  void onGeoLocationReady();
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<PlexyDesk::TableViewItem *> mCellList;

  class CellsProviderPrivate;
  CellsProviderPrivate *const d;
};

#endif // CELLSPROVIDER_H
