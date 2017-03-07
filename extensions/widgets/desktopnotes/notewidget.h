#ifndef M_NOTE_WIDGET_H
#define M_NOTE_WIDGET_H

#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>

#include <ck_style.h>
#include <ck_widget.h>

#include <ck_desktop_dialog.h>
#include <ck_session_sync.h>

class NoteWidget : public cherry_kit::widget {
  Q_OBJECT
public:
  explicit NoteWidget(cherry_kit::session_sync *a_session,
                      cherry_kit::widget *a_parent_ptr = 0);
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

  void attach_image(const std::string &a_pixmap);

  void resize(const QSizeF &size);
  void createToolBar();

  void setWindow(cherry_kit::window *a_window);
  void setViewport(cherry_kit::space *space);

  void on_text_data_changed(std::function<void(const QString &)> a_callback);
  void on_note_config_changed(
      std::function<void(const QString &, const QString &)> a_callback);
  void on_note_deleted(std::function<void()> a_callback);

Q_SIGNALS:
  void clicked(NoteWidget *item);

public
Q_SLOTS:
  void onClicked();
  void onTextUpdated(const QString &text);
  void onDocuemntTitleAvailable(const QString &title);
  void exec_toolbar_action(const QString &action);

#ifdef __QT5_TOOLKIT__
  // web service
  void onServiceCompleteJson(social_kit::web_service *service);
  void onSizeServiceCompleteJson(social_kit::web_service *service);
  void onDownloadCompleteJson(social_kit::web_service *service);
  void onImageReady();
  void onImageSaveReadyJson();
  void onImageReadyJson(const QString &fileName);
#endif

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
