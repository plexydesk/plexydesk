#include "webkitwebview.h"
#include <QObject>
#include <QWebFrame>
#include <QWebInspector>
#include <QNetworkCookieJar>

namespace UIKit
{

class WebKitWebView::PrivateWebKitWebView
{
public:
  PrivateWebKitWebView() {}
  ~PrivateWebKitWebView() {}

  QGraphicsWebView *mView;
  QHash<QString, QObject *> mObjects;
};

WebKitWebView::WebKitWebView(QGraphicsObject *parent)
  : UIKit::Widget(parent), d(new PrivateWebKitWebView)
{
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
            SLOT(add_java_script_object()));
  }
}

WebKitWebView::~WebKitWebView() { delete d; }

void WebKitWebView::set_url(const QUrl &a_url) { d->mView->setUrl(a_url); }

void WebKitWebView::inject_q_object(const QString &a_object_name, QObject *a_object_ptr)
{
  d->mObjects[a_object_name] = a_object_ptr;
}

void WebKitWebView::add_java_script_object()
{
  qDebug() << Q_FUNC_INFO << "Initiate WebKit Bridge";

  Q_FOREACH(const QString & objectName, d->mObjects.keys()) {
    if (d->mObjects[objectName]) {
      d->mView->page()->mainFrame()->addToJavaScriptWindowObject(
        objectName, d->mObjects[objectName]);
    }
  }
}
}
