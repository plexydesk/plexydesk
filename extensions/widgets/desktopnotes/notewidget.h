#ifndef M_NOTE_WIDGET_H
#define M_NOTE_WIDGET_H

#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <plexy.h>
#include <style.h>
#include <widget.h>
#include <webservice.h>
#include <desktopactivity.h>
#include <session_sync.h>

class NoteWidget : public CherryKit::Widget {
  Q_OBJECT
public:
  explicit NoteWidget(CherryKit::SessionSync *a_session,
                      CherryKit::Widget *a_parent_ptr = 0);
  virtual ~NoteWidget();

  void setTitle(const QString &title);
  QString title() const;

  void setNoteWidgetContent(const QString &status);

  void set_editor_text(const QString &a_text);

  void setID(const QString &id);

  void set_editor_color_scheme(const QString &a_fb_color,
                               const QString &a_bg_color);

  QString id();
  QString noteContent() const;

  void setPixmap(const QPixmap &pixmap);

  void resize(const QSizeF &size);
  void createToolBar();

  void setViewport(CherryKit::Space *space);

  void on_text_data_changed(std::function<void(const QString &)> a_callback);
  void on_note_config_changed(
      std::function<void(const QString &, const QString &)> a_callback);
Q_SIGNALS:
  void clicked(NoteWidget *item);

public
Q_SLOTS:
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

protected:
  /*
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
                     */
  virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
  class PrivateNoteWidget;
  PrivateNoteWidget *const d;

  void requestNoteSideImageFromWebService(const QString &key);
  void requestPhotoSizes(const QString &photoID);
};

#endif // M_NOTE_WIDGET_H
