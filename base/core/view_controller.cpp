#include <view_controller.h>
#include <extensionmanager.h>
#include <QDebug>

namespace PlexyDesk {

class ViewController::PrivateViewControllerPlugin {
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}
  QSharedPointer<DataSource> mDataSource;
  DesktopViewport *mViewport;
  QString mName;
};

ViewController::ViewController(QObject *parent)
    : QObject(parent), d(new PrivateViewControllerPlugin) {
  d->mViewport = 0;
}

ViewController::~ViewController() { delete d; }

void ViewController::setViewport(DesktopViewport *view) {
  d->mViewport = view;
  connect(this, SIGNAL(addWidget(Widget *)), d->mViewport,
          SLOT(addWidgetToView(Widget *)));
}

DesktopViewport *ViewController::viewport() { return d->mViewport; }

ActionList ViewController::actions() const { return ActionList(); }

void ViewController::requestAction(const QString & /*actionName*/,
                                        const QVariantMap & /*args*/) {
  // Q_EMIT actionComleted("none", false, QString("Invalid Action"));
}

void ViewController::handleDropEvent(Widget * /*widget*/,
                                          QDropEvent * /*event*/) {}

DataSource *ViewController::dataSource() { return d->mDataSource.data(); }

void ViewController::setControllerName(const QString &name) {
  d->mName = name;
}

QString ViewController::controllerName() const { return d->mName; }

DesktopActivityPtr ViewController::activity(const QString &name,
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

QString ViewController::label() const { return QString(); }

void ViewController::configure(const QPointF &pos) { Q_UNUSED(pos) }

void ViewController::prepareRemoval() { d->mDataSource.clear(); }

bool ViewController::connectToDataSource(const QString &source) {
  d->mDataSource = ExtensionManager::instance()->dataEngine(source);

  if (!d->mDataSource.data())
    return 0;

  connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));

  return true;
}

bool ViewController::removeWidget(Widget *widget) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ViewController::onReady() {
  if (d->mDataSource)
    Q_EMIT data(d->mDataSource.data());
}
}
