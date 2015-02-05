#include <view_controller.h>
#include <extensionmanager.h>
#include <QDebug>

namespace UI
{

class ViewController::PrivateViewControllerPlugin
{
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}
  QSharedPointer<DataSource> mDataSource;
  Space *mViewport;
  QString mName;
};

ViewController::ViewController(QObject *parent)
  : QObject(parent), d(new PrivateViewControllerPlugin)
{
  d->mViewport = 0;
}

ViewController::~ViewController() { delete d; }

void ViewController::setViewport(Space *view)
{
  d->mViewport = view;
}

Space *ViewController::viewport() { return d->mViewport; }

ActionList ViewController::actions() const { return ActionList(); }

void ViewController::requestAction(const QString & /*actionName*/,
                                   const QVariantMap & /*args*/)
{
  // Q_EMIT actionComleted("none", false, QString("Invalid Action"));
}

void ViewController::handleDropEvent(Window * /*widget*/,
                                     QDropEvent * /*event*/) {}

DataSource *ViewController::dataSource() { return d->mDataSource.data(); }

void ViewController::setControllerName(const QString &name)
{
  d->mName = name;
}

QString ViewController::controllerName() const { return d->mName; }

DesktopActivityPtr ViewController::activity(const QString &name,
    const QRectF &geometry,
    const QPointF &pos,
    const QString &window_title)
{
  DesktopActivityPtr intent = ExtensionManager::instance()->activity(name);

  if (intent) {
    intent->setController(ViewControllerPtr(this));
    intent->createWindow(geometry, window_title, pos);
  }

  if (viewport()) {
    viewport()->addActivity(intent);
  }
  return intent;
}

QString ViewController::label() const { return QString(); }

void ViewController::configure(const QPointF &pos) { Q_UNUSED(pos) }

void ViewController::prepareRemoval() { d->mDataSource.clear(); }

bool ViewController::connectToDataSource(const QString &source)
{
  d->mDataSource = ExtensionManager::instance()->dataEngine(source);

  if (!d->mDataSource.data()) {
    return 0;
  }

  connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));

  return true;
}

bool ViewController::removeWidget(Window *widget)
{
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ViewController::insert(Window *widget)
{
  if (!d->mViewport) {
    return;
  }

  d->mViewport->addWidgetToView(widget);
}

void ViewController::onReady()
{
  if (d->mDataSource) {
    Q_EMIT data(d->mDataSource.data());
  }
}
}
