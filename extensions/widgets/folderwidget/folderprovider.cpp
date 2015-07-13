#include "folderprovider.h"
#include "folderitem.h"

#include <QFileSystemModel>

#include <ck_extension_manager.h>
#include <ck_desktop_controller_interface.h>

#include "plexydesktopiconprovider.h"

class FolderProvider::FolderProviderPrivate {
public:
  FolderProviderPrivate() {}
  ~FolderProviderPrivate() {
    if (mIconProvider) {
      delete mIconProvider;
    }
  }

  QMap<QString, cherry_kit::ViewControllerPtr> mControllerMap;
  QMap<QString, cherry_kit::DataSourcePtr> mEngines;
  QFileSystemModel *mFileSystemModel;
  QFileIconProvider *mIconProvider;

  QSizeF mCurrentCellSize;
};

FolderProvider::FolderProvider(const QRectF &rect, QGraphicsObject *parent)
    : cherry_kit::TableModel(parent), d(new FolderProviderPrivate) {

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

float FolderProvider::left_margin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float FolderProvider::right_margin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

/*
UI::DesktopWidget *FolderProvider::loadWidgetControlllerByName(const
QString &controllerName) const
{
    QSharedPointer<UI::ControllerInterface> controller =
            (UI::PluginLoader::getInstance()->controller(controllerName));

    if (!controller.data()) {
        qWarning() << Q_FUNC_INFO << "Error loading extension" <<
controllerName;
        return 0;
    }

    UI::DesktopWidget *defaultView =
        qobject_cast<UI::DesktopWidget*>(controller->init());

    if (!defaultView)
        return 0;

    d->mControllerMap[controllerName] = controller;
    defaultView->setWidgetFlag(UI::DesktopWidget::BACKGROUND, false);
    defaultView->setWidgetFlag(UI::DesktopWidget::WINDOW, false);

    return defaultView;
}
*/

cherry_kit::DataSourcePtr
FolderProvider::loadDataSourceEngine(const QString &engine) {
  QSharedPointer<cherry_kit::DataSource> dataSource =
      cherry_kit::ExtensionManager::instance()->data_engine(engine);

  // connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));
  d->mEngines[engine] = dataSource;

  return dataSource;
}

cherry_kit::TableModel::TableRenderMode FolderProvider::render_type() const {
  return cherry_kit::TableModel::kRenderAsGridView;
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
