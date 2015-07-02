#ifndef FOLDERSPROVIDER_H
#define FOLDERSPROVIDER_H

#include <tableviewcellinterface.h>
#include <datasource.h>

class FolderItem;

class FolderProvider : public CherryKit::TableModel {
  Q_OBJECT
public:
  FolderProvider(const QRectF &rect, QGraphicsObject *a_parent_ptr = 0);
  virtual ~FolderProvider();

  virtual bool init();

  QList<CherryKit::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  CherryKit::Widget *
  loadWidgetControlllerByName(const QString &controllerName) const;

  CherryKit::DataSourcePtr loadDataSourceEngine(const QString &engine);

  CherryKit::TableModel::TableRenderMode render_type() const;

  void setDirectoryPath(const QString &path);

Q_SIGNALS:
  void itemClicked(FolderItem *);
  void itemDoubleClicked(FolderItem *);

public
Q_SLOTS:
  void onDirectoryReady(const QString &path);
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<CherryKit::TableViewItem *> mFolderList;

  class FolderProviderPrivate;
  FolderProviderPrivate *const d;
};

#endif // FOLDERSPROVIDER_H
