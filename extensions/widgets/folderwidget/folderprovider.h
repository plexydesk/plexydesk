#ifndef FOLDERSPROVIDER_H
#define FOLDERSPROVIDER_H

#include <tableviewcellinterface.h>
#include <datasource.h>

class FolderItem;

class FolderProvider : public UI::TableModel
{
  Q_OBJECT
public:
  FolderProvider(const QRectF &rect, QGraphicsObject *parent = 0);
  virtual ~FolderProvider();

  virtual bool init();

  QList<UI::TableViewItem *> componentList();

  float margin() const;

  float padding() const;

  virtual float leftMargin() const;

  virtual float rightMargin() const;

  UI::UIWidget *loadWidgetControlllerByName(
    const QString &controllerName) const;

  UI::DataSourcePtr loadDataSourceEngine(const QString &engine);

  UI::TableModel::TableRenderMode renderType() const;

  void setDirectoryPath(const QString &path);

Q_SIGNALS:
  void itemClicked(FolderItem *);
  void itemDoubleClicked(FolderItem *);

public Q_SLOTS:
  void onDirectoryReady(const QString &path);
  void onLocationSourceUpdated(const QVariantMap &data);

private:
  QList<UI::TableViewItem *> mFolderList;

  class FolderProviderPrivate;
  FolderProviderPrivate *const d;
};

#endif // FOLDERSPROVIDER_H
