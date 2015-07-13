#ifndef FOLDERSPROVIDER_H
#define FOLDERSPROVIDER_H

#include <ck_table_model.h>
#include <ck_data_source.h>

class FolderItem;

class FolderProvider : public cherry_kit::TableModel {
  Q_OBJECT
public:
  FolderProvider(const QRectF &rect, QGraphicsObject *a_parent_ptr = 0);
  virtual ~FolderProvider();

  virtual bool init();

  QList<cherry_kit::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  cherry_kit::Widget *
  loadWidgetControlllerByName(const QString &controllerName) const;

  cherry_kit::DataSourcePtr loadDataSourceEngine(const QString &engine);

  cherry_kit::TableModel::TableRenderMode render_type() const;

  void setDirectoryPath(const QString &path);

Q_SIGNALS:
  void itemClicked(FolderItem *);
  void itemDoubleClicked(FolderItem *);

public
Q_SLOTS:
  void onDirectoryReady(const QString &path);
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<cherry_kit::TableViewItem *> mFolderList;

  class FolderProviderPrivate;
  FolderProviderPrivate *const d;
};

#endif // FOLDERSPROVIDER_H
