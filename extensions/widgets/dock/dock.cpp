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

#include <ck_icon_button.h>
#include <ck_workspace.h>

// models
#include <ck_item_view.h>

// core
#include <ck_config.h>
#include <ck_extension_manager.h>
#include <ck_image_view.h>
#include <ck_fixed_layout.h>
#include <ck_label.h>
#include <ck_resource_manager.h>
#include <ck_item_view.h>

using namespace cherry_kit;

class desktop_panel_controller_impl::PrivateDock {
public:
  PrivateDock() {}
  ~PrivateDock() {}

public:
  window *m_panel_window;
  window *m_task_window;

  cherry_kit::item_view *m_task_grid;
  cherry_kit::fixed_layout *m_fixed_panel_layout;

  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;

  cherry_kit::icon_button *m_add_new_workspace_button_ptr;
  cherry_kit::ui_action_list m_task_list;
};

desktop_panel_controller_impl::desktop_panel_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object), priv(new PrivateDock) {
  priv->m_main_panel_is_hidden = true;

  // menu
  priv->m_task_window = new cherry_kit::window();
  priv->m_task_window->set_window_type(cherry_kit::window::kPopupWindow);
  priv->m_task_window->hide();
  priv->m_task_window->resize(400, 10);

  priv->m_task_window->on_window_discarded([=](cherry_kit::window *aWindow) {
    if (priv->m_task_grid)
      priv->m_task_grid->clear();

    delete aWindow;
  });

  priv->m_task_grid = new item_view(priv->m_task_window, item_view::kGridModel);
  priv->m_task_grid->set_content_spacing(0);

  priv->m_task_grid->on_item_removed([](cherry_kit::model_view_item *a_item) {
    if (a_item)
      delete a_item;
  });
}

desktop_panel_controller_impl::~desktop_panel_controller_impl() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void desktop_panel_controller_impl::create_dock_action(
    cherry_kit::fixed_layout *m_fixed_panel_layout, int row, int column,
    const std::string &icon, std::function<void()> a_button_action_func) {
  cherry_kit::widget_properties_t prop;
  cherry_kit::widget *ck_widget;
  prop["label"] = "";
  prop["icon"] = icon;
  ck_widget =
      m_fixed_panel_layout->add_widget(row, column, "image_button", prop);

  ck_widget->on_input_event([=](cherry_kit::widget::InputEvent a_event,
                                const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      if (a_button_action_func)
        a_button_action_func();
    }
  });
}

widget *desktop_panel_controller_impl::create_task_action(
    cherry_kit::ui_action &a_task) {

  QSizeF item_icon_size(viewport()->scaled_width(39),
                        viewport()->scaled_height(39));
  QSizeF item_label_size(viewport()->scaled_width(64),
                         viewport()->scaled_height(16));

  widget *l_rv = new widget();

  cherry_kit::image_view *l_image_view = new cherry_kit::image_view(l_rv);
  cherry_kit::label *l_action_label = new cherry_kit::label(l_rv);
  l_action_label->set_text(a_task.name().c_str());
  l_action_label->set_widget_name(a_task.name().c_str());

  QPixmap l_view_pixmap(cherry_kit::resource_manager::instance()->drawable(
      a_task.icon().c_str(), "mdpi"));
  l_image_view->set_pixmap(l_view_pixmap);
  l_image_view->setMinimumSize(item_icon_size);
  l_image_view->set_size(item_icon_size);

  l_action_label->set_size(item_label_size);
  l_action_label->setPos(0, l_image_view->geometry().height());
  l_image_view->setPos(16, 0);

  l_rv->setGeometry(l_image_view->geometry());

  QSizeF l_action_item_size;
  l_action_item_size.setHeight(l_image_view->boundingRect().height() +
                               l_action_label->boundingRect().height());
  l_action_item_size.setWidth(l_action_label->boundingRect().width());

  l_rv->setMinimumSize(l_action_item_size);

  a_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                         const cherry_kit::ui_task_data_t &a_data) {
    ui_action_list child_actions = a_action_ref->sub_actions();

    if (child_actions.size() > 0) {
      qDebug() << Q_FUNC_INFO << "insert Sub Tasks";
      ui_action copy(*a_action_ref);
      insert_sub_action(copy);
    }
  });

  l_image_view->on_input_event([=](cherry_kit::widget::InputEvent aEvent,
                                   const widget *aWidget) {
    if (aEvent == cherry_kit::widget::kMouseReleaseEvent) {
      a_task.execute();
    }
  });

  return l_rv;
}

void desktop_panel_controller_impl::discover_actions_from_controller(
    const QString &name) {

  if (priv->m_controller_name_list.contains(name) || !viewport()) {
    return;
  }

  cherry_kit::desktop_controller_ref controller = viewport()->controller(name);

  if (!controller) {
    return;
  }

  ui_action root_action = controller->task();
  qDebug() << Q_FUNC_INFO << "Loading from :" << name
           << "Root Task : " << root_action.name().c_str();
  insert_action(root_action);
}

void desktop_panel_controller_impl::insert_action(ui_action &a_task) {
  if (!a_task.is_visibile())
    return;

  cherry_kit::model_view_item *grid_item = new cherry_kit::model_view_item();
  grid_item->set_view(create_task_action(a_task));

  grid_item->on_view_removed([](cherry_kit::model_view_item *a_item) {
    if (a_item && a_item->view()) {
      cherry_kit::widget *view = a_item->view();
      if (view)
        delete view;
    }
  });

  if (!priv->m_task_grid)
    return;

  float row_count = (priv->m_task_grid->count() + 1) % 4;

  if (row_count == 0)
    row_count = (priv->m_task_grid->count() + 1) / 4;

  if (row_count != 0)
    row_count = 1 + ((priv->m_task_grid->count() + 1) / 4);
  else if (row_count == 0) {
    row_count = (priv->m_task_grid->count() + 1) / 4;
  }

  qDebug() << Q_FUNC_INFO << row_count;
  float window_width = (viewport()->scaled_width(64) * 4);
  float window_height = (viewport()->scaled_width(36) * row_count) +
                        (viewport()->scaled_height(24) * row_count);
  priv->m_task_grid->set_view_geometry(
      QRectF(0, 0, window_width, window_height));

  priv->m_task_grid->insert(grid_item);
  priv->m_task_window->set_window_content(priv->m_task_grid);
}

void desktop_panel_controller_impl::insert_sub_action(ui_action &a_task) {
  ui_action_list child_actions = a_task.sub_actions();

  cherry_kit::window *sub_menu = new cherry_kit::window(0);

  sub_menu->set_window_type(cherry_kit::window::kPopupWindow);
  sub_menu->set_window_title(a_task.name().c_str());
  sub_menu->setGeometry(QRectF(0, 0, 400, 400));

  cherry_kit::item_view *sub_task_grid =
      new cherry_kit::item_view(sub_menu, cherry_kit::item_view::kGridModel);

  int x_count = child_actions.size();

  float row_count = (x_count) % 4;

  if (row_count != 0 && x_count > 4) {
    row_count = 1 + (x_count / 4);
  } else if (row_count == 0) {
    row_count = (x_count) / 4;
  } else if (x_count < 4) {
     row_count = 1;
  }

  int menu_width = 4;
  if (x_count < 4)
      menu_width = x_count;

  float window_width = (viewport()->scaled_width(64) * menu_width);
  float window_height = (viewport()->scaled_width(36) * (row_count)) +
                        (viewport()->scaled_height(24) * (row_count));

  sub_task_grid->set_view_geometry(QRectF(0, 0, window_width, window_height));

  sub_menu->on_visibility_changed([=](window *a_window_ref, bool a_visible) {
    if (!a_visible) {
      a_window_ref->close();
      // delete a_window_ref;
      delete sub_task_grid;
      delete sub_menu;
    }
  });

  if (child_actions.size() > 0) {
    std::for_each(std::begin(child_actions), std::end(child_actions),
                  [&](ui_action &a_action) {
      if (!a_action.is_visibile())
        return;

      cherry_kit::model_view_item *grid_item =
          new cherry_kit::model_view_item();

      grid_item->set_view(create_task_action(a_action));

      grid_item->on_view_removed([](cherry_kit::model_view_item *a_item) {
        if (a_item && a_item->view()) {
          cherry_kit::widget *view = a_item->view();
          if (view)
            delete view;
        }
      });

      sub_task_grid->insert(grid_item);
    });
  }

  sub_menu->set_window_content(sub_task_grid);
  insert(sub_menu);

  // window pos
  QPointF _menu_pos;
  _menu_pos.setX(priv->m_task_window->x());
  _menu_pos.setY(priv->m_task_window->y());

  sub_menu->setPos(_menu_pos);
  sub_menu->raise();
}

ui_action desktop_panel_controller_impl::task() {
  ui_action task;
  task.set_visible(0);
  task.set_name("Desktop");
  task.set_icon("ck_desktop_icon.png");
  task.set_controller(controller_name().toStdString());

  ui_action menu_task;
  menu_task.set_name("menu");
  menu_task.set_id(0);
  menu_task.set_visible(0);
  menu_task.set_icon("ck_menu_icon.png");
  menu_task.set_controller(controller_name().toStdString());
  menu_task.set_task([this](const ui_action *a_action_ref,
                            const ui_task_data_t &a_data) {
    if (priv->m_task_window) {

      QPointF menu_pos;
      if (a_data.find("x") != a_data.end() &&
          a_data.find("y") != a_data.end()) {
        menu_pos.setX(std::stod(a_data.at("x")));
        menu_pos.setY(std::stod(a_data.at("y")));
      }

      priv->m_task_window->setPos(menu_pos);
      priv->m_task_window->show();
    }
  });

  ui_action desktop_task;
  desktop_task.set_name("seamless");
  desktop_task.set_id(1);
  desktop_task.set_visible(0);
  desktop_task.set_icon("ck_seamless_icon.png");
  desktop_task.set_controller(controller_name().toStdString());
  desktop_task.set_task([this](const ui_action *a_action_ref,
                               const ui_task_data_t &a_data) {
    toggle_seamless();
  });

  task.add_action(menu_task);
  task.add_action(desktop_task);

  return task;
}

void desktop_panel_controller_impl::init() {
  if (!viewport()) {
    return;
  }

  if (priv->m_task_window)
    priv->m_task_window->set_window_title("PlexyDesk RedCherry 1.0");

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

  // navigation
  // so that the icon size is 36.0f;
  float icon_size = viewport()->scaled_width(
      39.0f); /// viewport()->owner_workspace()->desktop_horizontal_scale_factor();
  priv->m_fixed_panel_layout =
      new cherry_kit::fixed_layout(priv->m_panel_window);
  priv->m_fixed_panel_layout->set_content_margin(3, 5, 5, 5);
  priv->m_fixed_panel_layout->set_geometry(0, 0, icon_size + 8, icon_size * 7);
  priv->m_fixed_panel_layout->add_rows(7);

  priv->m_panel_window->on_window_discarded([this](
      cherry_kit::window *aWindow) {
    if (priv->m_fixed_panel_layout)
      delete priv->m_fixed_panel_layout;
    delete aWindow;
  });

  std::string default_height =
      std::to_string((icon_size / (icon_size * 7)) * 100) + "%";

  for (int i = 0; i < 7; i++) {
    priv->m_fixed_panel_layout->add_segments(i, 1);
    priv->m_fixed_panel_layout->set_row_height(i, default_height);
  }

  create_dock_action(priv->m_fixed_panel_layout, 0, 0, "panel/ck_up_arrow.png",
                     [&]() { exec_action("Up"); });

  create_dock_action(priv->m_fixed_panel_layout, 1, 0, "panel/ck_add.png",
                     [&]() { exec_action("Add"); });

  create_dock_action(priv->m_fixed_panel_layout, 2, 0, "panel/ck_space.png",
                     [&]() { exec_action("Expose"); });

  create_dock_action(priv->m_fixed_panel_layout, 3, 0, "panel/ck_menu.png",
                     [&]() { exec_action("Menu"); });

  create_dock_action(priv->m_fixed_panel_layout, 4, 0, "panel/ck_expose.png",
                     [&]() { exec_action("Seamless"); });

  create_dock_action(priv->m_fixed_panel_layout, 5, 0, "panel/ck_trash.png",
                     [&]() { exec_action("Close"); });

  create_dock_action(priv->m_fixed_panel_layout, 6, 0,
                     "panel/ck_down_arrow.png", [&]() { exec_action("Down"); });

  priv->m_panel_window->set_window_content(
      priv->m_fixed_panel_layout->viewport());

  insert(priv->m_task_window);
  insert(priv->m_panel_window);
}

void desktop_panel_controller_impl::session_data_ready(
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

  if (priv->m_task_window) {
    priv->m_task_window->setPos(rect.x(), rect.y());
  }
}

desktop_dialog_ref desktop_panel_controller_impl::createActivity(
    const QString &controllerName, const QString &activity,
    const QString &title, const QPoint &pos, const QVariantMap &dataItem) {
  QPoint _activity_location = pos;

  cherry_kit::desktop_dialog_ref _intent = viewport()->open_desktop_dialog(
      activity, title, _activity_location,
      QRectF(0, _activity_location.y(), (64 * 3) - 10, 240), dataItem);
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

void desktop_panel_controller_impl::toggle_seamless() const {
  if (!viewport()) {
    return;
  }

  cherry_kit::desktop_controller_ref controller =
      viewport()->controller("classicbackdrop");

  if (!controller) {
    qWarning() << Q_FUNC_INFO << "Controller Not Found";
    return;
  }

  controller->task().execute("Seamless");
}

void desktop_panel_controller_impl::prepare_removal() {
  // if (priv->m_task_window) {
  //   priv->m_task_window->discard();
  //}
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
      update_desktop_preview();
  } else if (action == tr("Menu")) {
    if (!viewport() || !viewport()->owner_workspace()) {
      return;
    }

    QPointF _menu_pos =
        viewport()->center(priv->m_task_window->geometry(), QRectF(),
                           cherry_kit::space::kCenterOnViewportLeft);
    _menu_pos.setX(priv->m_panel_window->geometry().width() + 5);

    if (priv->m_task_window) {
      priv->m_task_window->setPos(_menu_pos);
      priv->m_task_window->show();
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

void desktop_panel_controller_impl::update_desktop_preview() {
  cherry_kit::window *ck_window = new cherry_kit::window;
  ck_window->set_window_title("Desktop Preview");
  ck_window->setGeometry(QRectF(0, 0, 400, 400));
  ck_window->set_window_type(cherry_kit::window::kPopupWindow);

  cherry_kit::item_view *preview_view =
      new cherry_kit::item_view(ck_window, cherry_kit::item_view::kGridModel);

  cherry_kit::workspace *workspace_ref = viewport()->owner_workspace();
  float item_height = 80;

  float row_count = (workspace_ref->current_spaces().count()) % 4;

  if (row_count != 0)
    row_count = 1 + (workspace_ref->current_spaces().count()) / 4;
  else if (row_count == 0) {
    row_count = (workspace_ref->current_spaces().count()) / 4;
  }
  float window_width = (128 * 4) + 32;
  float window_height = (item_height * (row_count)) + 56;

  preview_view->set_view_geometry(QRectF(0, 0, window_width, window_height));

  ck_window->on_visibility_changed([=](window *a_window_ref, bool a_visible) {
    if (!a_visible) {
      a_window_ref->close();
      // delete a_window_ref;
      delete preview_view;
    }
  });

  foreach(cherry_kit::space * _space, workspace_ref->current_spaces()) {
    QPixmap _preview = workspace_ref->thumbnail(_space);

    cherry_kit::image_view *p = new cherry_kit::image_view(preview_view);

    p->set_pixmap(_preview);
    p->set_size(_preview.size());
    p->setGeometry(QRectF(0, 0, _preview.size().width(), _preview.height()));
    p->setMinimumSize(_preview.size());

    item_height = _preview.height();

    cherry_kit::model_view_item *model_item = new cherry_kit::model_view_item();
    model_item->set_view(p);

    model_item->on_view_removed([=](cherry_kit::model_view_item *a_item) {
      if (a_item && a_item->view()) {
        cherry_kit::widget *view = a_item->view();
        if (view)
          delete view;
      }
    });

    preview_view->insert(model_item);
  }

  preview_view->on_activated([this](int index) {
    if (this->viewport() && this->viewport()->owner_workspace()) {
      cherry_kit::workspace *_workspace =
          qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

      if (_workspace) {
        _workspace->expose(index);
      }
    }
  });

  insert(ck_window);
  ck_window->raise();
  ck_window->set_window_content(preview_view);

  QPointF menu_pos =
      viewport()->center(ck_window->geometry(), QRectF(),
                         cherry_kit::space::kCenterOnViewportLeft);

  menu_pos.setX(priv->m_panel_window->geometry().width() + 5);
  ck_window->setPos(menu_pos);
}
