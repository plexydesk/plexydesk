#include <ck_sandbox.h>

#include <QStandardPaths>

QString ck_sandbox_root() {

#ifdef Q_OS_WIN
  QString home = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return home;
#else
  QString home = QDir::homePath();
  return home;
#endif
}
