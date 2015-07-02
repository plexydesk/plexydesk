/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <widget.h>
#include <QAction>
#include <session_sync.h>

#include <viewbuilder.h>

#include <button.h>
#include <imagebutton.h>
#include <texteditor.h>

#include "notewidget.h"

class DesktopNotesControllerImpl::PrivateDesktopNotes {
public:
  PrivateDesktopNotes() {}
  ~PrivateDesktopNotes() {}

  QMap<QString, int> mNoteActions;
  CherryKit::ActionList m_supported_action_list;
};

DesktopNotesControllerImpl::DesktopNotesControllerImpl(QObject *object)
    : CherryKit::ViewController(object),
      o_view_controller(new PrivateDesktopNotes) {
  o_view_controller->mNoteActions["Note"] = 1;
  o_view_controller->mNoteActions["Task"] = 2;
  o_view_controller->mNoteActions["Reminder"] = 3;
}

DesktopNotesControllerImpl::~DesktopNotesControllerImpl() {
  delete o_view_controller;
}

void DesktopNotesControllerImpl::init() {
  QAction *_add_note_action = new QAction(this);
  _add_note_action->setText(tr("Note"));

  _add_note_action->setProperty("id", QVariant(1));
  _add_note_action->setProperty("icon_name", "pd_add_note_frame_icon.png");

  QAction *_add_task_action = new QAction(this);
  _add_task_action->setText(tr("Task"));

  _add_task_action->setProperty("id", QVariant(2));
  _add_task_action->setProperty("icon_name", "pd_add_note_frame_icon.png");

  QAction *_add_reminder_action = new QAction(this);
  _add_reminder_action->setText(tr("Reminder"));

  _add_reminder_action->setProperty("id", QVariant(2));
  _add_reminder_action->setProperty("icon_name", "pd_add_note_frame_icon.png");

  o_view_controller->m_supported_action_list << _add_note_action;
  o_view_controller->m_supported_action_list << _add_task_action;
  o_view_controller->m_supported_action_list << _add_reminder_action;
}

void DesktopNotesControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_sesion_root) {
  revoke_previous_session(
      "Notes",
      [this](CherryKit::ViewController *a_controller,
             CherryKit::SessionSync *a_session) { createNoteUI(a_session); });

  revoke_previous_session("Reminders",
                          [this](CherryKit::ViewController *a_controller,
                                 CherryKit::SessionSync *a_session) {
    createReminderUI(a_session);
  });
}

void
DesktopNotesControllerImpl::submit_session_data(QuetzalKit::SyncObject *a_obj) {
  write_session_data("Notes");
  write_session_data("Reminders");
}

void DesktopNotesControllerImpl::set_view_rect(const QRectF &rect) {}

CherryKit::ActionList DesktopNotesControllerImpl::actions() const {
  return o_view_controller->m_supported_action_list;
}

void DesktopNotesControllerImpl::request_action(const QString &actionName,
                                                const QVariantMap &args) {
  QPointF window_location;

  if (viewport()) {
    window_location = viewport()->center(QRectF(0, 0, 240, 240 + 48));
  }

  QVariantMap session_args;

  switch (o_view_controller->mNoteActions[actionName]) {
  case 1:
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["notes_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("Notes"));

    start_session(
        "Notes", session_args, false,
        [this](CherryKit::ViewController *a_controller,
               CherryKit::SessionSync *a_session) { createNoteUI(a_session); });
    break;
  case 2:
    break;
  case 3:
    session_args["x"] = window_location.x();
    session_args["y"] = window_location.y();
    session_args["reminders_id"] = session_count();
    session_args["database_name"] =
        QString::fromStdString(session_database_name("reminders"));

    start_session("Reminders", session_args, false,
                  [this](CherryKit::ViewController *a_controller,
                         CherryKit::SessionSync *a_session) {
      createReminderUI(a_session);
    });
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Action";
  }
}

void DesktopNotesControllerImpl::handle_drop_event(CherryKit::Widget *widget,
                                                   QDropEvent *event) {
  const QString droppedFile = event->mimeData()->urls().value(0).toLocalFile();
  QFileInfo fileInfo(droppedFile);

  if (fileInfo.isFile()) {
    QPixmap image(droppedFile);
    NoteWidget *note = qobject_cast<NoteWidget *>(widget);
    if (note) {
      note->setPixmap(image);
    }
  }
}

QString DesktopNotesControllerImpl::icon() const {
  return QString("pd_add_note_frame_icon.png");
}

void DesktopNotesControllerImpl::onDataUpdated(const QVariantMap &data) {}

void
DesktopNotesControllerImpl::createNoteUI(CherryKit::SessionSync *a_session) {
  CherryKit::Window *window = new CherryKit::Window();
  window->setGeometry(QRectF(0, 0, 320, 240));

  NoteWidget *note = new NoteWidget(a_session, window);
  note->set_controller(this);
  note->setViewport(viewport());

  window->setGeometry(note->geometry());
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
        session_database_name("notes"), "Notes", "notes_id",
        a_session->session_id_to_string(), "text", a_text.toStdString());
  });

  note->on_geometry_changed([=](const QRectF &a_geometry) {
    if (window) {
      window->resize(a_geometry.width(), a_geometry.height());
    }
  });

  note->on_note_config_changed([=](const QString &a_key,
                                   const QString &a_value) {
    a_session->save_session_attribute(
        session_database_name("notes"), "Notes", "notes_id",
        a_session->session_id_to_string(), a_key.toStdString(),
        a_value.toStdString());
  });

  window->on_window_discarded([this](CherryKit::Window *aWindow) {
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

void DesktopNotesControllerImpl::createReminderUI(
    CherryKit::SessionSync *a_session) {
  CherryKit::Window *window = new CherryKit::Window();
  CherryKit::HybridLayout *view = new CherryKit::HybridLayout(window);

  view->set_content_margin(10, 10, 10, 10);
  view->set_geometry(0, 0, 320, 200);

  view->set_horizontal_segment_count(2);
  view->add_horizontal_segments(0, 1);
  view->add_horizontal_segments(1, 4);

  view->set_horizontal_height(0, "85%");
  view->set_horizontal_height(1, "15%");

  CherryKit::WidgetProperties top_label_prop;
  top_label_prop["label"] = "reminder";

  CherryKit::WidgetProperties text_editor_prop;
  text_editor_prop["text"] = "";

  if (a_session->session_keys().contains("text")) {
    text_editor_prop["text"] =
        std::string(a_session->session_data("text").toByteArray());
  }

  CherryKit::WidgetProperties accept_button_prop;

  view->add_widget(0, 0, "text_edit", text_editor_prop);

  accept_button_prop["label"] = "Date";
  accept_button_prop["icon"] = "actions/pd_notification.png";
  view->add_widget(1, 0, "image_button", accept_button_prop);

  if (a_session->session_keys().contains("state") &&
      (a_session->session_data("state").toString() == "done")) {
    accept_button_prop["label"] = "Resume";
    accept_button_prop["icon"] = "actions/pd_resume.png";

    CherryKit::TextEditor *editor =
        dynamic_cast<CherryKit::TextEditor *>(view->at(0, 0));
    if (editor) {
      editor->style("border: 0; background: #29CDA8; color: #ffffff");
    }
  } else {
    accept_button_prop["label"] = "Done";
    accept_button_prop["icon"] = "actions/pd_done.png";
  }

  view->add_widget(1, 1, "image_button", accept_button_prop);

  accept_button_prop["label"] = "save";
  accept_button_prop["icon"] = "actions/pd_save.png";
  view->add_widget(1, 2, "image_button", accept_button_prop);

  accept_button_prop["label"] = "delete";
  accept_button_prop["icon"] = "actions/pd_delete.png";
  view->add_widget(1, 3, "image_button", accept_button_prop);

  window->set_window_content(view->viewport());
  window->set_window_title("Reminder");

  a_session->bind_to_window(window);
  insert(window);

  CherryKit::ImageButton *delete_btn =
      dynamic_cast<CherryKit::ImageButton *>(view->at(1, 3));

  if (delete_btn) {
    CherryKit::Widget::InputCallback func = [=](
        CherryKit::Widget::InputEvent a_event,
        const CherryKit::Widget *a_widget) {
      if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
        a_session->unbind_window(window);
        window->close();
      }
    };

    delete_btn->on_input_event(func);
  }

  CherryKit::ImageButton *save_btn =
      dynamic_cast<CherryKit::ImageButton *>(view->at(1, 2));

  if (save_btn) {
    CherryKit::Widget::InputCallback func = [=](
        CherryKit::Widget::InputEvent a_event,
        const CherryKit::Widget *a_widget) {
      if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
        CherryKit::TextEditor *editor =
            dynamic_cast<CherryKit::TextEditor *>(view->at(0, 0));
        if (editor) {
          a_session->save_session_attribute(
              session_database_name("reminders"), "Reminders", "reminders_id",
              a_session->session_id_to_string(), "text",
              editor->text().toStdString());
        }
      }
    };

    save_btn->on_input_event(func);
  }

  CherryKit::ImageButton *done_btn =
      dynamic_cast<CherryKit::ImageButton *>(view->at(1, 1));

  if (done_btn) {
    CherryKit::Widget::InputCallback func = [=](
        CherryKit::Widget::InputEvent a_event,
        const CherryKit::Widget *a_widget) {
      if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
        CherryKit::TextEditor *editor =
            dynamic_cast<CherryKit::TextEditor *>(view->at(0, 0));
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

        CherryKit::WidgetProperties update_prop;
        if (is_complete) {
          update_prop["label"] = "Done";
          update_prop["icon"] = "actions/pd_done.png";
        } else {
          update_prop["label"] = "Resume";
          update_prop["icon"] = "actions/pd_resume.png";
        }

        view->update_property(1, 1, update_prop);

        a_session->save_session_attribute(
            session_database_name("reminders"), "Reminders", "reminders_id",
            a_session->session_id_to_string(), "state",
            (is_complete == 1) ? "wip" : "done");
      }
    };

    done_btn->on_input_event(func);
  }

  CherryKit::ImageButton *set_btn =
      dynamic_cast<CherryKit::ImageButton *>(view->at(1, 0));

  if (set_btn) {
    CherryKit::Widget::InputCallback func = [=](
        CherryKit::Widget::InputEvent a_event,
        const CherryKit::Widget *a_widget) {
      if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
        // todo : invoke the calendar.
        if (!viewport())
          return;

        QPointF _activity_window_location = viewport()->center(
            QRectF(0, 0, 240, 320),
            QRectF(window->x(), window->y(), window->geometry().width(),
                   window->geometry().height()),
            CherryKit::Space::kCenterOnWindow);

        CherryKit::DesktopActivityPtr activity = viewport()->create_activity(
            "date_dialog", "Date", _activity_window_location,
            QRectF(0, 0, 340, 320.0), QVariantMap());
      }
    };

    set_btn->on_input_event(func);
  }

  window->on_window_discarded([this](CherryKit::Window *aWindow) {
    delete aWindow;
  });

  if (viewport()) {
    QPointF window_location;
    window_location.setX(a_session->session_data("x").toFloat());
    window_location.setY(a_session->session_data("y").toFloat());
    window->setPos(window_location);
  }
}
