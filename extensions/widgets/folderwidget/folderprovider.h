#ifndef FOLDERSPROVIDER_H
#define FOLDERSPROVIDER_H

#include <tableviewcellinterface.h>
#include <datasource.h>

class FolderItem;

class FolderProvider : public PlexyDesk::TableModel {
  Q_OBJECT
public:
  FolderProvider(const QRectF &rect, QGraphicsObject *parent = 0);
  virtual ~FolderProvider();

  virtual bool init();

  QList<PlexyDesk::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  PlexyDesk::UIWidget *loadWidgetControlllerByName(
      const QString &controllerName) const;

  PlexyDesk::DataSourcePtr loadDataSourceEngine(const QString &engine);

  PlexyDesk::TableModel::TableRenderMode renderType() const;

  void setDirectoryPath(const QString &path);

Q_SIGNALS:
  void itemClicked(FolderItem *);
  void itemDoubleClicked(FolderItem *);

public Q_SLOTS:
  void onDirectoryReady(const QString &path);
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<PlexyDesk::TableViewItem *> mFolderList;

  class FolderProviderPrivate;
  FolderProviderPrivate *const d;
};

#endif // FOLDERSPROVIDER_H
