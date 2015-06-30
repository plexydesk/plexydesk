/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
*  Authored By  : *
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "date.h"
#include <QGraphicsLinearLayout>
#include <calendarwidget.h>
#include <item_view.h>
#include <toolbar.h>
#include <label.h>
#include <plexyconfig.h>
#include <session_sync.h>
#include <imagebutton.h>
#include <resource_manager.h>
#include <window.h>
#include <viewbuilder.h>

class DateControllerImpl::PrivateDate {
public:
  PrivateDate() {}
  ~PrivateDate() {}

  UIKit::ActionList m_supported_action_list;
};

DateControllerImpl::DateControllerImpl(QObject *object)
    : UIKit::ViewController(object), d(new PrivateDate) {}

DateControllerImpl::~DateControllerImpl() { delete d; }

void DateControllerImpl::init() {
  QAction *_add_note_action = new QAction(this);
  _add_note_action->setText(tr("Calendar"));

  _add_note_action->setProperty("id", QVariant(1));
  _add_note_action->setProperty("icon_name", "pd_add_note_frame_icon.png");

  d->m_supported_action_list << _add_note_action;
}

void DateControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_session_root) {
  revoke_previous_session("Calendar",
                          [this](UIKit::ViewController *a_controller,
                                 UIKit::SessionSync *a_session) {
    create_ui_calendar_ui(a_session);
  });
}

void DateControllerImpl::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  write_session_data("Calendar");
}

void DateControllerImpl::set_view_rect(const QRectF &a_rect) {}

bool DateControllerImpl::remove_widget(UIKit::Widget *a_widget_ptr) {
  return false;
}

UIKit::ActionList DateControllerImpl::actions() const {
  return d->m_supported_action_list;
}

void DateControllerImpl::request_action(const QString &a_name,
                                        const QVariantMap &a_args) {
  QPointF window_location;

  if (viewport()) {
    window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
  }

  QVariantMap session_args;

  if (a_name == tr("Calendar")) {
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["calendar_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("calendar"));

    start_session("Calendar", session_args, false,
                  [this](UIKit::ViewController *a_controller,
                         UIKit::SessionSync *a_session) {
      create_ui_calendar_ui(a_session);
    });
  }
}

QString DateControllerImpl::icon() const { return QString(); }

void DateControllerImpl::add_action_button(UIKit::ViewBuilder *ui,
                                           int a_row,
                                           int a_col,
                                           const std::string &a_label,
                                           const std::string &a_icon)
{
    UIKit::ViewProperties ui_data;
    ui_data["label"] = a_label;
    ui_data["icon"] = "actions/" + a_icon + ".png";
    ui->add_widget(a_row, a_col, "image_button", ui_data);
}

void DateControllerImpl::create_ui_calendar_ui(UIKit::SessionSync *a_session) {

  UIKit::Window *window = new UIKit::Window();

  UIKit::ViewBuilder *ui = new UIKit::ViewBuilder(window);

  ui->set_margine(10, 10, 10, 10);
  ui->set_geometry(0, 0, 360, 480);

  ui->set_row_count(2);
  ui->split_row(0, 1);
  ui->split_row(1, 3);

  ui->set_row_height(0, "95%");
  ui->set_row_height(1, "5%");

  UIKit::ViewProperties ui_data;
  ui_data["text"] + "";

  ui->add_widget(0, 0, "calendar", ui_data);

  add_action_button(ui, 1, 0, "Zoom In", "pd_zoom_in");
  add_action_button(ui, 1, 1, "Zoom Out", "pd_zoom_out");
  add_action_button(ui, 1, 2, "Tasks", "pd_view_list");


  window->set_window_content(ui->ui());

  a_session->bind_to_window(window);
  window->on_window_discarded([this](UIKit::Window *aWindow) {
    delete aWindow;
  });

  if (viewport()) {
    insert(window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}

void DateControllerImpl::create_calendar_ui(UIKit::SessionSync *a_session) {
  UIKit::Window *window = new UIKit::Window();
  window->setGeometry(QRectF(0, 0, 320, 480));
  window->set_window_title("Calendar");

  UIKit::Widget *content_frame = new UIKit::Widget(window);
  content_frame->setGeometry(QRectF(0, 0, 320, 320 + (48 * 3) + 48));

  UIKit::CalendarView *calendar = new UIKit::CalendarView(content_frame);
  calendar->setGeometry(QRectF(0, 0, 320, 320));

  UIKit::ToolBar *toolbar = new UIKit::ToolBar(content_frame);
  toolbar->set_icon_resolution("hdpi");
  toolbar->set_icon_size(QSize(24, 24));
  toolbar->setMinimumSize(320, 48);
  toolbar->setGeometry(QRectF(0, 0, 320, 48));

  UIKit::ItemView *todo_list = new UIKit::ItemView(content_frame);
  todo_list->setGeometry(QRectF(0, 0, 320, 48 * 3));
  todo_list->set_view_geometry(QRectF(0, 0, 320, 48 * 3));

  UIKit::Label *today_label = new UIKit::Label(content_frame);
  today_label->setGeometry(QRectF(0, 0, 240, 240));
  today_label->set_size(QSizeF(240, 240));
  today_label->set_font_size(128);
  today_label->set_label(QString("%1").arg(QDate::currentDate().day()));

  today_label->setPos(0, 0);
  calendar->setPos(15, 0);
  todo_list->setPos(0, calendar->geometry().height());
  toolbar->setPos(0, today_label->boundingRect().height());

  window->set_window_content(content_frame);

  // only show the label;
  calendar->hide();
  todo_list->hide();

  window->resize(240, 240 + toolbar->boundingRect().height());
  window->set_window_title(
      QDate::longDayName(QDate::currentDate().dayOfWeek()));

  a_session->bind_to_window(window);

  todo_list->on_activated([&](int a_index) {
    UIKit::ModelViewItem *item = todo_list->at(a_index);

    if (!item)
      return;
  });

  toolbar->on_item_activated([=](const QString &a_action) {
    qDebug() << Q_FUNC_INFO << "Toolbar Action : " << a_action;
    if (a_action == "ZoomIn") {
      today_label->hide();
      calendar->show();
      todo_list->hide();
      window->resize(320, calendar->boundingRect().height() +
                              toolbar->frame_geometry().height());

      toolbar->setPos(0, calendar->boundingRect().height());
      return;
    }

    if (a_action == "ZoomOut") {
      calendar->hide();
      today_label->show();
      todo_list->hide();

      window->resize(240, today_label->boundingRect().height() +
                              toolbar->boundingRect().height());

      toolbar->setPos(0, today_label->boundingRect().height());
      return;
    }

    if (a_action == "List") {
      todo_list->show();
      today_label->hide();
      calendar->show();
      window->resize(320, calendar->boundingRect().height() +
                              todo_list->boundingRect().height() +
                              toolbar->boundingRect().height());
      toolbar->setPos(0, calendar->boundingRect().height() +
                             todo_list->boundingRect().height());
    }
  });

  toolbar->add_action("ZoomIn", "actions/pd_zoom_in", false);
  toolbar->add_action("ZoomOut", "actions/pd_zoom_out", false);
  toolbar->add_action("List", "actions/pd_view_list", false);

  for (int i = 0; i < 10; i++) {
    UIKit::ModelViewItem *item = new UIKit::ModelViewItem();
    UIKit::Widget *item_view = new UIKit::Widget(todo_list);
    item_view->setGeometry(QRectF(0, 0, 320, 48));
    item_view->setMinimumSize(QSizeF(320, 48));

    UIKit::Label *todo_label = new UIKit::Label(item_view);
    todo_label->set_size(QSizeF(320 - 48, 48));
    todo_label->setMinimumSize(320 - 48, 48);
    todo_label->set_label(QString(" Task Number :# %1").arg(i));
    todo_label->set_alignment(Qt::AlignLeft);

    UIKit::ImageButton *todo_icon = new UIKit::ImageButton(item_view);
    todo_icon->set_pixmap(UIKit::ResourceManager::instance()->drawable(
        "pd_reminder_icon.png", "hdpi"));
    todo_icon->set_size(QSize(24, 24));
    todo_icon->setMinimumSize(QSizeF(24, 24));
    todo_icon->setGeometry(QRectF(0, 0, 24, 24));

    item_view->show();
    todo_icon->setPos(10, 0);
    todo_label->setPos(48, 0);

    item->set_view(item_view);

    todo_list->insert(item);
  }

  if (a_session->session_keys().contains("todo")) {
    //note->set_editor_text(a_session->session_data("text").toString());
  }

  /*
  note->on_geometry_changed([=](const QRectF &a_geometry) {
    if (window) {
      window->resize(a_geometry.width(), a_geometry.height());
    }
  });
  */

  window->on_window_discarded([this](UIKit::Window *aWindow) {
    delete aWindow;
  });

  if (viewport()) {
    insert(window);
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}
