#ifndef SOCIALTESTRUNNER_H
#define SOCIALTESTRUNNER_H

#include <QObject>
#include <webservice.h>

class SocialTestRunner : public QObject {
  Q_OBJECT
public:
  explicit SocialTestRunner(QObject *parent = 0);

  virtual ~SocialTestRunner();

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

public slots:
  void onServiceComplete(QuetzalSocialKit::WebService *service);
  void onSizeServiceComplete(QuetzalSocialKit::WebService *service);
  void onDownloadComplete(QuetzalSocialKit::WebService *service);
  void onImageReady();
  void onImageSaveReady();
  void onImageReady(const QString &fileName);

  // json
  void onServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onDownloadCompleteJson(QuetzalSocialKit::WebService *service);
  void onImageReadyJson();
  void onImageSaveReadyJson();
  void onImageReadyJson(const QString &fileName);

  // webserver
  void onServerRequestCompleted(const QVariantMap &data);
  void onDropBoxRequestComplete(const QVariantMap &data);
  void onFinished(QNetworkReply *reply);

  // dropbox
  void onDropBoxAuthServiceComplete(QuetzalSocialKit::WebService *service);
  void onDropBoxAccountInfoServiceComplete(
      QuetzalSocialKit::WebService *service);

private:
  class PrivateSocialTestRunner;
  PrivateSocialTestRunner *const d;
};

#endif // SOCIALTESTRUNNER_H
