
#include <config.h>

#include "notewidget.h"

#include <webservice.h>
#include <asyncimagecreator.h>
#include <asyncdatadownloader.h>

#include <ck_label.h>
#include <ck_config.h>
#include <ck_button.h>
#include <ck_image_view.h>
#include <ck_icon_button.h>
#include <ck_text_editor.h>
#include <ck_disk_engine.h>
#include <ck_sync_object.h>
#include <ck_icon_button.h>
#include <ck_fixed_layout.h>
#include <ck_resource_manager.h>
#include <ck_extension_manager.h>
#include <ck_memory_sync_engine.h>


typedef std::function<void(const QString &)> on_title_callback_func;
typedef std::function<void(const QString &, const QString &)>
on_config_callback_func;

class NoteWidget::PrivateNoteWidget {
public:
  PrivateNoteWidget() {}
  ~PrivateNoteWidget() {}

  void initDataStore();

  QString getContentText(const QString &data) const;
  void notify_config_change(const QString &a_key, const QString &a_value);

  QString m_note_id;

  QString m_note_title;
  QString mStatusMessage;

  QPixmap m_image_attachment;

  cherry_kit::text_editor *m_text_editor_widget;
  cherry_kit::icon_button *m_attachment_del_button;
  cherry_kit::image_view *m_image_attachment_view;

  QImage mBackgroundPixmap;

  cherry_kit::space *m_viewport;
  std::vector<on_title_callback_func> m_on_title_callback_func_list;
  std::vector<on_config_callback_func> m_on_config_callback_func_list;
  QVariantMap m_config_data;

  // new ui
  cherry_kit::fixed_layout *m_ui;

  std::function<void ()> m_on_delete_func;
};

void NoteWidget::createToolBar() {
  /*
d->m_note_toolbar_widget = new cherry_kit::ToolBar(this);
d->m_note_toolbar_widget->add_action("contact", "pd_add_contact_icon.png",
                                     false);∫
d->m_note_toolbar_widget->add_action("list", "pd_list_icon.png", false);
d->m_note_toolbar_widget->add_action("date", "pd_calendar_icon.png", false);
d->m_note_toolbar_widget->add_action("red", "pd_icon_red_color_action.png",
                                     false);
d->m_note_toolbar_widget->add_action(
    "yellow", "pd_icon_yellow_color_action.png", false);
d->m_note_toolbar_widget->add_action("green",
                                     "pd_icon_green_color_action.png", false);
d->m_note_toolbar_widget->add_action("blue", "pd_icon_blue_color_action.png",
                                     false);
d->m_note_toolbar_widget->add_action("black",
                                     "pd_icon_black_color_action.png", false);
d->m_note_toolbar_widget->add_action("delete", "actions/pd_delete.png",
                                     false);
                                     */
}

void NoteWidget::setViewport(cherry_kit::space *space) {
  d->m_viewport = space;
}

void NoteWidget::on_text_data_changed(
    std::function<void(const QString &)> a_callback) {
  d->m_on_title_callback_func_list.push_back(a_callback);
}

void NoteWidget::on_note_config_changed(
    std::function<void(const QString &, const QString &)> a_callback) {
  d->m_on_config_callback_func_list.push_back(a_callback);
}

void NoteWidget::on_note_deleted(std::function<void ()> a_callback) {
  d->m_on_delete_func = a_callback;
}

NoteWidget::NoteWidget(cherry_kit::session_sync *a_session,
                       cherry_kit::widget *parent)
    : cherry_kit::widget(parent), d(new PrivateNoteWidget) {

  if (!parent)
    return;

  d->m_ui = new cherry_kit::fixed_layout(this);
  d->m_ui->set_verticle_spacing(5);
  d->m_ui->set_content_margin(0, 0, 0, 0);
  d->m_ui->set_geometry(0, 0, 320, 240);

  d->m_ui->add_rows(2);
  d->m_ui->add_segments(0, 1);
  d->m_ui->add_segments(1, 9);
  d->m_ui->set_row_height(0, "88%");
  d->m_ui->set_row_height(1, "12%");

  cherry_kit::widget_properties_t text_editor_prop;
  text_editor_prop["text"] = "";

  d->m_text_editor_widget = dynamic_cast<cherry_kit::text_editor *>(
      d->m_ui->add_widget(0, 0, "text_edit", text_editor_prop));

  cherry_kit::widget_properties_t button_props;

  button_props["label"] = "";
  button_props["icon"] = "actions/pd_increase.png";
  d->m_ui->add_widget(1, 0, "image_button", button_props);

  button_props["label"] = "";
  button_props["icon"] = "actions/pd_increase.png";
  d->m_ui->add_widget(1, 1, "image_button", button_props);

  button_props["label"] = "";
  button_props["icon"] = "actions/pd_copy_text.png";
  cherry_kit::icon_button *copy_btn = dynamic_cast<cherry_kit::icon_button *>
    (d->m_ui->add_widget(1, 2, "image_button", button_props));
  copy_btn->on_click([this]() {});

  button_props["label"] = "";
  button_props["icon"] = "actions/pd_paste_text.png";
  cherry_kit::icon_button *paste_btn = dynamic_cast<cherry_kit::icon_button *>
    (d->m_ui->add_widget(1, 3, "image_button", button_props));
  paste_btn->on_click([this]() {});

  button_props["label"] = "";
  button_props["icon"] = "toolbar/ck_red_selection.png";
  cherry_kit::icon_button *red_btn = dynamic_cast<cherry_kit::icon_button *>
    (d->m_ui->add_widget(1, 4, "image_button", button_props));
  red_btn->on_click([this]() {
    exec_toolbar_action("red");
  });

  button_props["label"] = "";
  button_props["icon"] = "toolbar/ck_blue_selection.png";
  cherry_kit::icon_button *blue_btn = dynamic_cast<cherry_kit::icon_button *>
   (d->m_ui->add_widget(1, 5, "image_button", button_props));
  blue_btn->on_click([this]() {
    exec_toolbar_action("blue");
  });

  button_props["label"] = "";
  button_props["icon"] = "toolbar/ck_yellow_selection.png";
  cherry_kit::icon_button *yellow_btn =
      dynamic_cast<cherry_kit::icon_button *>
      (d->m_ui->add_widget(1, 6, "image_button", button_props));
  yellow_btn->on_click([this]() {
      exec_toolbar_action("yellow");
  });

  button_props["label"] = "";
  button_props["icon"] = "toolbar/ck_green_selection.png";
  cherry_kit::icon_button *green_btn  = dynamic_cast<cherry_kit::icon_button*>(
      d->m_ui->add_widget(1, 7, "image_button", button_props));
  green_btn->on_click([this]() {
      exec_toolbar_action("green");
  });

  button_props["label"] = "";
  button_props["icon"] = "toolbar/ck_delete.png";
  cherry_kit::icon_button *delete_btn = dynamic_cast<cherry_kit::icon_button*>
      (d->m_ui->add_widget(1, 8, "image_button", button_props));

  delete_btn->on_click([this]() {
    if (d->m_on_delete_func)
      d->m_on_delete_func();
  });

  connect(d->m_text_editor_widget, SIGNAL(documentTitleAvailable(QString)),
          this, SLOT(onDocuemntTitleAvailable(QString)));
  connect(d->m_text_editor_widget, SIGNAL(text_updated(QString)), this,
          SLOT(onTextUpdated(QString)));

  setAcceptDrops(true);
  setGeometry(parent->geometry());

  d->m_image_attachment_view = new cherry_kit::image_view(this);
  d->m_image_attachment_view->setMinimumSize(0, 0);

}

NoteWidget::~NoteWidget() { delete d; }

void NoteWidget::setTitle(const QString &name) {
  d->m_note_title = name;
  d->notify_config_change("title", name);
  update();
}

void NoteWidget::setNoteWidgetContent(const QString &status) {
  d->mStatusMessage = status;
  update();
}

void NoteWidget::set_editor_text(const QString &a_text) {
  d->m_text_editor_widget->set_text(a_text);
}

void NoteWidget::setID(const QString &id) { d->m_note_id = id; }

void NoteWidget::set_editor_color_scheme(const QString &a_fb_color,
                                         const QString &a_bg_color) {
  if (!d->m_text_editor_widget)
    return;
  d->m_text_editor_widget->style(QString("border: 0; background: %1; color: %2")
                                     .arg(a_bg_color)
                                     .arg(a_fb_color));
}

QString NoteWidget::title() const { return d->m_note_title; }

QString NoteWidget::id() { return d->m_note_id; }

QString NoteWidget::noteContent() const { return d->mStatusMessage; }

void NoteWidget::attach_image(const std::string &a_url) {

  QPixmap pixmap(a_url.c_str());

  prepareGeometryChange();

  if (d->m_image_attachment_view) {
    float image_width_ratio = 320.0 / pixmap.width();

    float image_height = pixmap.height() * image_width_ratio;
    float image_width = pixmap.width() * image_width_ratio;

    d->m_image_attachment_view->setMinimumSize(image_width, image_height);

    d->m_image_attachment_view->set_pixmap(
        pixmap.scaled(image_width, image_height));

    d->m_image_attachment_view->setGeometry(QRectF(0, 0, image_width, image_height));
    d->m_image_attachment_view->set_size(QSizeF(image_width, image_height));
    d->m_image_attachment_view->setPos(1, 0);
    d->m_image_attachment_view->update();
    d->m_image_attachment_view->updateGeometry();
    d->m_image_attachment_view->show();

    if (d->m_text_editor_widget) {
      d->m_text_editor_widget->setMinimumWidth(image_width + 1);
      d->m_ui->viewport()->setPos(1, image_height);
    }

    setGeometry(QRectF(0, 0, image_width + 2, image_height +
                       d->m_ui->viewport()->geometry().height()));

    d->notify_config_change("image", a_url.c_str());
  }

}

void NoteWidget::resize(const QSizeF &size) {
  /*
setGeometry(QRectF(0, 0, size.width(), size.height()));

d->mLayoutBase->setGeometry(geometry());

d->mSubLayoutBase->setGeometry(boundingRect());
d->m_text_editor_widget->setMaximumSize(geometry().size());
d->mMainVerticleLayout->setGeometry(
    QRectF(0.0, 0.0, size.width(), size.height()));
d->m_attachment_del_button->setPos(boundingRect().width() - 20, 72.0);

d->mMainVerticleLayout->updateGeometry();

d->mMainVerticleLayout->invalidate();

d->mSubLayout->invalidate();
d->mSubLayout->updateGeometry();
d->mSubLayout->activate();
*/
}

/*
void NoteWidget::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  cherry_kit::Widget::paint(painter, option, widget);

  painter->save();
  painter->setRenderHint(QPainter::SmoothPixmapTransform);
  painter->drawPixmap(
      QRectF(0, y(), option->exposedRect.width(), 300.0), d->m_image_attachment,
      QRectF((d->m_image_attachment.width() - option->exposedRect.width()) / 2,
0.0,
             option->exposedRect.width(), 300));
  painter->restore();
}
*/

void NoteWidget::dropEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << Q_FUNC_INFO << event->mimeData();
}

void NoteWidget::requestNoteSideImageFromWebService(const QString &key) {
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

void NoteWidget::requestPhotoSizes(const QString &photoID) {
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

void NoteWidget::onClicked() { Q_EMIT clicked(this); }

void NoteWidget::onTextUpdated(const QString &text) {
  QString save;
  if (d->m_note_title.isEmpty()) {
    save = text;
  } else {
    save = d->getContentText(text);
  }

  std::for_each(std::begin(d->m_on_title_callback_func_list),
                std::end(d->m_on_title_callback_func_list),
                [&](on_title_callback_func a_func) {
    if (a_func)
      a_func(save);
  });
}

void NoteWidget::onDocuemntTitleAvailable(const QString &title) {
  this->setTitle(title);
}

void NoteWidget::exec_toolbar_action(const QString &action) {
  qDebug() << Q_FUNC_INFO << action;
  if (action == tr("date")) {

    if (d->m_viewport) {
      QPointF window_pos(mapToScene(QPointF()));

      QRectF window_geometry(window_pos.x(), window_pos.y(), 300, 348);

      d->m_viewport->open_desktop_dialog(
          "datepickeractivity", tr("Calendar"),
          d->m_viewport->center(window_geometry, window_geometry,
                                cherry_kit::space::kCenterOnWindow),
          window_geometry, QVariantMap());
    }

  } else if (action == tr("list")) {
    d->m_text_editor_widget->begin_list();
  } else if (action == tr("link")) {
    d->m_text_editor_widget->convert_to_link();
  } else if (action == tr("red")) {
    d->m_text_editor_widget->style(
        "border: 0; background: #D55521; color: #ffffff");
    d->notify_config_change("background", "#D55521");
    d->notify_config_change("forground", "#ffffff");
  } else if (action == tr("yellow")) {
    d->m_text_editor_widget->style(
        "border: 0; background: #E6DA42; color: #000000");
    d->notify_config_change("background", "#e6da42");
    d->notify_config_change("forground", "#000000");
  } else if (action == tr("green")) {
    d->m_text_editor_widget->style(
        "border: 0; background: #29CDA8; color: #ffffff");
    d->notify_config_change("background", "#29cda8");
    d->notify_config_change("forground", "#ffffff");
  } else if (action == tr("blue")) {
    d->m_text_editor_widget->style(
        "border: 0; background: #0AACF0; color: #ffffff");
    d->notify_config_change("background", "#0AACF0");
    d->notify_config_change("forground", "#ffffff");
  } else if (action == tr("black")) {
    d->m_text_editor_widget->style(
        "border: 0; background: #4A4A4A; color: #ffffff");
    d->notify_config_change("background", "#4A4A4A");
    d->notify_config_change("forground", "#ffffff");
  } else if (action == tr("delete")) {
    this->hide();
  }
}

void NoteWidget::onServiceCompleteJson(QuetzalSocialKit::WebService *service) {
  QList<QVariantMap> photoList = service->methodData("photo");

  Q_FOREACH(const QVariantMap & map, photoList) {
    requestPhotoSizes(map["id"].toString());
  }

  service->deleteLater();
}

void
NoteWidget::onSizeServiceCompleteJson(QuetzalSocialKit::WebService *service) {
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

void NoteWidget::onDownloadCompleteJson(QuetzalSocialKit::WebService *service) {
}

void NoteWidget::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReadyJson()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), cherry_kit::config::cache_dir(),
                       true);
    imageSave->setCrop(QRectF(0.0, 0.0, 300.0, 300.0));
    imageSave->start();

    downloader->deleteLater();
  }
}

void NoteWidget::onImageSaveReadyJson() {
  /*
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    d->mBackgroundPixmap = c->image();

    update();
    c->quit();
    c->deleteLater();
  }
  */
}

void NoteWidget::onImageReadyJson(const QString &fileName) {
  qDebug() << Q_FUNC_INFO << fileName;
}

void NoteWidget::deleteImageAttachment() {
 /*
d->m_image_attachment = QPixmap();

this->prepareGeometryChange();
this->setGeometry(QRectF(0.0, 0.0, this->boundingRect().width(), 300));

if (d->m_image_attachment.isNull()) {
  d->m_text_editor_widget->setPos(d->m_text_editor_widget->pos().x(),
                                  d->m_text_editor_widget->pos().y() - 300);
  d->m_note_toolbar_widget->setPos(d->m_note_toolbar_widget->pos().x(),
                                   d->m_note_toolbar_widget->pos().y() - 300);
}

d->m_attachment_del_button->hide();

update();
*/
}

QString
NoteWidget::PrivateNoteWidget::getContentText(const QString &data) const {
  QStringList dataList = data.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

  QString rv;
  for (int i = 1; i < dataList.count(); i++) {
    rv += dataList.at(i);
  }

  return rv;
}

void
NoteWidget::PrivateNoteWidget::notify_config_change(const QString &a_key,
                                                    const QString &a_value) {
  std::for_each(std::begin(m_on_config_callback_func_list),
                std::end(m_on_config_callback_func_list),
                [&](on_config_callback_func a_func) {
    if (a_func)
      a_func(a_key, a_value);
  });
}
