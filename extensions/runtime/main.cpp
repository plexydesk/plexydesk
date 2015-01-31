#include "extrunner.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QCommandLineParser>
#include <extensionmanager.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setApplicationName("PlexyDesk Engine Loader");
  QCoreApplication::setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("EngineLoader helper");
  parser.addHelpOption();
  parser.addVersionOption();

  parser.process(app);

  const QStringList args = parser.positionalArguments();
  if (args.count() <= 0 || args[0].isEmpty() || args[0].isNull()) {
    qWarning() << "Source Engine Not provided";
    exit(0);
  }

  QString pathPtr(getenv("PLUGIN_HOME"));

  if (pathPtr.isEmpty() || pathPtr.isNull()) {
    qWarning() << "PLUGIN_HOME environment variable not set ";
    exit(0);
  }
  qDebug() << "Checking environment variable PLUGIN_HOME" << pathPtr;

#ifdef Q_OS_MAC
  PlexyDesk::ExtensionManager *loader = PlexyDesk::ExtensionManager::instance(
      QDir::toNativeSeparators(
          (pathPtr) + QLatin1String("/Contents/share/plexy/ext/groups/")),
      QDir::toNativeSeparators((pathPtr) +
                               QLatin1String("/Contents/lib/plexyext/")));
#endif

#ifdef Q_OS_LINUX
  PlexyDesk::ExtensionManager *loader = PlexyDesk::ExtensionManager::instance(
      QDir::toNativeSeparators((pathPtr) +
                               QLatin1String("/share/plexy/ext/groups/")),
      QDir::toNativeSeparators((pathPtr) + QLatin1String("/lib/plexyext/")));
#endif

  ExtLaoderTestRunner runner;

  // runner.connectToDataSource(args[0]);
  runner.loadtest(args[0]);

  loader->destroyInstance();

  return app.exec();
}
