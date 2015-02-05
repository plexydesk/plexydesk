#ifndef M_NOTE_WIDGET_H
#define M_NOTE_WIDGET_H

#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <plexy.h>
#include <style.h>
#include <widget.h>
#include <webservice.h>
#include <desktopactivity.h>

class NoteWidget : public UI::Window
{
  Q_OBJECT
public:
  explicit NoteWidget(QGraphicsObject *parent = 0);

  virtual ~NoteWidget();

  void setTitle(const QString &title);

  void setNoteWidgetContent(const QString &status);

  void setID(const QString &id);

  QString title() const;

  QString id();

  QString noteContent() const;

  void setPixmap(const QPixmap &pixmap);

  void saveNoteToStore();

  void resize(const QSizeF &size);

  void createToolBar();
Q_SIGNALS:
  void clicked(NoteWidget *item);

public Q_SLOTS:
  void onClicked();
  void onTextUpdated(const QString &text);
  void onDocuemntTitleAvailable(const QString &title);
  void onToolBarAction(const QString &action);

  // web service
  void onServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service);
  void onDownloadCompleteJson(QuetzalSocialKit::WebService *service);
  void onImageReady();
  void onImageSaveReadyJson();
  void onImageReadyJson(const QString &fileName);
  void deleteImageAttachment();
  void onDatePickerDone();

  void onActivityClosed();

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
  virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
  class PrivateNoteWidget;
  PrivateNoteWidget *const d;

  void requestNoteSideImageFromWebService(const QString &key);
  void requestPhotoSizes(const QString &photoID);
  void initDataStore();
  UI::DesktopActivityPtr showCalendar(const QString &activity,
                                      const QString &title,
                                      const QVariantMap &dataItem);
};

#endif // M_NOTE_WIDGET_H
