#ifndef FOLDERSPROVIDER_H
#define FOLDERSPROVIDER_H

#include <tableviewcellinterface.h>
#include <datasource.h>

class FolderItem;

class FolderProvider : public UIKit::TableModel {
  Q_OBJECT
public:
  FolderProvider(const QRectF &rect, QGraphicsObject *a_parent_ptr = 0);
  virtual ~FolderProvider();

  virtual bool init();

  QList<UIKit::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  UIKit::Widget *loadWidgetControlllerByName(const QString &controllerName)
      const;

  UIKit::DataSourcePtr loadDataSourceEngine(const QString &engine);

  UIKit::TableModel::TableRenderMode render_type() const;

  void setDirectoryPath(const QString &path);

Q_SIGNALS:
  void itemClicked(FolderItem *);
  void itemDoubleClicked(FolderItem *);

public
Q_SLOTS:
  void onDirectoryReady(const QString &path);
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<UIKit::TableViewItem *> mFolderList;

  class FolderProviderPrivate;
  FolderProviderPrivate *const d;
};

#endif // FOLDERSPROVIDER_H
