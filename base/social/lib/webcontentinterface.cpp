#include "webcontentinterface.h"

#include <QDateTime>

namespace QuetzalSocialKit {

WebContentInterface::WebContentInterface() {}

QString WebContentInterface::content() {
  QString rv("HTTP/1.0 200 Ok\r\n"
             "Content-Type: text/html; charset=\"utf-8\"\r\n"
             "\r\n"
             "<html><head></head><body onLoad=\"\"><h1>Authentication "
             "Completed, Please Return to PlexyDesk</h1></body></html>\n");

  return rv;
}
}
