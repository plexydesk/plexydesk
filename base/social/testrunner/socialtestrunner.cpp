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

#include <iostream>

#include <ck_url.h>

#define CK_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__         \
                << " line " << __LINE__ << ": " << message << std::endl;       \
      std::exit(EXIT_FAILURE);                                                 \
    } else {                                                                   \
      std::cout << __LINE__ << " " << __func__ << "  " << #condition           \
                << "  [PASS]" << std::endl;                                    \
    }                                                                          \
  } while (false)

class SocialTestRunner::PrivateSocialTestRunner {
public:
  PrivateSocialTestRunner() {}
  ~PrivateSocialTestRunner() {}
  QList<social_kit::web_service *> mSizeServicesPending;
  QList<social_kit::web_service *> mSizeServicesCompleted;
};

SocialTestRunner::SocialTestRunner(QObject *parent)
    : d(new PrivateSocialTestRunner), QObject(parent) {
  qDebug() << Q_FUNC_INFO << "Runner Started";

  check_service_file();
  check_url_encode();
  check_url_request_loader();

  testSocialPrefix();

  // testDirLoader("file:///Library/Desktop Pictures/");
}

SocialTestRunner::~SocialTestRunner() {
  qDebug() << Q_FUNC_INFO << "Runner Ended";
  delete d;
}

void SocialTestRunner::check_url_encode() {
  social_kit::url_encode url("siraj@gmail.com");
  CK_ASSERT(url.to_string() == "siraj%40gmail.com", "Invalid Encoding");
}

void SocialTestRunner::check_url_request_loader() {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([&](const social_kit::url_response &response) {
    CK_ASSERT(response.status_code() == 200, "Invalid Response From Server");
    CK_ASSERT(response.http_version() == "HTTP 1.0",
              "Invalid Response From Server");

    //CK_ASSERT(response.data_buffer_size() == 5320,
    //          "Size Mismatch : " << response.data_buffer_size());
    CK_ASSERT(response.data_buffer()[0] == '<', "Not XML Data");
    CK_ASSERT(response.data_buffer()[1] == '?', "Not XML Data");
    CK_ASSERT(response.data_buffer()[2] == 'x', "Not XML Data");
    CK_ASSERT(response.data_buffer()[3] == 'm', "Not XML Data");
    CK_ASSERT(response.data_buffer()[4] == 'l', "Not XML Data");

    social_kit::remote_service srv_query("com.flikr.api.xml");
    const social_kit::remote_result result =
        srv_query.response("flickr.photos.search", response);
    CK_ASSERT(result.get("photo").size() == 30,
              "expected 30 but got : " << result.get("photo").size());
    social_kit::remote_result_data query = result.get("rsp").at(0);

    CK_ASSERT(query.get("stat").value() == "ok",
              "expected OK but got : " << query.get("stat").value());
  });

  request->send_message(
      social_kit::url_request::kGETRequest,
      "https://api.flickr.com/services/rest/"
      "?method=flickr.photos.search&api_key=" K_SOCIAL_KIT_FLICKR_API_KEY
      "&text=sky&format="
      "rest&tags=wallpapers%2cwallpaper&tag_mode=all&safe_"
      "search=1&in_gallery=true&per_page=30&page=1");
}

void SocialTestRunner::check_service_file() {
  social_kit::remote_service srv_query("com.flikr.api.xml");

  /* service data */
  social_kit::service_query_parameters input_data;

  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("text", "sky");
  input_data.insert("safe_search", "1");
  input_data.insert("tags", "wallpapers,wallpaper");
  input_data.insert("tag_mode", "all");
  input_data.insert("page", "1");

  CK_ASSERT(
      srv_query.method("flickr.photos.search") == 1,
      "Invalid Request Type : " << srv_query.method("flickr.photos.search"));
  CK_ASSERT(srv_query.endpoint("flickr.photos.search") ==
                "https://api.flickr.com/services/rest/",
            "Ivalid Input : " << srv_query.endpoint("flickr.photos.search"));

  social_kit::string_list argument_list =
      srv_query.input_arguments("flickr.photos.search");
  social_kit::string_list argument_list_opt =
      srv_query.input_arguments("flickr.photos.search", true);

  std::for_each(std::begin(argument_list), std::end(argument_list),
                [](std::string value) {
    // qDebug() << Q_FUNC_INFO << value.c_str();
  });

  std::for_each(std::begin(argument_list_opt), std::end(argument_list_opt),
                [](std::string value) {
    // qDebug() << Q_FUNC_INFO << value.c_str();
  });

  QString expected_query_url(
      "https://api.flickr.com/services/rest/"
      "?method=flickr.photos.search&api_key=" K_SOCIAL_KIT_FLICKR_API_KEY
      "&text=sky&"
      "format=rest&tags=wallpapers%2cwallpaper&tag_mode="
      "all&safe_search=1&in_gallery=true&per_page=30&"
      "page=1");

  CK_ASSERT(expected_query_url.toStdString() ==
                srv_query.url("flickr.photos.search", &input_data),
            "Invalid Query URL");
}

void SocialTestRunner::testSocialPrefix() {
  social_kit::web_service *web_service = new social_kit::web_service(0);
  social_kit::service_query_parameters input_data;

  web_service->create("com.flikr.api.xml");
  web_service->on_response_ready([&](const social_kit::remote_result &result,
                                 const social_kit::web_service *a_service) {
  });

  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("text", "sky");
  input_data.insert("safe_search", "1");
  input_data.insert("tags", "wallpapers,wallpaper");
  input_data.insert("tag_mode", "all");
  input_data.insert("page", "1");

  web_service->submit("flickr.photos.search", &input_data);
}

void SocialTestRunner::testJSONHanlding() {
  social_kit::web_service *service = new social_kit::web_service(this);

  service->create("com.flickr.json.api");

  social_kit::service_query_parameters input_data;

  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("text", "sky");
  input_data.insert("safe_search", "1");
  input_data.insert("tags", "wallpapers,wallpaper");
  input_data.insert("tag_mode", "all");
  input_data.insert("page", "1");

  service->on_response_ready([&](const social_kit::remote_result &a_result,
                             const social_kit::web_service *a_web_service) {
      qDebug() << Q_FUNC_INFO << "Done";
      onServiceCompleteJson(a_web_service);
  });

  service->submit("flickr.photos.search", &input_data);
}

void SocialTestRunner::testSocialPhotoSizes(const QString &photoID) {
  social_kit::web_service *service = new social_kit::web_service(this);

  service->create("com.flikr.api");

  social_kit::service_query_parameters input_data;
  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("photo_id", photoID.toStdString());

  service->submit("flickr.photos.getSizes", &input_data);
  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(social_kit::web_service *)), this,
          SLOT(onSizeServiceComplete(social_kit::web_service *)));
}

void SocialTestRunner::testsocialphotosizesJson(const QString &photoID) {
  social_kit::web_service *service = new social_kit::web_service(this);

  service->create("com.flickr.json.api");

  social_kit::service_query_parameters input_data;
  input_data.insert("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.insert("photo_id", photoID.toStdString());

  service->submit("flickr.photos.getSizes", &input_data);

  d->mSizeServicesPending.append(service);

  connect(service, SIGNAL(finished(social_kit::web_service *)), this,
          SLOT(onSizeServiceCompleteJson(social_kit::web_service *)));
}

void SocialTestRunner::testDirLoader(const QString &path) {
  social_kit::AsyncImageLoader *loader = new social_kit::AsyncImageLoader(this);
  loader->setUrl(QUrl(path));
  loader->start();

  connect(loader, SIGNAL(ready()), this, SLOT(onImageReady()));
}

void SocialTestRunner::onServiceComplete(social_kit::web_service *service) {
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

void SocialTestRunner::onSizeServiceComplete(social_kit::web_service *service) {
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
      metaData["method"] = service->query();
      metaData["id"] = service->inputArgumentForMethod(service->query())
                           .value("photo_id")
                           .c_str();
      // metaData["data"] =
      // service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }
  }

  service->deleteLater();
}

void SocialTestRunner::onDownloadComplete(social_kit::web_service *service) {
  if (service) {
    /*
    QByteArray data = service->rawServiceData();

    social_kit::AsyncImageCreator *imageSave =
        new social_kit::AsyncImageCreator(this);
    imageSave->setData(data, "/Users/siraj/Desktop/", true);
    imageSave->start();

    qDebug() << Q_FUNC_INFO << "Download Complete";
    */
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

void SocialTestRunner::onServiceCompleteJson(const social_kit::web_service *service) {
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
}

void
SocialTestRunner::onSizeServiceCompleteJson(social_kit::web_service *service) {
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
      metaData["method"] = service->query();
      metaData["id"] = service->inputArgumentForMethod(service->query())
                           .value("photo_id")
                           .c_str();
      // metaData["data"] =
      // service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }
  }

  service->deleteLater();
}

void SocialTestRunner::onDownloadCompleteJson(social_kit::web_service *service) {
}

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
  social_kit::web_service *service = new social_kit::web_service(this);

  service->create("com.dropbox.api.v2");

  /*
  QVariantMap args;
  args["code"] = data["code"].toString();
  args["client_id"] = "abxxj5vmfruyahu";
  args["client_secret"] = K_SOCIAL_KIT_DROPBOX_API_KEY;
  args["redirect_uri"] = "http://localhost:8081/";
  */

  social_kit::service_query_parameters input_data;
  input_data.insert("code", data["code"].toString().toStdString());
  input_data.insert("client_id", "abxxj5vmfruyahu");
  input_data.insert("client_secret", K_SOCIAL_KIT_DROPBOX_API_KEY);
  input_data.insert("redirect_uri", "http://localhost:8081/");

  QHttpMultiPart *mpart = new QHttpMultiPart(this);
  service->submit("dropbox.oauth2.token", &input_data, mpart);

  connect(service, SIGNAL(finished(social_kit::web_service *)), this,
          SLOT(onDropBoxAuthServiceComplete(social_kit::web_service *)));
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
    social_kit::web_service *service) {
  getDropBoxAccountInfo(
      service->methodData("access_token").at(0)["access_token"].toString(),
      service->methodData("uid").at(0)["uid"].toString());
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid")["uid"];
  // qDebug() << Q_FUNC_INFO << "Service Complete: " <<
  // service->methodData("uid").at(0)["uid"];
}

void SocialTestRunner::onDropBoxAccountInfoServiceComplete(
    social_kit::web_service *service) {
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
  social_kit::web_service *service = new social_kit::web_service(this);

  service->create("com.dropbox.api.v2");

  QHttpMultiPart *mpart = new QHttpMultiPart(this);

  social_kit::service_query_parameters input_data;

  service->submit("dropbox.oauth2.accountInfo", &input_data, mpart,
                        "Authorization",
                        QString("Bearer " + access_token).toLatin1());

  connect(service, SIGNAL(finished(social_kit::web_service *)), this,
          SLOT(onDropBoxAccountInfoServiceComplete(social_kit::web_service *)));
}
