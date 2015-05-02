#include <config.h>
#include "socialtestrunner.h"
#include <QDebug>
#include <QImage>
#include <webservice.h>
#include <asyncimageloader.h>
#include <asyncimagecreator.h>
#include <asyncdatadownloader.h>
#include <webserver.h>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QtNetwork>
#include <QMap>

class SocialTestRunner::PrivateSocialTestRunner {
public:
  PrivateSocialTestRunner() {}
  ~PrivateSocialTestRunner() {}
  QList<QuetzalSocialKit::WebService *> mSizeServicesPending;
  QList<QuetzalSocialKit::WebService *> mSizeServicesCompleted;
};

SocialTestRunner::SocialTestRunner(QObject *parent)
    : d(new PrivateSocialTestRunner), QObject(parent) {
  qDebug() << Q_FUNC_INFO << "Runner Started";

  testSocialPrefix();

  // testJSONHanlding();

  // startWebServer();

  // testDirLoader("file:///Library/Desktop Pictures/");
}

SocialTestRunner::~SocialTestRunner() {
  qDebug() << Q_FUNC_INFO << "Runner Ended";
  delete d;
}

void SocialTestRunner::testSocialPrefix() {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flikr.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = "sky";
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceComplete(QuetzalSocialKit::WebService *)));
}

void SocialTestRunner::testJSONHanlding() {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = "sky";
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void SocialTestRunner::testSocialPhotoSizes(const QString &photoID) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flikr.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);
  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onSizeServiceComplete(QuetzalSocialKit::WebService *)));
}

void SocialTestRunner::testsocialphotosizesJson(const QString &photoID) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);
  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onSizeServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void SocialTestRunner::testDirLoader(const QString &path) {
  QuetzalSocialKit::AsyncImageLoader *loader =
      new QuetzalSocialKit::AsyncImageLoader(this);
  loader->setUrl(QUrl(path));
  loader->start();

  connect(loader, SIGNAL(ready()), this, SLOT(onImageReady()));
}

void SocialTestRunner::onServiceComplete(
    QuetzalSocialKit::WebService *service) {
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photo").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("status").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photos").count();

  QList<QVariantMap> photoList = service->methodData("photo");

  Q_FOREACH(const QVariantMap & map, photoList) {
    qDebug() << Q_FUNC_INFO << map["id"].toString();
    testSocialPhotoSizes(map["id"].toString());
  }

  service->deleteLater();
  ;
}

void SocialTestRunner::onSizeServiceComplete(
    QuetzalSocialKit::WebService *service) {
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("size").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("status").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("sizes").count();

  Q_FOREACH(const QVariantMap & map, service->methodData("size")) {
    qDebug() << Q_FUNC_INFO << map;
    if (map["label"].toString() == "Large" ||
        map["label"].toString() == "Large 1600" ||
        map["label"].toString() == "Original") {
      qDebug() << Q_FUNC_INFO << map["label"].toString() << "->"
               << map["source"].toString();
      QuetzalSocialKit::AsyncDataDownloader *downloader =
          new QuetzalSocialKit::AsyncDataDownloader(this);

      QVariantMap metaData;
      metaData["method"] = service->methodName();
      metaData["id"] =
          service->inputArgumentForMethod(service->methodName())["photo_id"];
      metaData["data"] = service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }
  }

  service->deleteLater();
}

void SocialTestRunner::onDownloadComplete(
    QuetzalSocialKit::WebService *service) {
  if (service) {
    QByteArray data = service->rawServiceData();

    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);
    imageSave->setData(data, "/Users/siraj/Desktop/", true);
    imageSave->start();

    qDebug() << Q_FUNC_INFO << "Download Complete";
  }

  service->deleteLater();
}

void SocialTestRunner::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), "/Users/siraj/Desktop/", false);
    imageSave->start();
    downloader->deleteLater();
  }
}

void SocialTestRunner::onImageSaveReady() {
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    qDebug() << Q_FUNC_INFO << "File Saved to: " << c->imagePath();
    qDebug() << Q_FUNC_INFO << c->metaData()["id"] << ": "
             << "File Details: " << c->image();
  }
}

void SocialTestRunner::onImageReady(const QString &fileName) {}

void SocialTestRunner::onServiceCompleteJson(
    QuetzalSocialKit::WebService *service) {
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photo").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("stat").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photos");

  QList<QVariantMap> photoList = service->methodData("photo");

  Q_FOREACH(const QVariantMap & map, photoList) {
    qDebug() << Q_FUNC_INFO << map["id"].toString();
    testsocialphotosizesJson(map["id"].toString());
  }

  service->deleteLater();
  ;
}

void SocialTestRunner::onSizeServiceCompleteJson(
    QuetzalSocialKit::WebService *service) {
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("size").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("stat").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("sizes").count();

  Q_FOREACH(const QVariantMap & map, service->methodData("size")) {
    // qDebug() << Q_FUNC_INFO << map;
    if (map["label"].toString() == "Large" ||
        map["label"].toString() == "Large 1600" ||
        map["label"].toString() == "Original") {
      qDebug() << Q_FUNC_INFO << map["label"].toString() << "->"
               << map["source"].toString();
      QuetzalSocialKit::AsyncDataDownloader *downloader =
          new QuetzalSocialKit::AsyncDataDownloader(this);

      QVariantMap metaData;
      metaData["method"] = service->methodName();
      metaData["id"] =
          service->inputArgumentForMethod(service->methodName())["photo_id"];
      metaData["data"] = service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }
  }

  service->deleteLater();
}

void SocialTestRunner::onDownloadCompleteJson(
    QuetzalSocialKit::WebService *service) {}

void SocialTestRunner::onImageReadyJson() {}

void SocialTestRunner::onImageSaveReadyJson() {}

void SocialTestRunner::onImageReadyJson(const QString &fileName) {}

void SocialTestRunner::onServerRequestCompleted(const QVariantMap &data) {
  // qDebug() << Q_FUNC_INFO << data;

  // https://api.dropbox.com/1/oauth2/token
  /*
  QUrlQuery query;
  query.addQueryItem("code", data["code"].toString());
  query.addQueryItem("grant_type", "authorization_code");
  query.addQueryItem("client_id", "abxxj5vmfruyahu");
  query.addQueryItem("client_secret", K_SOCIAL_KIT_DROPBOX_API_KEY);
  query.addQueryItem("redirect_uri", "http://localhost:8081/");

  //https://www.dropbox.com/1/oauth2/authorize?
  QUrl dropboxUrl("https://api.dropbox.com/1/oauth2/token");
  dropboxUrl.setQuery(query);
  qDebug() <<Q_FUNC_INFO << dropboxUrl;

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);

  QHttpMultiPart *part = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  manager->post(QNetworkRequest(dropboxUrl), part);

  connect(manager, SIGNAL(finished(QNetworkReply*)), this,
  SLOT(onFinished(QNetworkReply*)));
  */

  // social
  qDebug() << Q_FUNC_INFO;
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.dropbox.api.v2");

  QVariantMap args;
  args["code"] = data["code"].toString();
  args["client_id"] = "abxxj5vmfruyahu";
  args["client_secret"] = K_SOCIAL_KIT_DROPBOX_API_KEY;
  args["redirect_uri"] = "http://localhost:8081/";

  QHttpMultiPart *mpart = new QHttpMultiPart(this);
  service->queryService("dropbox.oauth2.token", args, mpart);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onDropBoxAuthServiceComplete(QuetzalSocialKit::WebService *)));
}

void SocialTestRunner::onDropBoxRequestComplete(const QVariantMap &data) {
  qDebug() << Q_FUNC_INFO;
}

void SocialTestRunner::onFinished(QNetworkReply *reply) {
  qDebug() << Q_FUNC_INFO;
  if (reply) {
    QByteArray data = reply->readAll();
    qDebug() << Q_FUNC_INFO << data;
  }
}

void SocialTestRunner::onDropBoxAuthServiceComplete(
    QuetzalSocialKit::WebService *service) {
  getDropBoxAccountInfo(
      service->methodData("access_token").at(0)["access_token"].toString(),
      service->methodData("uid").at(0)["uid"].toString());
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid")["uid"];
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid").at(0)["uid"];
}

void SocialTestRunner::onDropBoxAccountInfoServiceComplete(
    QuetzalSocialKit::WebService *service) {
  qDebug() << Q_FUNC_INFO << "Done" << service->rawServiceData();
  qDebug() << Q_FUNC_INFO
           << "Account Info:" << service->methodData("referral_link");
  qDebug() << Q_FUNC_INFO << "Account Info:" << service->methodData("team");
  qDebug() << Q_FUNC_INFO << "Account Info:" << service->methodData("quota");
  qDebug() << Q_FUNC_INFO << "Account Info:" << service->methodData("country");
  qDebug() << Q_FUNC_INFO << "Account Info:" << service->methodData("uid");
  qDebug() << Q_FUNC_INFO
           << "Account Info:" << service->methodData("display_name");
  qDebug() << Q_FUNC_INFO << "Account Info:" << service->methodData("email");
  qDebug() << Q_FUNC_INFO
           << "Account Info:" << service->methodData("quota_info");
}

// web server
void SocialTestRunner::startWebServer() {
  authDropBox();

  QuetzalSocialKit::WebServer *server = new QuetzalSocialKit::WebServer(this);
  server->startService(8081);

  connect(server, SIGNAL(requestCompleted(QVariantMap)), this,
          SLOT(onServerRequestCompleted(QVariantMap)));
}

void SocialTestRunner::authDropBox() {
  // based on :
  // https://www.dropbox.com/developers/blog/45/using-oauth-20-with-the-core-api;
  qsrand(16);
  int number = qrand();
  QString data = QString("%1").arg(number);
  QByteArray CSRF_Token = data.toLatin1().toBase64();

  QUrlQuery query;
  query.addQueryItem("client_id", "abxxj5vmfruyahu");
  query.addQueryItem("response_type", "code");
  query.addQueryItem("redirect_uri", "http://localhost:8081/");
  query.addQueryItem("state", CSRF_Token);

  QUrl dropboxUrl("https://www.dropbox.com/1/oauth2/authorize?");
  dropboxUrl.setQuery(query);

  QDesktopServices::openUrl(dropboxUrl);
}

void SocialTestRunner::getDropBoxAccountInfo(const QString &access_token,
                                             const QString &uid) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.dropbox.api.v2");

  QVariantMap args;
  QHttpMultiPart *mpart = new QHttpMultiPart(this);

  service->queryService("dropbox.oauth2.accountInfo", args, mpart,
                        "Authorization",
                        QString("Bearer " + access_token).toLatin1());

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onDropBoxAccountInfoServiceComplete(
              QuetzalSocialKit::WebService *)));
}
