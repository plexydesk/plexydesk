/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include "desktopnotes.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_widget.h>
#include <ck_session_sync.h>

#include <ck_fixed_layout.h>

#include <ck_button.h>
#include <ck_icon_button.h>
#include <ck_text_editor.h>

#include "notewidget.h"

class desktop_task_controller_impl::PrivateDesktopNotes {
public:
  PrivateDesktopNotes() {}
  ~PrivateDesktopNotes() {}
};

desktop_task_controller_impl::desktop_task_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new PrivateDesktopNotes) {}

desktop_task_controller_impl::~desktop_task_controller_impl() { delete priv; }

void desktop_task_controller_impl::init() {}

void desktop_task_controller_impl::session_data_ready(
    const cherry_kit::sync_object &a_sesion_root) {
  revoke_previous_session(
      "Notes",
      [this](cherry_kit::desktop_controller_interface *a_controller,
             cherry_kit::session_sync *a_session) { createNoteUI(a_session); });

  revoke_previous_session(
      "Reminders",
      [this](cherry_kit::desktop_controller_interface *a_controller,
             cherry_kit::session_sync *a_session) {
        createReminderUI(a_session);
      });
}

void desktop_task_controller_impl::submit_session_data(
    cherry_kit::sync_object *a_obj) {
  write_session_data("Notes");
  write_session_data("Reminders");
}

void desktop_task_controller_impl::set_view_rect(const QRectF &rect) {}

cherry_kit::ui_action desktop_task_controller_impl::task() {
  cherry_kit::ui_action task;
  task.set_name("Record");
  task.set_visible(1);
  task.set_controller(controller_name().toStdString());
  task.set_icon("navigation/ck_do.png");

  cherry_kit::ui_action note_action;
  note_action.set_name("Note");
  note_action.set_visible(1);
  note_action.set_id(1);
  note_action.set_icon("panel/ck_add.png");
  note_action.set_controller(controller_name().toStdString());
  note_action.set_task([this](const cherry_kit::ui_action *a_action_ref,
                              const cherry_kit::ui_task_data_t &a_data) {
    QPointF window_location;

    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
    }

    QVariantMap session_args;
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["notes_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("Notes"));

    start_session("Notes", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      createNoteUI(a_session);
    });
  });

  cherry_kit::ui_action task_action;
  task_action.set_name("Task");
  task_action.set_visible(1);
  task_action.set_id(2);
  task_action.set_icon("panel/ck_add.png");
  task_action.set_controller(controller_name().toStdString());
  task_action.set_task([this](const cherry_kit::ui_action *a_action_ref,
                              const cherry_kit::ui_task_data_t &a_data) {
    QPointF window_location;

    if (viewport()) {
      window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
    }

    QVariantMap session_args;
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["reminders_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_store_name("reminders"));

    start_session("Reminders", session_args, false,
                  [this](cherry_kit::desktop_controller_interface *a_controller,
                         cherry_kit::session_sync *a_session) {
      createReminderUI(a_session);
    });
  });

  task.add_action(note_action);
  task.add_action(task_action);

  return task;
}

void desktop_task_controller_impl::handle_drop_event(cherry_kit::widget *widget,
                                                     QDropEvent *event) {
  const QString drop_file_name =
      event->mimeData()->urls().value(0).toLocalFile();
  QFileInfo fileInfo(drop_file_name);

  if (fileInfo.isFile()) {
    QPixmap image(drop_file_name);
    NoteWidget *note = qobject_cast<NoteWidget *>(widget);
    if (note) {
      note->attach_image(drop_file_name.toStdString());
    }
  }
}

void desktop_task_controller_impl::createNoteUI(
    cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  window->setGeometry(QRectF(0, 0, 320, 240));

  NoteWidget *note = new NoteWidget(a_session, window);
  note->set_controller(this);
  note->setViewport(viewport());
  // window->setGeometry(note->geometry());
  window->set_window_title("Note");
  window->set_window_content(note);

  a_session->bind_to_window(window);

  if (a_session->session_keys().contains("text")) {
    note->set_editor_text(a_session->session_data("text").toString());
  }

  if (a_session->session_keys().contains("background") &&
      a_session->session_keys().contains("forground")) {
    QString backgorund_color = a_session->session_data("background").toString();
    QString forground_color = a_session->session_data("forground").toString();

    note->set_editor_color_scheme(forground_color, backgorund_color);
  }

  note->on_text_data_changed([=](const QString &a_text) {
    a_session->save_session_attribute(
        session_store_name("notes"), "Notes", "notes_id",
        a_session->session_id_to_string(), "text", a_text.toStdString());
  });

  note->on_geometry_changed([=](const QRectF &a_geometry) {
    if (window) {
      window->resize(a_geometry.width(), a_geometry.height());
    }
  });

  if (a_session->session_keys().contains("image")) {
    note->attach_image(
        a_session->session_data("image").toString().toStdString());
  }

  note->on_note_config_changed([=](const QString &a_key,
                                   const QString &a_value) {
    a_session->save_session_attribute(
        session_store_name("notes"), "Notes", "notes_id",
        a_session->session_id_to_string(), a_key.toStdString(),
        a_value.toStdString());
  });

  note->on_note_deleted([=]() {
    a_session->unbind_window(window);
    window->close();
  });

  window->on_window_discarded([this](cherry_kit::window *aWindow) {
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

void desktop_task_controller_impl::createReminderUI(
    cherry_kit::session_sync *a_session) {
  cherry_kit::window *window = new cherry_kit::window();
  cherry_kit::fixed_layout *view = new cherry_kit::fixed_layout(window);

  view->set_content_margin(0, 0, 0, 5);
  view->set_geometry(0, 0, 320, 160);

  view->add_rows(2);
  view->add_segments(0, 1);
  view->add_segments(1, 4);

  view->set_row_height(0, "85%");
  view->set_row_height(1, "15%");

  cherry_kit::widget_properties_t top_label_prop;
  top_label_prop["label"] = "reminder";

  cherry_kit::widget_properties_t text_editor_prop;
  text_editor_prop["text"] = "";

  if (a_session->session_keys().contains("text")) {
    text_editor_prop["text"] =
        std::string(a_session->session_data("text").toByteArray());
  }

  cherry_kit::widget_properties_t accept_button_prop;

  view->add_widget(0, 0, "text_edit", text_editor_prop, [=]() {});

  accept_button_prop["label"] = "";
  accept_button_prop["icon"] = "toolbar/ck_calendar.png";
  view->add_widget(1, 0, "image_button", accept_button_prop, [=]() {});

  if (a_session->session_keys().contains("state") &&
      (a_session->session_data("state").toString() == "done")) {
    accept_button_prop["label"] = "";
    accept_button_prop["icon"] = "toolbar/ck_checkmark2.png";

    cherry_kit::text_editor *editor =
        dynamic_cast<cherry_kit::text_editor *>(view->at(0, 0));

    if (editor) {
      editor->style("border: 0; background: #29CDA8; color: #ffffff");
    }
  } else {
    accept_button_prop["label"] = "";
    accept_button_prop["icon"] = "toolbar/ck_checkmark.png";
  }

  view->add_widget(1, 1, "image_button", accept_button_prop,[=]() {});

  accept_button_prop["label"] = "";
  accept_button_prop["icon"] = "toolbar/ck_floppy-disk.png";
  view->add_widget(1, 2, "image_button", accept_button_prop, [=]() {});

  accept_button_prop["label"] = "";
  accept_button_prop["icon"] = "toolbar/ck_bin.png";
  view->add_widget(1, 3, "image_button", accept_button_prop, [=]() {});

  window->set_window_content(view->viewport());
  window->set_window_title("Task");

  a_session->bind_to_window(window);
  insert(window);

  cherry_kit::icon_button *delete_btn =
      dynamic_cast<cherry_kit::icon_button *>(view->at(1, 3));

  if (delete_btn) {
    std::function<void()> func = [=]() {
      a_session->unbind_window(window);
      window->close();
    };

    delete_btn->on_click(func);
  }

  cherry_kit::icon_button *save_btn =
      dynamic_cast<cherry_kit::icon_button *>(view->at(1, 2));

  if (save_btn) {
    std::function<void()> func = [=]() {
      cherry_kit::text_editor *editor =
          dynamic_cast<cherry_kit::text_editor *>(view->at(0, 0));
      if (editor) {
        a_session->save_session_attribute(session_store_name("reminders"),
                                          "Reminders", "reminders_id",
                                          a_session->session_id_to_string(),
                                          "text", editor->text().toStdString());
      }
    };

    save_btn->on_click(func);
  }

  cherry_kit::icon_button *done_btn =
      dynamic_cast<cherry_kit::icon_button *>(view->at(1, 1));

  if (done_btn) {
    std::function<void()> func = [=]() {
      cherry_kit::text_editor *editor =
          dynamic_cast<cherry_kit::text_editor *>(view->at(0, 0));
      bool is_complete = 0;

      if (a_session->session_keys().contains("state")) {
        qDebug() << Q_FUNC_INFO << "Current state : "
                 << a_session->session_data("state").toString();
      }

      if (a_session->session_keys().contains("state") &&
          (a_session->session_data("state").toString() == "done")) {
        is_complete = 1;
      }

      if (editor) {
        if (is_complete)
          editor->style("border: 0; background: #ffffff; color: #000000");
        else
          editor->style("border: 0; background: #29CDA8; color: #ffffff");
      }

      cherry_kit::widget_properties_t update_prop;
      if (is_complete) {
        update_prop["label"] = "";
        update_prop["icon"] = "toolbar/ck_checkmark2.png";
      } else {
        update_prop["label"] = "";
        update_prop["icon"] = "toolbar/ck_checkmark.png";
      }

      view->update_property(1, 1, update_prop);

      a_session->save_session_attribute(
          session_store_name("reminders"), "Reminders", "reminders_id",
          a_session->session_id_to_string(), "state",
          (is_complete == 1) ? "wip" : "done");
    };

    done_btn->on_click(func);
  }

  cherry_kit::icon_button *set_btn =
      dynamic_cast<cherry_kit::icon_button *>(view->at(1, 0));

  if (set_btn) {
    std::function<void()> func = [=]() {
      if (!viewport())
        return;

      QRectF parent_rect(window->x(), window->y(), window->geometry().width(),
                         window->geometry().height());

      QRectF dialog_rect(0, 0, 320, 320);

      QPointF dialog_pos = viewport()->center(
          dialog_rect, parent_rect, cherry_kit::space::kCenterOnWindow);

      cherry_kit::desktop_dialog_ref ck_date_dialog =
          viewport()->open_desktop_dialog("date_dialog", "Date", dialog_pos,
                                          dialog_rect, QVariantMap());
      // todo handle dialog close.
    };

    set_btn->on_click(func);
  }

  window->on_window_discarded([this](cherry_kit::window *aWindow) {
    delete aWindow;
  });

  if (viewport()) {
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}
