#include "webkitwebview.h"
#include <QObject>
#include <QWebFrame>
#include <QWebInspector>
#include <QNetworkCookieJar>

namespace PlexyDesk {

class WebKitWebView::PrivateWebKitWebView {
public:
  PrivateWebKitWebView() {}
  ~PrivateWebKitWebView() {}

  QGraphicsWebView *mView;
  QHash<QString, QObject *> mObjects;
};

WebKitWebView::WebKitWebView(QGraphicsObject *parent)
    : PlexyDesk::UIWidget(parent), d(new PrivateWebKitWebView) {
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  d->mView = new QGraphicsWebView(this);
  QRectF rect(0.0, 0.0, 200.0, 200.0);
  d->mView->resize(rect.size());
  d->mView->setResizesToContents(false);
  d->mView->setGeometry(rect);
  d->mView->show();
  d->mView->setPos(0.0, 24.0);
  d->mView->setRenderHints(QPainter::Antialiasing |
                           QPainter::HighQualityAntialiasing);

  QNetworkCookieJar *cookieJar = new QNetworkCookieJar(this);
  QNetworkAccessManager *nam = new QNetworkAccessManager();
  nam->setCookieJar(cookieJar);
  d->mView->page()->setNetworkAccessManager(nam);

  QWebSettings *settings = d->mView->page()->settings();
  settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

#if 0
    QWebInspector *inspector = new QWebInspector;
    inspector->setPage(d->mView->page());
    inspector->show();
#endif

  if (d->mView->page() && d->mView->page()->mainFrame()) {
    connect(d->mView->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()), this,
            SLOT(addJavaScriptObject()));
  }
}

WebKitWebView::~WebKitWebView() { delete d; }

void WebKitWebView::setUrl(const QUrl &url) { d->mView->setUrl(url); }

void WebKitWebView::injectQObject(const QString &objectName, QObject *object) {
  d->mObjects[objectName] = object;
}

void WebKitWebView::addJavaScriptObject() {
  qDebug() << Q_FUNC_INFO << "Initiate WebKit Bridge";

  Q_FOREACH(const QString & objectName, d->mObjects.keys()) {
    if (d->mObjects[objectName]) {
      d->mView->page()->mainFrame()->addToJavaScriptWindowObject(
          objectName, d->mObjects[objectName]);
    }
  }
}
}
