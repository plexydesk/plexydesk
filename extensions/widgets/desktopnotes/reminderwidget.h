#ifndef M_REMINDER_WIDGET_H
#define M_REMINDER_WIDGET_H

#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <plexy.h>
#include <style.h>
#include <widget.h>
#include <webservice.h>

class ReminderWidget : public UIKit::Widget {
  Q_OBJECT
public:
  explicit ReminderWidget(QGraphicsObject *a_parent_ptr = 0);

  virtual ~ReminderWidget();

  void setTitle(const QString &title);

  void setReminderWidgetContent(const QString &status);

  void setID(const QString &id);

  QString title() const;

  QString id();

  QString reminderContent() const;

  void setPixmap(const QPixmap &pixmap);

  void saveReminderToStore();

Q_SIGNALS:
  void clicked(ReminderWidget *item);

public
Q_SLOTS:
  void onClicked();
  void onTextUpdated(const QString &text);
  void onDocuemntTitleAvailable(const QString &title);

  // web service
  void onServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onDownloadCompleteJson(QuetzalSocialKit::WebService *service);
  void onImageReady();
  void onImageSaveReadyJson();
  void onImageReadyJson(const QString &fileName);

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private:
  class PrivateReminderWidget;
  PrivateReminderWidget *const d;

  void requestReminderSideImageFromWebService(const QString &key);
  void requestPhotoSizes(const QString &photoID);
  void initDataStore();
};

#endif // M_REMINDER_WIDGET_H
