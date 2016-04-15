/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include <QuetzalSocialKitQt_export.h>

namespace social_kit {
class WebContentInterface;
class QuetzalSocialKitQt_EXPORT WebServer : public QTcpServer {
  Q_OBJECT
public:
  explicit WebServer(QObject *a_parent_ptr = 0);
  virtual ~WebServer();

  virtual void incomingConnection(qintptr socketDescriptor);
  virtual void startService(uint port);
  virtual void setWebContentDelegate(WebContentInterface *interface);
Q_SIGNALS:
  void requestCompleted(const QVariantMap &data);
public
Q_SLOTS:
  void onReadyRead();
  void onDisconnected();

private:
  class PrivateWebServer;
  PrivateWebServer *const d;
};
}
#endif // WEBSERVER_H
