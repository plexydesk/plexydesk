#include "extrunner.h"
#include <QDebug>

#include <datasource.h>
#include <extensionmanager.h>

class ExtLaoderTestRunner::PrivateExtLaoderTestRunner {
public:
  PrivateExtLaoderTestRunner() {}
  ~PrivateExtLaoderTestRunner() {}

  PlexyDesk::DataSourcePtr mDataSource;
};

ExtLaoderTestRunner::ExtLaoderTestRunner(QObject *parent)
    : d(new PrivateExtLaoderTestRunner), QObject(parent) {
  qDebug() << Q_FUNC_INFO << "Runner Started";
}

ExtLaoderTestRunner::~ExtLaoderTestRunner() {
  qDebug() << Q_FUNC_INFO << "Runner Ended";
  delete d;
}

bool ExtLaoderTestRunner::connectToDataSource(const QString &source) {
  d->mDataSource = PlexyDesk::ExtensionManager::instance()->dataEngine(source);

  if (!d->mDataSource.data())
    return 0;

  connect(d->mDataSource.data(), SIGNAL(ready()), this, SLOT(onReady()));
  connect(d->mDataSource.data(), SIGNAL(sourceUpdated(const QVariantMap &)),
          this, SLOT(onSourceUpdated(QVariantMap)));

  return true;
}

void ExtLaoderTestRunner::loadtest(const QString &source) {
  PlexyDesk::DataSourcePtr _source =
      PlexyDesk::ExtensionManager::instance()->dataEngine(source);

  if (!_source)
    return;

  qDebug() << Q_FUNC_INFO << "Complete";
}

void ExtLaoderTestRunner::onSourceUpdated(const QVariantMap &data) {
  qDebug() << Q_FUNC_INFO << data;
}
