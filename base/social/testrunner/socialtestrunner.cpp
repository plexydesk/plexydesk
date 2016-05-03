#include "socialtestrunner.h"
#include <QDebug>
#include <QDesktopServices>
#include <QImage>
#include <QMap>
#include <QUrlQuery>
#include <QtNetwork>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <asyncimageloader.h>
#include <config.h>
#include <servicedefinition.h>
#include <webserver.h>
#include <webservice.h>

class SocialTestRunner::PrivateSocialTestRunner {
public:
  PrivateSocialTestRunner() {}
  ~PrivateSocialTestRunner() {}
  QList<social_kit::WebService *> mSizeServicesPending;
  QList<social_kit::WebService *> mSizeServicesCompleted;
};

SocialTestRunner::SocialTestRunner(QObject *parent)
    : d(new PrivateSocialTestRunner), QObject(parent) {
  qDebug() << Q_FUNC_INFO << "Runner Started";

  check_service_file();

  // testSocialPrefix();
  // testJSONHanlding();
  // startWebServer();
  // testDirLoader("file:///Library/Desktop Pictures/");
}

SocialTestRunner::~SocialTestRunner() {
  qDebug() << Q_FUNC_INFO << "Runner Ended";
  delete d;
}

void SocialTestRunner::check_service_file() {
  social_kit::ServiceDefinition def("/home/siraj/projects/plexydesk/"
                                          "/base/social/data/"
                                          "com.flikr.api.xml");

  /* service data */

  /*
   *
  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = "sky";
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);
  */

  social_kit::service_query_parameters input_data;

  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("text", "sky");
  input_data.insert("safe_search", "1");
  input_data.insert("tags", "wallpapers,wallpaper");
  input_data.insert("tag_mode", "all");
  input_data.insert("page", "1");

  qDebug() << Q_FUNC_INFO << "service_list =" << def.knownServices();
  qDebug() << Q_FUNC_INFO
           << "service_endpoint  = " << def.endpoint("flickr.photos.search");

  qDebug() << Q_FUNC_INFO << "service_request_type = "
           << def.requestType("flickr.photos.search");

  std::vector<std::string> argument_list =
      def.input_arguments("flickr.photos.search");
  std::vector<std::string> argument_list_opt =
      def.input_arguments("flickr.photos.search", true);

  std::for_each(
      std::begin(argument_list), std::end(argument_list),
      [](std::string value) { qDebug() << Q_FUNC_INFO << value.c_str(); });

  std::for_each(
      std::begin(argument_list_opt), std::end(argument_list_opt),
      [](std::string value) { qDebug() << Q_FUNC_INFO << value.c_str(); });

  qDebug() << Q_FUNC_INFO << "service_url = "
           << def.service_url("flickr.photos.search", &input_data).c_str();
}

void SocialTestRunner::testSocialPrefix() {
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.flikr.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = "sky";
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onServiceComplete(social_kit::WebService *)));
}

void SocialTestRunner::testJSONHanlding() {
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = "sky";
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onServiceCompleteJson(social_kit::WebService *)));
}

void SocialTestRunner::testSocialPhotoSizes(const QString &photoID) {
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.flikr.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);
  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onSizeServiceComplete(social_kit::WebService *)));
}

void SocialTestRunner::testsocialphotosizesJson(const QString &photoID) {
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);
  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onSizeServiceCompleteJson(social_kit::WebService *)));
}

void SocialTestRunner::testDirLoader(const QString &path) {
  social_kit::AsyncImageLoader *loader =
      new social_kit::AsyncImageLoader(this);
  loader->setUrl(QUrl(path));
  loader->start();

  connect(loader, SIGNAL(ready()), this, SLOT(onImageReady()));
}

void
SocialTestRunner::onServiceComplete(social_kit::WebService *service) {
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

void
SocialTestRunner::onSizeServiceComplete(social_kit::WebService *service) {
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
      social_kit::AsyncDataDownloader *downloader =
          new social_kit::AsyncDataDownloader(this);

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

void
SocialTestRunner::onDownloadComplete(social_kit::WebService *service) {
  if (service) {
    QByteArray data = service->rawServiceData();

    social_kit::AsyncImageCreator *imageSave =
        new social_kit::AsyncImageCreator(this);
    imageSave->setData(data, "/Users/siraj/Desktop/", true);
    imageSave->start();

    qDebug() << Q_FUNC_INFO << "Download Complete";
  }

  service->deleteLater();
}

void SocialTestRunner::onImageReady() {
  social_kit::AsyncDataDownloader *downloader =
      qobject_cast<social_kit::AsyncDataDownloader *>(sender());

  if (downloader) {
    social_kit::AsyncImageCreator *imageSave =
        new social_kit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), "/Users/siraj/Desktop/", false);
    imageSave->start();
    downloader->deleteLater();
  }
}

void SocialTestRunner::onImageSaveReady() {
  social_kit::AsyncImageCreator *c =
      qobject_cast<social_kit::AsyncImageCreator *>(sender());

  if (c) {
    qDebug() << Q_FUNC_INFO << "File Saved to: " << c->imagePath();
    qDebug() << Q_FUNC_INFO << c->metaData()["id"] << ": "
             << "File Details: " << c->image();
  }
}

void SocialTestRunner::onImageReady(const QString &fileName) {}

void
SocialTestRunner::onServiceCompleteJson(social_kit::WebService *service) {
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
    social_kit::WebService *service) {
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
      social_kit::AsyncDataDownloader *downloader =
          new social_kit::AsyncDataDownloader(this);

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
    social_kit::WebService *service) {}

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
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.dropbox.api.v2");

  QVariantMap args;
  args["code"] = data["code"].toString();
  args["client_id"] = "abxxj5vmfruyahu";
  args["client_secret"] = K_SOCIAL_KIT_DROPBOX_API_KEY;
  args["redirect_uri"] = "http://localhost:8081/";

  QHttpMultiPart *mpart = new QHttpMultiPart(this);
  service->queryService("dropbox.oauth2.token", args, mpart);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onDropBoxAuthServiceComplete(social_kit::WebService *)));
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
    social_kit::WebService *service) {
  getDropBoxAccountInfo(
      service->methodData("access_token").at(0)["access_token"].toString(),
      service->methodData("uid").at(0)["uid"].toString());
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid")["uid"];
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid").at(0)["uid"];
}

void SocialTestRunner::onDropBoxAccountInfoServiceComplete(
    social_kit::WebService *service) {
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

  social_kit::WebServer *server = new social_kit::WebServer(this);
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
  social_kit::WebService *service =
      new social_kit::WebService(this);

  service->create("com.dropbox.api.v2");

  QVariantMap args;
  QHttpMultiPart *mpart = new QHttpMultiPart(this);

  service->queryService("dropbox.oauth2.accountInfo", args, mpart,
                        "Authorization",
                        QString("Bearer " + access_token).toLatin1());

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onDropBoxAccountInfoServiceComplete(
              social_kit::WebService *)));
}
