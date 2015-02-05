#include <QApplication>
#include <QDebug>

#include "datasyncruntime.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  DataSyncRuntime runner;

  return app.exec();
}
