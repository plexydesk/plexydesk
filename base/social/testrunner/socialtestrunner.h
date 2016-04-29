#ifndef SOCIALTESTRUNNER_H
#define SOCIALTESTRUNNER_H

#include <QObject>
#include <webservice.h>

class SocialTestRunner : public QObject {
  Q_OBJECT
public:
  explicit SocialTestRunner(QObject *a_parent_ptr = 0);

  virtual ~SocialTestRunner();

  void check_url_encode();

  void check_url_request_loader();

  void check_service_file();

  void testSocialPrefix();

  void testJSONHanlding();

  void testSocialPhotoSizes(const QString &photoID);

  void testsocialphotosizesJson(const QString &photoID);

  void testDirLoader(const QString &path);

  // webserver

  void startWebServer();

  void authDropBox();

  void getDropBoxAccountInfo(const QString &access_token, const QString &uid);

signals:

public
slots:
  void onServiceComplete(social_kit::web_service *service);
  void onSizeServiceComplete(social_kit::web_service *service);
  void onDownloadComplete(social_kit::web_service *service);
  void onImageReady();
  void onImageSaveReady();
  void onImageReady(const QString &fileName);

  // json
  void onServiceCompleteJson(const social_kit::web_service *service);
  void onSizeServiceCompleteJson(social_kit::web_service *service);
  void onDownloadCompleteJson(social_kit::web_service *service);
  void onImageReadyJson();
  void onImageSaveReadyJson();
  void onImageReadyJson(const QString &fileName);

  // webserver
  void onServerRequestCompleted(const QVariantMap &data);
  void onDropBoxRequestComplete(const QVariantMap &data);
  void onFinished(QNetworkReply *reply);

  // dropbox
  void onDropBoxAuthServiceComplete(social_kit::web_service *service);
  void
  onDropBoxAccountInfoServiceComplete(social_kit::web_service *service);

private:
  class PrivateSocialTestRunner;
  PrivateSocialTestRunner *const d;
};

#endif // SOCIALTESTRUNNER_H
