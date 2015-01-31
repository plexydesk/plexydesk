#include <controllerinterface.h>
#include <extensionmanager.h>
#include <QDebug>

namespace PlexyDesk {

class ControllerInterface::PrivateViewControllerPlugin {
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}
  QSharedPointer<DataSource> mDataSource;
  DesktopViewport *mViewport;
  QString mName;
};

ControllerInterface::ControllerInterface(QObject *parent)
    : QObject(parent), d(new PrivateViewControllerPlugin) {
  d->mViewport = 0;
}

ControllerInterface::~ControllerInterface() { delete d; }

void ControllerInterface::setViewport(DesktopViewport *view) {
  d->mViewport = view;
  connect(this, SIGNAL(addWidget(Widget *)), d->mViewport,
          SLOT(addWidgetToView(Widget *)));
}

DesktopViewport *ControllerInterface::viewport() { return d->mViewport; }

ActionList ControllerInterface::actions() const { return ActionList(); }

void ControllerInterface::requestAction(const QString & /*actionName*/,
                                        const QVariantMap & /*args*/) {
  // Q_EMIT actionComleted("none", false, QString("Invalid Action"));
}

void ControllerInterface::handleDropEvent(Widget * /*widget*/,
                                          QDropEvent * /*event*/) {}

DataSource *ControllerInterface::dataSource() { return d->mDataSource.data(); }

void ControllerInterface::setControllerName(const QString &name) {
  d->mName = name;
}

QString ControllerInterface::controllerName() const { return d->mName; }

DesktopActivityPtr ControllerInterface::activity(const QString &name,
                                                 const QRectF &geometry,
                                                 const QPointF &pos,
                                                 const QString &window_title) {
  DesktopActivityPtr intent = ExtensionManager::instance()->activity(name);

  if (intent) {
    intent->setController(ControllerPtr(this));
    intent->createWindow(geometry, window_title, pos);
  }

  if (viewport()) {
    PlexyDesk::DesktopViewport *view =
        qobject_cast<PlexyDesk::DesktopViewport *>(viewport());

    if (view) {
      view->addActivity(intent);
    }
  }
  return intent;
}

QString ControllerInterface::label() const { return QString(); }

void ControllerInterface::configure(const QPointF &pos) { Q_UNUSED(pos) }

void ControllerInterface::prepareRemoval() { d->mDataSource.clear(); }

bool ControllerInterface::connectToDataSource(const QString &source) {
  d->mDataSource = ExtensionManager::instance()->dataEngine(source);

  if (!d->mDataSource.data())
    return 0;

  connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));

  return true;
}

bool ControllerInterface::removeWidget(Widget *widget) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ControllerInterface::onReady() {
  if (d->mDataSource)
    Q_EMIT data(d->mDataSource.data());
}
}
