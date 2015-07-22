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
#include "dock.h"
#include "snapframe.h"

#include <ck_ToolBar.h>
#include <ck_icon_button.h>
#include <ck_TableView.h>

#include <ck_workspace.h>

// models
#include <ck_DefaultTableModel.h>
#include <ck_DefaultTableComponent.h>
#include <ck_item_view.h>

// core
#include <ck_config.h>
#include <ck_extension_manager.h>
#include <ck_image_view.h>
#include <ck_fixed_layout.h>

using namespace cherry_kit;

class desktop_panel_controller_impl::PrivateDock {
public:
  PrivateDock() {}
  ~PrivateDock() { qDebug() << Q_FUNC_INFO; }

public:
  window *m_panel_window;
  window *m_preview_window;

  cherry_kit::item_view *m_preview_widget;

  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;

  cherry_kit::desktop_dialog_ref m_desktop_menu;
  cherry_kit::icon_button *m_add_new_workspace_button_ptr;
  cherry_kit::ActionList m_supported_action_list;
};

desktop_panel_controller_impl::desktop_panel_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new PrivateDock) {
  priv->m_actions_map["ToggleDock"] = 1;
  priv->m_actions_map["ShowDock"] = 2;
  priv->m_actions_map["HideDock"] = 3;
  priv->m_actions_map["HideDock"] = 4;
  priv->m_actions_map["ShowMenu"] = 5;

  priv->m_main_panel_is_hidden = true;

  // menu
  priv->m_preview_widget = new cherry_kit::item_view();
  priv->m_preview_widget->on_item_removed([this](
      cherry_kit::model_view_item *a_item) {
    if (a_item) {
      delete a_item;
    }
  });
  priv->m_preview_widget->on_activated([this](int index) {
    if (this->viewport() && this->viewport()->owner_workspace()) {
      cherry_kit::workspace *_workspace =
          qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

      if (_workspace) {
        _workspace->expose(index);
      }
    }
  });
}

desktop_panel_controller_impl::~desktop_panel_controller_impl() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void desktop_panel_controller_impl::create_dock_action(
    cherry_kit::fixed_layout *build, int row, int column,
    const std::string &icon, std::function<void()> a_button_action_func) {
  cherry_kit::widget_properties_t prop;
  cherry_kit::widget *ck_widget;
  prop["label"] = "";
  prop["icon"] = icon;
  ck_widget = build->add_widget(row, column, "image_button", prop);

  ck_widget->on_input_event([=](cherry_kit::widget::InputEvent a_event,
                                const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      if (a_button_action_func)
        a_button_action_func();
    }
  });
}

void desktop_panel_controller_impl::init() {
  priv->m_supported_action_list << createAction(1, tr("Menu"),
                                                   "pd_menu_icon.png");
  priv->m_supported_action_list << createAction(2, tr("show-dock"),
                                                   "pd_menu_icon.png");
  priv->m_supported_action_list << createAction(3, tr("hide-dock"),
                                                   "pd_menu_icon.png");
  priv->m_supported_action_list << createAction(4, tr("show-expose"),
                                                   "pd_menu_icon.png");
  priv->m_supported_action_list << createAction(5, tr("hide-expose"),
                                                   "pd_menu_icon.png");

  if (!viewport()) {
    return;
  }

  cherry_kit::space *_space = viewport();
  if (_space) {
    priv->m_desktop_menu = createActivity("", "icon_dialog", "PlexyDesk 1.0",
                                             QPoint(), QVariantMap());

    if (priv->m_desktop_menu && priv->m_desktop_menu->dialog_window()) {
      priv->m_desktop_menu->dialog_window()->set_window_type(
          window::kPopupWindow);
      priv->m_desktop_menu->dialog_window()->setVisible(false);
    }
  }

  // loads the controllers before dock was created;
  Q_FOREACH(const QString & name, viewport()->current_controller_list()) {
    discover_actions_from_controller(name);
  }

  viewport()->on_viewport_event_notify([this](
      space::ViewportNotificationType aType, const QVariant &aData,
      const space *aSpace) {

    if (aType == space::kControllerAddedNotification) {
      discover_actions_from_controller(aData.toString());
    }
  });

  priv->m_panel_window = new cherry_kit::window();
  priv->m_panel_window->set_window_type(window::kPanelWindow);

  priv->m_preview_window = new cherry_kit::window();

  priv->m_preview_window->set_window_type(window::kPopupWindow);
  priv->m_preview_window->enable_window_background(false);

  // navigation
  cherry_kit::fixed_layout *build =
      new cherry_kit::fixed_layout(priv->m_panel_window);
  build->set_content_margin(6, 10, 10, 10);
  build->set_geometry(0, 0, 32 + 16, 32 * 7);
  build->add_rows(7);

  std::string default_height = std::to_string((32.0 / (32.0 * 7)) * 100) + "%";

  for (int i = 0; i < 7; i++) {
    build->add_segments(i, 1);
    build->set_row_height(i, default_height);
  }

  cherry_kit::widget_properties_t accept_button_prop;

  create_dock_action(build, 0, 0, "pd_up_icon.png",
                     [&]() { exec_action("Up"); });

  create_dock_action(build, 1, 0, "pd_space_icon.png",
                     [&]() { exec_action("Expose"); });

  create_dock_action(build, 2, 0, "pd_note_add_button_green.png",
                     [&]() { exec_action("Add"); });

  create_dock_action(build, 3, 0, "pd_menu_icon.png",
                     [&]() { exec_action("Menu"); });

  create_dock_action(build, 4, 0, "pd_seamless_icon.png",
                     [&]() { exec_action("Seamless"); });

  create_dock_action(build, 5, 0, "pd_delete_icon.png",
                     [&]() { exec_action("Close"); });

  create_dock_action(build, 6, 0, "pd_down_icon.png",
                     [&]() { exec_action("Down"); });

  // base->setGeometry(build->ui()->geometry());
  priv->m_panel_window->set_window_content(build->viewport());
  priv->m_preview_window->set_window_content(priv->m_preview_widget);

  insert(priv->m_panel_window);
  insert(priv->m_preview_window);

  priv->m_preview_window->hide();
}

void desktop_panel_controller_impl::session_data_available(
    const cherry_kit::sync_object &a_sesion_root) {}

void desktop_panel_controller_impl::submit_session_data(
    cherry_kit::sync_object *a_obj) {}

void desktop_panel_controller_impl::set_view_rect(const QRectF &rect) {
  if (!viewport()) {
    return;
  }

  priv->m_panel_window->setPos(
      viewport()->center(priv->m_panel_window->geometry(), QRectF(),
                         space::kCenterOnViewportLeft));

  priv->m_preview_widget->set_view_geometry(
      QRectF(0.0, 0.0, 256, rect.height() - 24.0));

  priv->m_preview_window->setGeometry(QRectF(0.0, 0.0, 256, rect.height()));

  priv->m_preview_window->setPos(
      rect.x() + priv->m_panel_window->geometry().width() + 5,
      rect.y() + 24.0);

  priv->m_preview_window->hide();

  if (priv->m_desktop_menu) {
    priv->m_desktop_menu->dialog_window()->setPos(rect.x(), rect.y());
  }
}

ActionList desktop_panel_controller_impl::actions() const {
  return priv->m_supported_action_list;
}

void desktop_panel_controller_impl::request_action(const QString &actionName,
                                                   const QVariantMap &args) {
  if (actionName.toLower() == "menu") {

    if (priv->m_desktop_menu && priv->m_desktop_menu->dialog_window()) {
      priv->m_desktop_menu->dialog_window()->setPos(
          args["menu_pos"].toPoint());
      priv->m_desktop_menu->dialog_window()->show();
    }

    return;
  } else if (actionName.toLower() == "show-dock") {
    priv->m_panel_window->show();
    return;
  } else if (actionName.toLower() == "hide-dock") {
    priv->m_panel_window->hide();
    return;
  } else if (actionName.toLower() == "show-expose") {
    if (priv->m_preview_window->isVisible()) {
      priv->m_preview_window->hide();
    } else {
      udpate_desktop_preview();
      priv->m_preview_window->show();
    }
  } else if (actionName.toLower() == "hide-expose") {
    priv->m_preview_window->hide();
  }

  if (viewport() && viewport()->controller(args["controller"].toString())) {
    qDebug() << Q_FUNC_INFO << actionName;
    qDebug() << Q_FUNC_INFO << args;

    viewport()->controller(args["controller"].toString())->request_action(
        actionName, args);
  } else {
    qWarning() << Q_FUNC_INFO << "Unknown Action";
  }
}

QString desktop_panel_controller_impl::icon() const {
  return QString("pd_desktop_icon.png");
}

desktop_dialog_ref desktop_panel_controller_impl::createActivity(
    const QString &controllerName, const QString &activity,
    const QString &title, const QPoint &pos, const QVariantMap &dataItem) {
  QPoint _activity_location = pos;

  cherry_kit::desktop_dialog_ref _intent = viewport()->open_desktop_dialog(
      activity, title, _activity_location,
      QRectF(0, _activity_location.y(), 320, 320), dataItem);
  _intent->set_controller(cherry_kit::desktop_controller_ref(this));
  _intent->set_activity_attribute("data", QVariant(dataItem));
  _intent->set_activity_attribute("auto_scale", QVariant(1));

  return _intent;
}

void desktop_panel_controller_impl::switch_to_next_space() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    if (_workspace) {
      toggle_panel();
      _workspace->expose_next();
    }
  }
}

void desktop_panel_controller_impl::toggle_seamless() {
  if (!viewport()) {
    return;
  }

  cherry_kit::desktop_controller_ref controller =
      viewport()->controller("classicbackdrop");

  if (!controller) {
    qWarning() << Q_FUNC_INFO << "Controller Not Found";
    return;
  }

  controller->request_action("Seamless");
}

void desktop_panel_controller_impl::prepare_removal() {
  if (viewport() && viewport()->owner_workspace()) {
    QGraphicsView *_workspace =
        qobject_cast<QGraphicsView *>(viewport()->owner_workspace());

    if (_workspace) {
      priv->m_desktop_menu.clear();
    } else {
      if (priv->m_desktop_menu) {
        priv->m_desktop_menu.clear();
      }
    }
  } else {
    qWarning() << Q_FUNC_INFO << "Error : Missing workspace or viewport";
  }
}

void desktop_panel_controller_impl::switch_to_previous_space() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    if (_workspace) {
      toggle_panel();
      _workspace->expose_previous();
    }
  }
}

void desktop_panel_controller_impl::toggle_panel() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    if (_workspace) {
      QRectF _work_area = viewport()->geometry();

      if (!priv->m_main_panel_is_hidden) {
        _work_area.setX(331.0f);
        _work_area.setWidth(_work_area.width() + 330.0f);
      }

      priv->m_main_panel_is_hidden = !priv->m_main_panel_is_hidden;

      _workspace->expose_sub_region(_work_area);
    }
  }
}

void desktop_panel_controller_impl::discover_actions_from_controller(const QString &name) {
  if (priv->m_controller_name_list.contains(name) || !viewport()) {
    return;
  }

  cherry_kit::desktop_controller_ref controller = viewport()->controller(name);

  if (!controller) {
    return;
  }

  if (controller->actions().count() <= 0) {
    return;
  }

  QVariantMap _data;

  Q_FOREACH(QAction * action, controller->actions()) {
    if (!action) {
      continue;
    }

    QVariantMap _item;

    bool _is_hidden = action->property("hidden").toBool();

    if (_is_hidden) {
      continue;
    }

    _item["label"] = action->text();
    _item["icon"] = action->property("icon_name");
    _item["controller"] = QVariant(name);
    _item["id"] = action->property("id");

    _data[QString("%1.%2").arg(name).arg(action->text())] = _item;
  }

  if (_data.keys().count() <= 0) {
    return;
  }

  priv->m_desktop_menu->update_attribute("data", _data);
}

void desktop_panel_controller_impl::remove_space_request() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());
    if (_workspace) {
      _workspace->remove(viewport());
    }
  }
}

void desktop_panel_controller_impl::exec_action(const QString &action) {
  if (action == tr("Close")) {
    remove_space_request();
    return;
  } else if (action == tr("Up")) {
    this->switch_to_previous_space();
  } else if (action == tr("Down")) {
    this->switch_to_next_space();
  } else if (action == tr("Seamless")) {
    this->toggle_seamless();
  } else if (action == tr("Expose")) {
    if (priv->m_preview_window->isVisible()) {
      priv->m_preview_window->hide();
      if (priv->m_preview_widget)
        priv->m_preview_widget->clear();
    } else {
      udpate_desktop_preview();
      priv->m_preview_window->show();
    }
    return;
  } else if (action == tr("Menu")) {
    if (!viewport() || !viewport()->owner_workspace()) {
      return;
    }

    QPointF _menu_pos = viewport()->center(
        priv->m_desktop_menu->dialog_window()->boundingRect(), QRectF(),
        cherry_kit::space::kCenterOnViewportLeft);
    _menu_pos.setX(priv->m_panel_window->geometry().width() + 5);

    if (priv->m_desktop_menu && priv->m_desktop_menu->dialog_window()) {
      priv->m_desktop_menu->dialog_window()->setPos(_menu_pos);
      priv->m_desktop_menu->dialog_window()->show();
    }

  } else if (action == tr("Add")) {
    add_new_space();
  }
}

void desktop_panel_controller_impl::add_new_space() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    if (_workspace) {
      _workspace->add_default_space();
    }
  }
}

QAction *desktop_panel_controller_impl::createAction(int id,
                                                     const QString &action_name,
                                                     const QString &icon_name) {
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 1);

  return _add_clock_action;
}

void desktop_panel_controller_impl::udpate_desktop_preview() {
  priv->m_preview_widget->clear();

  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    float lHeight = 10;
    float lWidth = 0;

    if (_workspace) {
      foreach(cherry_kit::space * _space, _workspace->current_spaces()) {
        QPixmap _preview = _workspace->thumbnail(_space);

        cherry_kit::image_view *p = new cherry_kit::image_view();

        p->setMinimumSize(_preview.size());
        p->set_pixmap(_preview);
        lHeight += _preview.size().height();
        lWidth = _preview.size().width();

        cherry_kit::model_view_item *model_item =
            new cherry_kit::model_view_item();
        model_item->set_view(p);
        model_item->on_view_removed([=](cherry_kit::model_view_item *a_item) {
          if (a_item && a_item->view()) {
            cherry_kit::widget *view = a_item->view();
            if (view)
              delete view;
          }
        });

        priv->m_preview_widget->insert(model_item);
      }
    }

    QPointF lMenuPos = viewport()->center(
        priv->m_desktop_menu->dialog_window()->boundingRect(), QRectF(),
        cherry_kit::space::kCenterOnViewportLeft);

    lMenuPos.setX(priv->m_panel_window->geometry().width() + 5.0);
    priv->m_preview_window->setGeometry(
        QRectF(lMenuPos.x(), lMenuPos.y(), lWidth, lHeight + 24));
  }
}
