#include <view_controller.h>
#include <extensionmanager.h>
#include <QDebug>

namespace UIKit {

class ViewController::PrivateViewControllerPlugin {
public:
  PrivateViewControllerPlugin() {}
  ~PrivateViewControllerPlugin() {}
  QSharedPointer<DataSource> mDataSource;
  Space *mViewport;
  QString mName;
};

ViewController::ViewController(QObject *parent)
    : QObject(parent), d(new PrivateViewControllerPlugin) {
  d->mViewport = 0;
}

ViewController::~ViewController() { delete d; }

void ViewController::set_viewport(Space *a_view_ptr) {
  d->mViewport = a_view_ptr;
}

Space *ViewController::viewport() { return d->mViewport; }

ActionList ViewController::actions() const { return ActionList(); }

void ViewController::request_action(const QString & /*actionName*/,
                                    const QVariantMap & /*args*/) {
  // Q_EMIT actionComleted("none", false, QString("Invalid Action"));
}

void ViewController::handle_drop_event(Widget * /*widget*/,
                                       QDropEvent * /*event*/) {}

DataSource *ViewController::dataSource() { return d->mDataSource.data(); }

void ViewController::set_controller_name(const QString &a_name) {
  d->mName = a_name;
}

QString ViewController::controller_name() const { return d->mName; }

QString ViewController::label() const { return QString(); }

void ViewController::configure(const QPointF &a_pos) { Q_UNUSED(a_pos) }

void ViewController::prepare_removal() { d->mDataSource.clear(); }

bool ViewController::connect_to_data_source(const QString &a_source) {
  d->mDataSource = ExtensionManager::instance()->data_engine(a_source);

  if (!d->mDataSource.data()) {
    return 0;
  }

  connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(on_ready()));

  return true;
}

bool ViewController::remove_widget(Widget *a_widget_ptr) {
  // disconnect(d->mDataSource.data(), SIGNAL(sourceUpdated(QVariantMap)));
  return false;
}

void ViewController::insert(Window *a_window_ptr) {
  if (!d->mViewport) {
    return;
  }

  d->mViewport->insert_window_to_view(a_window_ptr);
}

void ViewController::on_ready() {
  if (d->mDataSource) {
    Q_EMIT data(d->mDataSource.data());
  }
}
}
