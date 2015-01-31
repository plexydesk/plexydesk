#include "cellsprovider.h"

#include "cellitem.h"

#include <extensionmanager.h>
#include <controllerinterface.h>

class CellsProvider::CellsProviderPrivate {
public:
  CellsProviderPrivate() {}
  ~CellsProviderPrivate() {}

  QMap<QString, PlexyDesk::ControllerPtr> mControllerMap;
  QMap<QString, PlexyDesk::DataSourcePtr> mEngines;
  QSizeF mCurrentCellSize;
};

CellsProvider::CellsProvider(const QRectF &rect, QGraphicsObject *parent)
    : PlexyDesk::DelegateInterface(parent), d(new CellsProviderPrivate) {}

CellsProvider::~CellsProvider() { delete d; }

bool CellsProvider::init() {
  PlexyDesk::DataSourcePtr geoLocationSource =
      this->loadDataSourceEngine("locationengine");

  if (geoLocationSource.data()) {
    connect(geoLocationSource.data(), SIGNAL(ready()), this,
            SLOT(onGeoLocationReady()));
    connect(geoLocationSource.data(), SIGNAL(sourceUpdated(QVariantMap)), this,
            SLOT(onLocationSourceUpdated(QVariantMap)));
  }

  PlexyDesk::DataSourcePtr googlePlacesSource =
      this->loadDataSourceEngine("googleplacesengine");

  if (googlePlacesSource.data()) {
    connect(googlePlacesSource.data(), SIGNAL(ready()), this,
            SLOT(onGeoLocationReady()));
    connect(googlePlacesSource.data(), SIGNAL(sourceUpdated(QVariantMap)), this,
            SLOT(onLocationSourceUpdated(QVariantMap)));
  }
  /*Load test clock */

  /*
  PlexyDesk::DesktopWidget *clockWidget =
  this->loadWidgetControlllerByName("plexyclock");
  clockWidget->setParent(this);

  for (int i = 0 ; i < 10 ; i++) {
      if (clockWidget) {
          CellItem *item = new CellItem(QRectF(0.0, 0.0, 100, 100), 0);
          mCellList.append(item);
          clockWidget->setParentItem(item);
          clockWidget->show();
          d->mCurrentCellSize += item->boundingRect().size();
      }
  }

  */
  Q_EMIT updated();

  return 1;
}

QList<PlexyDesk::TableViewItem *> CellsProvider::componentList() {
  return mCellList;
}

float CellsProvider::margin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float CellsProvider::padding() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float CellsProvider::leftMargin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

float CellsProvider::rightMargin() const {
#ifdef Q_WS_QPA
  return 20.0;
#endif

  return 5.0;
}

/*
PlexyDesk::DesktopWidget *CellsProvider::loadWidgetControlllerByName(const
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

PlexyDesk::DataSourcePtr CellsProvider::loadDataSourceEngine(
    const QString &engine) {
  QSharedPointer<PlexyDesk::DataSource> dataSource =
      PlexyDesk::ExtensionManager::instance()->desktopDataEngineExtension(
          engine);

  // connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));
  d->mEngines[engine] = dataSource;

  return dataSource;
}

void CellsProvider::onGeoLocationReady() {}

void CellsProvider::onLocationSourceUpdated(const QVariantMap &data) {

  Q_EMIT updated();
}
