#include <QApplication>
#include <QDebug>

#include "socialtestrunner.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  SocialTestRunner runner;

  return app.exec();
}
