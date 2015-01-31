#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit {
class WebContentInterface;
class QuetzalSocialKit_EXPORT WebServer : public QTcpServer {
  Q_OBJECT
public:
  explicit WebServer(QObject *parent = 0);
  virtual ~WebServer();

  virtual void incomingConnection(qintptr socketDescriptor);
  virtual void startService(uint port);
  virtual void setWebContentDelegate(WebContentInterface *interface);
Q_SIGNALS:
  void requestCompleted(const QVariantMap &data);
public Q_SLOTS:
  void onReadyRead();
  void onDisconnected();

private:
  class PrivateWebServer;
  PrivateWebServer *const d;
};
}
#endif // WEBSERVER_H
