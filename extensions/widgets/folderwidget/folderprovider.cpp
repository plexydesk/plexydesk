#include "folderprovider.h"
#include "folderitem.h"

#include <QFileSystemModel>

#include <extensionmanager.h>
#include <controllerinterface.h>

#include "plexydesktopiconprovider.h"

class FolderProvider::FolderProviderPrivate {
public:
  FolderProviderPrivate() {}
  ~FolderProviderPrivate() {
    if (mIconProvider)
      delete mIconProvider;
  }

  QMap<QString, PlexyDesk::ControllerPtr> mControllerMap;
  QMap<QString, PlexyDesk::DataSourcePtr> mEngines;
  QFileSystemModel *mFileSystemModel;
  QFileIconProvider *mIconProvider;

  QSizeF mCurrentCellSize;
};

FolderProvider::FolderProvider(const QRectF &rect, QGraphicsObject *parent)
    : PlexyDesk::TableModel(parent), d(new FolderProviderPrivate) {

  d->mIconProvider = new QFileIconProvider();
  d->mFileSystemModel = new QFileSystemModel(this);
  d->mFileSystemModel->setIconProvider(d->mIconProvider);
  connect(d->mFileSystemModel, SIGNAL(directoryLoaded(QString)), this,
          SLOT(onDirectoryReady(QString)));
}

FolderProvider::~FolderProvider() { delete d; }

bool FolderProvider::init() {
  // Q_EMIT updated()

  return true;
}

float FolderProvider::margin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float FolderProvider::padding() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float FolderProvider::leftMargin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float FolderProvider::rightMargin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

/*
PlexyDesk::DesktopWidget *FolderProvider::loadWidgetControlllerByName(const
QString &controllerName) const
{
    QSharedPointer<PlexyDesk::ControllerInterface> controller =
            (PlexyDesk::PluginLoader::getInstance()->controller(controllerName));

    if (!controller.data()) {
        qWarning() << Q_FUNC_INFO << "Error loading extension" <<
controllerName;
        return 0;
    }

    PlexyDesk::DesktopWidget *defaultView =
        qobject_cast<PlexyDesk::DesktopWidget*>(controller->init());

    if (!defaultView)
        return 0;

    d->mControllerMap[controllerName] = controller;
    defaultView->setWidgetFlag(PlexyDesk::DesktopWidget::BACKGROUND, false);
    defaultView->setWidgetFlag(PlexyDesk::DesktopWidget::WINDOW, false);

    return defaultView;
}
*/

PlexyDesk::DataSourcePtr FolderProvider::loadDataSourceEngine(
    const QString &engine) {
  QSharedPointer<PlexyDesk::DataSource> dataSource =
      PlexyDesk::ExtensionManager::instance()->dataEngine(engine);

  // connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));
  d->mEngines[engine] = dataSource;

  return dataSource;
}

PlexyDesk::TableModel::TableRenderMode FolderProvider::renderType() const {
  return PlexyDesk::TableModel::kRenderAsGridView;
}

void FolderProvider::setDirectoryPath(const QString &path) {
  d->mFileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::Files |
                                 QDir::Dirs);
  d->mFileSystemModel->setRootPath(path);
}

void FolderProvider::onDirectoryReady(const QString &path) {
  QModelIndex idx = d->mFileSystemModel->index(d->mFileSystemModel->rootPath());

  mFolderList.clear();
  for (int i = 0; i < d->mFileSystemModel->rowCount(idx); i++) {
    QModelIndex child = idx.child(i, 0);
    /// qDebug() << Q_FUNC_INFO << d->mFileSystemModel->fileName(child) << endl;
    FolderItem *item = new FolderItem(QRectF(0.0, 0.0, 64, 64), 0);
    item->setFileName(d->mFileSystemModel->fileName(child));
    item->setFileInfo((d->mFileSystemModel->fileInfo(child)));
    item->setFilePath(d->mFileSystemModel->filePath(child));
    item->setIcon(d->mFileSystemModel->fileIcon(child));
    mFolderList.append(item);
    connect(item, SIGNAL(clicked(FolderItem *)), this,
            SIGNAL(itemClicked(FolderItem *)));
    connect(item, SIGNAL(doubleClicked(FolderItem *)), this,
            SIGNAL(itemDoubleClicked(FolderItem *)));
  }

  Q_EMIT updated();
}

void FolderProvider::onLocationSourceUpdated(const QVariantMap &data) {

  Q_EMIT updated();
}
