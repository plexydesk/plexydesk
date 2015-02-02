#include <config.h>
#include "reminderwidget.h"
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QDateTime>
#include <QGraphicsWidget>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <button.h>
#include <imageview.h>
#include <texteditor.h>
#include <label.h>
#include <themepackloader.h>
#include <webservice.h>
#include <datastore.h>
#include <memorysyncengine.h>
#include <disksyncengine.h>
#include <syncobject.h>
#include <plexyconfig.h>

class ReminderWidget::PrivateReminderWidget {
public:
  PrivateReminderWidget() {}
  ~PrivateReminderWidget() {}

  void initDataStore();

  QString getContentText(const QString &data) const;

  UI::Style *mStyle;
  QString mReminderTitle;
  QString mStatusMessage;
  QPixmap mPixmap;
  QString mID;
  QPixmap mAvatar;

  UI::Button *mButton;
  UI::TextEditor *mTextEdit;
  UI::Label *mLable;

  QGraphicsWidget *mLayoutBase;
  QGraphicsWidget *mSubLayoutBase;
  QGraphicsLinearLayout *mMainVerticleLayout;
  QGraphicsLinearLayout *mSubLayout;
  QImage mBackgroundPixmap;

  // datastore
  QuetzalKit::DataStore *mDataStore;           // main data store.
  QuetzalKit::SyncObject *mReminderListObject; // contains the list of
                                               // Reminders.
  QuetzalKit::SyncObject *mCurrentReminderObject;

  QuetzalKit::SyncObject *getReminderObject();
};

ReminderWidget::ReminderWidget(QGraphicsObject *parent)
    : UI::UIWidget(parent), d(new PrivateReminderWidget) {
  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setWindowFlag(UI::UIWidget::kRenderDropShadow, true);
  setWindowFlag(UI::UIWidget::kConvertToWindowType, true);

  d->mLayoutBase = new QGraphicsWidget(this);
  d->mLayoutBase->setGeometry(this->boundingRect());

  d->mSubLayoutBase = new QGraphicsWidget(d->mLayoutBase);
  d->mSubLayoutBase->setGeometry(
      QRectF(64.0, 0.0, boundingRect().width(), boundingRect().height()));

  d->mMainVerticleLayout = new QGraphicsLinearLayout(d->mLayoutBase);
  d->mMainVerticleLayout->setOrientation(Qt::Horizontal);
  d->mMainVerticleLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
  d->mLayoutBase->setPos(0.0, 0.0);
  d->mMainVerticleLayout->setGeometry(QRectF(
      0.0, 0.0, this->boundingRect().width(), this->boundingRect().height()));

  d->mSubLayout = new QGraphicsLinearLayout(d->mSubLayoutBase);
  d->mSubLayout->setOrientation(Qt::Vertical);
  d->mSubLayout->setContentsMargins(64.0, 0.0, 0.0, 0.0);

  d->mLable = new UI::Label(d->mSubLayoutBase);
  d->mLable->setLabel(QDateTime::currentDateTime().toString());
  d->mLable->setSize(QSizeF(this->boundingRect().width() - 64.0, 24));
  d->mLable->setLabelStyle(QColor(254, 254, 254, 128), QColor(0, 0, 0));
  d->mTextEdit = new UI::TextEditor(d->mSubLayoutBase);
  d->mTextEdit->style(
      "border: 0; background: rgba(0,0,0,0); color: rgb(255, 255, 255)");

  d->mSubLayout->addItem(d->mTextEdit);
  d->mSubLayout->addItem(d->mLable);
  d->mMainVerticleLayout->addItem(d->mSubLayoutBase);

  d->mTextEdit->setPlaceholderText("Title :");

  connect(d->mTextEdit, SIGNAL(documentTitleAvailable(QString)), this,
          SLOT(onDocuemntTitleAvailable(QString)));
  connect(d->mTextEdit, SIGNAL(textUpdated(QString)), this,
          SLOT(onTextUpdated(QString)));

  initDataStore();
}

ReminderWidget::~ReminderWidget() { delete d; }

void ReminderWidget::setTitle(const QString &name) {
  d->mReminderTitle = name;
  update();

  d->mCurrentReminderObject->setObjectAttribute("title", name);
  d->mDataStore->updateNode(d->mCurrentReminderObject);

  requestReminderSideImageFromWebService(d->mReminderTitle);
}

void ReminderWidget::initDataStore() {
  d->mDataStore = new QuetzalKit::DataStore("desktopreminders", this);
  QuetzalKit::DiskSyncEngine *engine =
      new QuetzalKit::DiskSyncEngine(d->mDataStore);

  d->mDataStore->setSyncEngine(engine);

  d->mReminderListObject = d->mDataStore->begin("ReminderList");
  d->mCurrentReminderObject =
      d->mReminderListObject->createNewObject("Reminder");
  d->mCurrentReminderObject->setObjectAttribute("title", "");

  d->mDataStore->insert(d->mCurrentReminderObject);
}

void ReminderWidget::setReminderWidgetContent(const QString &status) {
  d->mStatusMessage = status;
  update();
}

void ReminderWidget::setID(const QString &id) { d->mID = id; }

QString ReminderWidget::title() const { return d->mReminderTitle; }

QString ReminderWidget::id() { return d->mID; }

QString ReminderWidget::reminderContent() const { return d->mStatusMessage; }

void ReminderWidget::setPixmap(const QPixmap &pixmap) {
  d->mPixmap = pixmap;
  update();
}

void ReminderWidget::saveReminderToStore() {
  QuetzalKit::DataStore *store =
      new QuetzalKit::DataStore("deskopreminders", this);

  QuetzalKit::DiskSyncEngine *diskEngine = new QuetzalKit::DiskSyncEngine(this);
  store->setSyncEngine(diskEngine);

  QuetzalKit::SyncObject *object = store->rootObject();

  bool newObject = false;
  if (object->name().isEmpty()) {
    object->setName("reminderlist");
    object->setKey(10);
    newObject = true;
    // object->setObjectAttribute("src", "http://www.google.com");
  }

  // child object
  QuetzalKit::SyncObject reminderObject;

  reminderObject.setName("reminder");
  reminderObject.setObjectAttribute("content", d->mTextEdit->text());
  // meta object

  QuetzalKit::SyncObject metaData;

  metaData.setName("metadata");
  metaData.setKey(100);
  metaData.setObjectAttribute("compression", "PNG");
  metaData.setObjectAttribute("location", "Colomobo, Sri Lanka");

  reminderObject.addChildObject(&metaData);
  if (!newObject)
    object->addChildObject(&reminderObject);

  store->addObject(object);
}

void ReminderWidget::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget) {
  painter->save();
  painter->setRenderHint(QPainter::SmoothPixmapTransform);
  // painter->fillRect(option->exposedRect, QColor(10, 172, 240));
  painter->fillRect(option->exposedRect, QColor(216, 84, 17));
  painter->drawImage(QRectF(0.0, 0.0, 64, this->boundingRect().height()),
                     d->mBackgroundPixmap);
  painter->restore();
}

void ReminderWidget::requestReminderSideImageFromWebService(
    const QString &key) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = key;
  args["per_page"] = QString::number(1);
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers,banners";
  args["tag_mode"] = "all";
  args["page"] = QString::number(1);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void ReminderWidget::requestPhotoSizes(const QString &photoID) {
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["photo_id"] = photoID;

  service->queryService("flickr.photos.getSizes", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onSizeServiceCompleteJson(QuetzalSocialKit::WebService *)));
}

void ReminderWidget::onClicked() { Q_EMIT clicked(this); }

void ReminderWidget::onTextUpdated(const QString &text) {
  if (d->mReminderTitle.isEmpty())
    return;

  d->getContentText(text);
  d->mCurrentReminderObject->setTextData(d->getContentText(text));
  d->mDataStore->updateNode(d->mCurrentReminderObject);
}

void ReminderWidget::onDocuemntTitleAvailable(const QString &title) {
  this->setTitle(title);
}

void ReminderWidget::onServiceCompleteJson(
    QuetzalSocialKit::WebService *service) {
  QList<QVariantMap> photoList = service->methodData("photo");

  Q_FOREACH(const QVariantMap & map, photoList) {
    requestPhotoSizes(map["id"].toString());
  }

  service->deleteLater();
  ;
}

void ReminderWidget::onSizeServiceCompleteJson(
    QuetzalSocialKit::WebService *service) {
  Q_FOREACH(const QVariantMap & map, service->methodData("size")) {
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

void ReminderWidget::onDownloadCompleteJson(
    QuetzalSocialKit::WebService *service) {}

void ReminderWidget::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReadyJson()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), UI::Config::cacheDir(), true);
    imageSave->setCrop(QRectF(100, 0.0, 64.0, boundingRect().height()));
    imageSave->start();

    downloader->deleteLater();
  }
}

void ReminderWidget::onImageSaveReadyJson() {
  qDebug() << Q_FUNC_INFO;
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    d->mBackgroundPixmap = c->image();
    update();
    c->quit();
    c->deleteLater();
  }
}

void ReminderWidget::onImageReadyJson(const QString &fileName) {
  qDebug() << Q_FUNC_INFO << fileName;
}

QString ReminderWidget::PrivateReminderWidget::getContentText(
    const QString &data) const {
  QStringList dataList = data.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

  QString rv;
  for (int i = 1; i < dataList.count(); i++) {
    rv += dataList.at(i);
  }

  return rv;
}

QuetzalKit::SyncObject *
ReminderWidget::PrivateReminderWidget::getReminderObject() {
  return 0;
}
