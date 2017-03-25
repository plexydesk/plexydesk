#include <QApplication>
#include <QDebug>

#include "socialtestrunner.h"
#include "test_url_post_request.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  //SocialTestRunner runner;

  test_url_post_request test_01;
  test_01.run();

  return app.exec();
}
