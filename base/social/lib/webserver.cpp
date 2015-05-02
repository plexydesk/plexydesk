#include "webserver.h"

#include <QTcpSocket>
#include <QDateTime>
#include <QUrl>
#include <QUrlQuery>
#include <QStringList>

#include "webcontentinterface.h"

namespace QuetzalSocialKit {

class WebServer::PrivateWebServer {

public:
  PrivateWebServer() {}
  ~PrivateWebServer() {}
  WebContentInterface *mContentIface;
};

WebServer::WebServer(QObject *parent)
    : QTcpServer(parent), d(new PrivateWebServer) {
  setMaxPendingConnections(1000);
  d->mContentIface = 0;
  qDebug() << Q_FUNC_INFO << "Server Start:";
  // startService(port);
}

WebServer::~WebServer() { delete d; }

void WebServer::startService(uint port) {
  bool ok = listen(QHostAddress::Any, port);

  if (ok) {
    qDebug() << Q_FUNC_INFO << "Listning";
  } else {
    qDebug() << Q_FUNC_INFO << "Error" << errorString();
  }
}

void WebServer::setWebContentDelegate(WebContentInterface *interface) {
  d->mContentIface = interface;
}

void WebServer::incomingConnection(qintptr socketDescriptor) {
  qDebug() << Q_FUNC_INFO << "New Connection";
  QTcpSocket *serverSocket = new QTcpSocket(this);

  connect(serverSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(serverSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

  if (serverSocket->setSocketDescriptor(socketDescriptor)) {
    addPendingConnection(serverSocket);
  } else {
    delete serverSocket;
  }
}

void WebServer::onReadyRead() {
  QTcpSocket *socket = (QTcpSocket *)sender();

  if (socket->canReadLine()) {
    QStringList tokens =
        QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));

    qDebug() << Q_FUNC_INFO << "Token ---->" << tokens;
    QUrl clientURL("http://localhost" + tokens[1]);
    QUrlQuery query(clientURL);
    qDebug() << Q_FUNC_INFO << query.queryItems();

    QVariantMap data;
    for (int i = 0; i < query.queryItems().count(); i++) {
      QPair<QString, QString> pair = query.queryItems().at(i);
      data[pair.first] = pair.second;
    }

    Q_EMIT requestCompleted(data);

    if (tokens[0] == "GET") {
      QTextStream os(socket);
      os.setAutoDetectUnicode(true);

      if (d->mContentIface) {
        os << d->mContentIface->content();
      } else {
        os << "HTTP/1.0 200 Ok\r\n"
              "Content-Type: text/html; charset=\"utf-8\"\r\n"
              "\r\n"
              "<html><head></head><body onLoad=\"\">"
              "<h1>Authentication Completed, Please Return to "
              "PlexyDesk</h1></body></html>\n"
           << QDateTime::currentDateTime().toString() << "\n";
      }
      socket->close();

      if (socket->state() == QTcpSocket::UnconnectedState) {
        delete socket;
      }
    }
  }
}

void WebServer::onDisconnected() {
  qDebug() << Q_FUNC_INFO;
  QTcpSocket *socket = (QTcpSocket *)sender();

  if (socket) {
    socket->deleteLater();
  }
}
}
