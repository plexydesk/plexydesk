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
#include "desktop_button.h"
#include "snapframe.h"
#include "vertical_dock.h"

#include <ck_icon_button.h>
#include <ck_workspace.h>

// models
#include <ck_item_view.h>

// core
#include <ck_config.h>
#include <ck_extension_manager.h>
#include <ck_fixed_layout.h>
#include <ck_image_view.h>
#include <ck_icon_button.h>
#include <ck_item_view.h>
#include <ck_label.h>
#include <ck_resource_manager.h>
#include <ck_screen.h>

using namespace cherry_kit;

class desktop_panel_controller_impl::PrivateDock {
public:
  PrivateDock() : m_main_menu_toolbar(0), m_sub_menu_toolbar(0), m_expose_window(0){}
  ~PrivateDock() {}

public:
  window *m_deskt_menu;
  window *m_expose_window;

  cherry_kit::item_view *m_task_grid;
  cherry_kit::fixed_layout *m_fixed_panel_layout;

  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;

  desktop_button *m_menu_btn;
  desktop_button *m_up_btn;
  desktop_button *m_down_btn;

  cherry_kit::fixed_layout *m_main_menu_toolbar;
  cherry_kit::fixed_layout *m_sub_menu_toolbar;
};

desktop_panel_controller_impl::desktop_panel_controller_impl(QObject *object)
    : cherry_kit::desktop_controller_interface(object), priv(new PrivateDock) {
  priv->m_main_panel_is_hidden = true;

  // menu
  priv->m_deskt_menu = new cherry_kit::window();
  priv->m_deskt_menu->set_window_type(cherry_kit::window::kPopupWindow);
  priv->m_deskt_menu->hide();
  priv->m_deskt_menu->resize(400, 10);
  priv->m_deskt_menu->set_window_opacity(1.0);

  priv->m_deskt_menu->on_window_discarded([=](cherry_kit::window *aWindow) {
    if (priv->m_task_grid)
      priv->m_task_grid->clear();

    delete aWindow;
  });

  priv->m_task_grid = new item_view(priv->m_deskt_menu, item_view::kGridModel);
  priv->m_task_grid->set_content_margin(0, 0, 0, 0);
  priv->m_task_grid->set_content_spacing(0);

  priv->m_task_grid->on_item_removed([](cherry_kit::model_view_item *a_item) {
    if (a_item)
      delete a_item;
  });
}

desktop_panel_controller_impl::~desktop_panel_controller_impl() {
  delete priv;
}

widget *desktop_panel_controller_impl::create_task_action(
    cherry_kit::ui_action &a_task) {

  cherry_kit::widget *rv = new cherry_kit::widget();
  cherry_kit::icon_button *btn = new cherry_kit::icon_button(rv);

  QPixmap icon_pixmap(cherry_kit::resource_manager::instance()->drawable(
      a_task.icon().c_str(), "mdpi"));

  int icon_size = viewport()->scaled_width(96);
  btn->set_pixmap(icon_pixmap);
  btn->set_text(a_task.name().c_str());
  btn->set_size(QSize(icon_size, icon_size));
  btn->set_geometry(QRectF(0, 0, icon_size, icon_size));
  rv->set_geometry(QRectF(0, 0, icon_size, icon_size));

  a_task.set_task([this](const cherry_kit::ui_action *a_action_ref,
                         const cherry_kit::ui_task_data_t &a_data) {
    ui_action_list child_actions = a_action_ref->sub_actions();

    if (child_actions.size() > 0) {
      ui_action copy(*a_action_ref);
      insert_sub_action(copy);
    }
  });

  btn->on_click([=]() { a_task.execute(); });

  return rv;
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

#ifdef __DEBUG_BUILD__
  qDebug() << Q_FUNC_INFO << "Loading from :" << name
           << "Root Task : " << root_action.name().c_str();
#endif

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

  int row_count = (priv->m_task_grid->count()) / 4;

  float window_width = (96 * 4);
  float window_height = (96 * row_count);

  priv->m_task_grid->set_view_geometry(QRectF(
      0, 0, window_width, window_height + 52));

  priv->m_task_grid->insert(grid_item);

  /* navigation toolbar */
  cherry_kit::fixed_layout *ck_layout =
      new cherry_kit::fixed_layout(priv->m_task_grid);
  priv->m_main_menu_toolbar = ck_layout;

  ck_layout->set_content_margin(4, 4, 4, 4);
  ck_layout->set_geometry(0, 0, priv->m_task_grid->geometry().width(), 32);

  ck_layout->add_rows(1);
  ck_layout->add_segments(0, 5);

  ck_layout->set_row_height(0, "100%");

  cherry_kit::widget_properties_t ui_data;

  ui_data["icon"] = "toolbar/ck_arrow-left.png";
  ck_layout->add_widget(0, 0, "image_button", ui_data, [=]() {
     switch_to_previous_space();
  });

  ui_data["icon"] = "toolbar/ck_eye.png";
  ck_layout->add_widget(0, 1, "image_button", ui_data, [=]() {
     toggle_seamless();
  });

  ui_data["icon"] = "toolbar/ck_bin.png";
  ck_layout->add_widget(0, 2, "image_button", ui_data, [=]() {
      remove_space_request();
  });

  ui_data["icon"] = "toolbar/ck_switch.png";
  ck_layout->add_widget(0, 3, "image_button", ui_data, [=]() {
      qApp->quit();
  });

  ui_data["icon"] = "toolbar/ck_arrow-right.png";
  ck_layout->add_widget(0, 4, "image_button", ui_data, [=]() {
      switch_to_next_space();
  });

  ck_layout->viewport()->set_coordinates(
      0, 10 + priv->m_deskt_menu->window_title_height() +
             priv->m_task_grid->geometry().height());

  priv->m_deskt_menu->set_window_content(priv->m_task_grid);
}

void desktop_panel_controller_impl::insert_sub_action(ui_action &a_task) {
  ui_action_list child_actions = a_task.sub_actions();

  cherry_kit::window *sub_menu = new cherry_kit::window(0);

  sub_menu->set_window_type(cherry_kit::window::kPopupWindow);
  sub_menu->set_window_title(a_task.name().c_str());
  sub_menu->set_geometry(QRectF(0, 0, 400, 400));
  sub_menu->set_window_opacity(1.0);

  cherry_kit::item_view *sub_task_grid =
      new cherry_kit::item_view(sub_menu, cherry_kit::item_view::kGridModel);
  sub_task_grid->set_content_margin(0, 0, 0, 0);
  sub_task_grid->set_content_spacing(0);

  sub_task_grid->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

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

  if (x_count < 2)
    menu_width = 4;

  float window_width = (96 * menu_width);
  float window_height = (96 * (row_count));

  sub_task_grid->set_view_geometry(QRectF(
      0, 0, window_width, window_height + 52));

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

  /*sub menu toolbar */
  cherry_kit::fixed_layout *ck_layout =
      new cherry_kit::fixed_layout(sub_task_grid);

  ck_layout->set_content_margin(4, 4, 4, 4);
  ck_layout->set_geometry(0, 0, sub_task_grid->geometry().width(), 32);

  ck_layout->add_rows(1);
  ck_layout->add_segments(0, 5);

  ck_layout->set_row_height(0, "100%");

  cherry_kit::widget_properties_t ui_data;

  ui_data["icon"] = "toolbar/ck_arrow-left.png";
  ck_layout->add_widget(0, 0, "image_button", ui_data, [=]() {
      sub_menu->hide();
      priv->m_deskt_menu->show();
  });

  ck_layout->viewport()->set_coordinates(
      0, 10 + window_height);

  sub_menu->on_visibility_changed([=](window *a_window_ref, bool a_visible) {
    if (!a_visible) {
      sub_task_grid->clear();
      a_window_ref->close();
      // delete a_window_ref;
      delete ck_layout;
      delete sub_task_grid;
      delete sub_menu;
    }
  });

  sub_menu->set_window_content(sub_task_grid);
  insert(sub_menu);

  // window pos
  QPointF _menu_pos;
  _menu_pos.setX(priv->m_deskt_menu->x());
  _menu_pos.setY(priv->m_deskt_menu->y());

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
    if (priv->m_deskt_menu) {

      QPointF menu_pos;
      if (a_data.find("x") != a_data.end() &&
          a_data.find("y") != a_data.end()) {
        menu_pos.setX(std::stod(a_data.at("x")));
        menu_pos.setY(std::stod(a_data.at("y")));
      }

      priv->m_deskt_menu->setPos(menu_pos);
      priv->m_deskt_menu->show();
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

void desktop_panel_controller_impl::create_desktop_navigation_panel() {
  if (!viewport()) {
    return;
  }

  // desktop button : experimental
  priv->m_menu_btn = new desktop_button();
  priv->m_menu_btn->set_icon("navigation/ck_nav.png");
  priv->m_menu_btn->set_label("Menu");
  priv->m_menu_btn->set_action([=]() {
    exec_action("Menu", priv->m_menu_btn->window());
  });

  priv->m_menu_btn->window()->setPos(
      viewport()->center(priv->m_menu_btn->window()->geometry(), QRectF(),
                         space::kCenterOnViewportLeft));

  insert(priv->m_menu_btn->window());

  viewport()->on_viewport_event_notify([=](
      space::ViewportNotificationType aType,
      const cherry_kit::ui_task_data_t &a_data, const space *aSpace) {

    if (aType == space::kGeometryChangedNotification) {
      priv->m_menu_btn->window()->setPos(
          viewport()->center(priv->m_menu_btn->window()->geometry(), QRectF(),
                             space::kCenterOnViewportLeft));
      if (priv->m_up_btn) {
        QPointF up_btn_location =
            viewport()->center(priv->m_up_btn->window()->geometry(), QRectF(),
                               space::kCenterOnViewportLeft);
        up_btn_location.setY(up_btn_location.y() - 96);
        priv->m_up_btn->window()->setPos(up_btn_location);
      }

      if (priv->m_down_btn) {
        QPointF down_btn_location =
            viewport()->center(priv->m_down_btn->window()->geometry(), QRectF(),
                               space::kCenterOnViewportLeft);

        down_btn_location.setY(down_btn_location.y() + 96);
        priv->m_down_btn->window()->setPos(down_btn_location);
      }
    }
  });

  // move to next desktop button
  priv->m_up_btn = new desktop_button();
  priv->m_up_btn->set_icon("navigation/ck_up_arrow.png");
  priv->m_up_btn->set_label("Menu");
  priv->m_up_btn->set_action([=]() {
    exec_action("Up", priv->m_up_btn->window());
  });

  QPointF up_btn_location =
      viewport()->center(priv->m_up_btn->window()->geometry(), QRectF(),
                         space::kCenterOnViewportLeft);

  up_btn_location.setY(up_btn_location.y() - 96);
  priv->m_up_btn->window()->setPos(up_btn_location);

  priv->m_up_btn->window()->set_window_type(cherry_kit::window::kPopupWindow);
  insert(priv->m_up_btn->window());

  priv->m_up_btn->window()->hide();

  // move down button
  priv->m_down_btn = new desktop_button();
  priv->m_down_btn->set_icon("navigation/ck_down_arrow.png");
  priv->m_down_btn->set_label("Menu");
  priv->m_down_btn->set_action([=]() {
    exec_action("Down", priv->m_down_btn->window());
  });

  QPointF down_btn_location =
      viewport()->center(priv->m_down_btn->window()->geometry(), QRectF(),
                         space::kCenterOnViewportLeft);

  down_btn_location.setY(down_btn_location.y() + 96);
  priv->m_down_btn->window()->setPos(down_btn_location);

  priv->m_down_btn->window()->set_window_type(cherry_kit::window::kPopupWindow);
  insert(priv->m_down_btn->window());

  priv->m_down_btn->window()->hide();
}

void desktop_panel_controller_impl::init() {
  // loads the controllers before dock was created
  Q_FOREACH(const QString & name, viewport()->current_controller_list()) {
    discover_actions_from_controller(name);
  }

  viewport()->on_viewport_event_notify([=](
      space::ViewportNotificationType aType,
      const cherry_kit::ui_task_data_t &a_data, const space *aSpace) {

    if (aType == space::kControllerAddedNotification) {
      discover_actions_from_controller(
          QString::fromStdString(a_data.at("controller")));
    }
  });

  create_desktop_navigation_panel();

  if (priv->m_deskt_menu) {
    priv->m_deskt_menu->set_window_title("PlexyDesk 1.0");
    insert(priv->m_deskt_menu);
    priv->m_deskt_menu->hide();
  }
}

void desktop_panel_controller_impl::session_data_ready(
    const cherry_kit::sync_object &a_sesion_root) {}

void desktop_panel_controller_impl::submit_session_data(
    cherry_kit::sync_object *a_obj) {}

void desktop_panel_controller_impl::set_view_rect(const QRectF &rect) {
  if (!viewport()) {
    return;
  }

  if (priv->m_deskt_menu) {
    priv->m_deskt_menu->setPos(rect.x(), rect.y());
  }
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

  controller->task().execute("Hide");
}

void desktop_panel_controller_impl::prepare_removal() {
  // if (priv->m_task_window) {
  //   priv->m_task_window->discard();
  //}

    if (priv->m_expose_window) {
        priv->m_expose_window->close();
        delete priv->m_expose_window;
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

void desktop_panel_controller_impl::remove_space_request() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());
    if (_workspace) {
      _workspace->remove(viewport());
    }
  }
}

void desktop_panel_controller_impl::exec_action(const QString &action,
                                                cherry_kit::window *a_window) {
  if (action == tr("Close")) {
    remove_space_request();
    return;
  } else if (action == tr("Up")) {
    this->switch_to_previous_space();
  } else if (action == tr("Down")) {
    this->switch_to_next_space();
  } else if (action == tr("Hide")) {
    this->toggle_seamless();
  } else if (action == tr("Expose")) {
    update_desktop_preview();
  } else if (action == tr("Menu")) {
    if (!viewport() || !viewport()->owner_workspace()) {
      return;
    }
    update_desktop_preview();

    QPointF _menu_pos =
        viewport()->center(priv->m_deskt_menu->geometry(), QRectF(),
                           cherry_kit::space::kCenterOnViewportLeft);
    _menu_pos.setX(a_window->geometry().width() + 5);

    if (priv->m_deskt_menu) {
      priv->m_deskt_menu->setPos(_menu_pos);
      priv->m_deskt_menu->show();
    }

    if (priv->m_up_btn)
      priv->m_up_btn->window()->show();

    if (priv->m_down_btn)
      priv->m_down_btn->window()->show();

  } else if (action == tr("Add")) {
    add_new_space();
  }
}

void desktop_panel_controller_impl::add_new_space() {
  if (this->viewport() && this->viewport()->owner_workspace()) {
    cherry_kit::workspace *_workspace =
        qobject_cast<cherry_kit::workspace *>(viewport()->owner_workspace());

    if (_workspace) {
      if (_workspace->space_count() >= 12)
        return;

      space *_current_space = _workspace->current_active_space();
      if (_current_space)
          _current_space->reset_focus();

      _workspace->add_default_space();
    }
  }
}

void desktop_panel_controller_impl::load_desktop_expose(
        cherry_kit::item_view *preview_list,
        float preview_height, float preview_width)
{

    foreach(cherry_kit::space * _space,
            viewport()->owner_workspace()->current_spaces()) {
      cherry_kit::model_view_item *item = new cherry_kit::model_view_item();
      cherry_kit::image_view *image_view = new cherry_kit::image_view();
      QPixmap preview = viewport()->owner_workspace()->thumbnail(_space);

      image_view->on_click([=]() {
        viewport()->owner_workspace()->expose(_space->id());
      });

      image_view->set_pixmap(preview);
      image_view->set_contents_geometry(0, 0, preview_width, preview_height);

      item->set_view(image_view);

      preview_list->insert(item);
    }
}

void desktop_panel_controller_impl::remove_desktop_review(item_view *a_list,
                                                          int a_space_id)
{
  if (!a_list || !viewport())
    return;
  // todo
  qDebug() << Q_FUNC_INFO << "Not impl - Grid Model needs items removal";
  a_list->clear();

  /* default thumbnail returned is 10% of the actual desktop */
  float preview_width = (viewport()->geometry().width() / 100) * 10;
  float preview_height = (viewport()->geometry().height() / 100) * 10;
  
  // insert button
  cherry_kit::model_view_item *button_item = new cherry_kit::model_view_item();
  cherry_kit::icon_button *btn = new cherry_kit::icon_button();

  btn->set_contents_geometry(0, 0, 128, 128);
  btn->set_icon("navigation/ck_add.png");
  btn->on_click([=]() { add_new_space(); });

  button_item->set_view(btn);
  a_list->insert(button_item);

  load_desktop_expose(a_list, preview_height, preview_width);
}

void desktop_panel_controller_impl::insert_desktop_preview_item(
        cherry_kit::item_view *preview_list,
        float preview_height, float preview_width)
{
    int total_spaces = viewport()->owner_workspace()->current_spaces().count();
    int availble_preview_count = preview_list->count();

    space *_space = viewport()->owner_workspace()->get_space(total_spaces - 1);

    cherry_kit::model_view_item *item = new cherry_kit::model_view_item();
    cherry_kit::image_view *image_view = new cherry_kit::image_view();

    QPixmap preview = viewport()->owner_workspace()->thumbnail(_space);

    image_view->on_click([=]() {
      viewport()->owner_workspace()->expose(_space->id());
    });

    image_view->set_pixmap(preview);
    image_view->set_contents_geometry(0, 0, preview_width, preview_height);

    item->set_view(image_view);

    preview_list->insert(item);
}

void desktop_panel_controller_impl::create_desktop_preview() {
  if (!viewport())
    return;

  if (!priv->m_expose_window)
     priv->m_expose_window = new cherry_kit::window();

  /* default thumbnail returned is 10% of the actual desktop */
  float preview_width = (viewport()->geometry().width() / 100) * 10;
  float preview_height = (viewport()->geometry().height() / 100) * 10;

  cherry_kit::item_view *preview_list = new cherry_kit::item_view(
      priv->m_expose_window, cherry_kit::item_view::kGridModel);

  preview_list->set_column_count(11);
  preview_list->set_content_size(preview_width, preview_height);
  preview_list->set_view_geometry(
      QRectF(0, 0, viewport()->geometry().width(),
             (preview_height + priv->m_expose_window->window_title_height()) - 16 ));

  priv->m_expose_window->set_window_title("");
  priv->m_expose_window->set_geometry(
      QRectF(0, 0, viewport()->geometry().width(), preview_height));
  priv->m_expose_window->set_window_type(cherry_kit::window::kPopupWindow);

  priv->m_expose_window->set_window_opacity(0.5);

  priv->m_expose_window->on_visibility_changed([=](window *a_window_ref,
                                           bool a_visible) {
    if (!a_visible) {
      //preview_list->clear();
      //a_window_ref->close();
      //delete a_window_ref;
    }
  });

  viewport()->owner_workspace()->on_change([=](workspace::workspace_change_t a_change, int a_space_id) {
      if (a_change == workspace::kSpaceRemovedNotify) {
          //remove preview
          remove_desktop_review(preview_list, a_space_id);
	  viewport()->owner_workspace()->expose(0);
          return;
      }

      if (a_change == workspace::kSpaceAddedNotify) {
         // space added
          insert_desktop_preview_item(preview_list, preview_height, preview_width);
	  viewport()->owner_workspace()->expose(a_space_id);
          return;
      }
  });

  // cleanup
  preview_list->on_item_removed([=](cherry_kit::model_view_item *a_item) {
    delete a_item;
  });

  // insert button
  cherry_kit::model_view_item *button_item = new cherry_kit::model_view_item();
  cherry_kit::icon_button *btn = new cherry_kit::icon_button();

  btn->set_contents_geometry(0, 0, 128, 128);
  btn->set_icon("navigation/ck_add.png");
  btn->on_click([=]() { add_new_space(); });

  button_item->set_view(btn);
  preview_list->insert(button_item);

  // insert preview items.
  load_desktop_expose(preview_list, preview_height, preview_width);

  priv->m_expose_window->set_window_content(preview_list);

  QPointF menu_pos =
      viewport()->center(priv->m_expose_window->geometry(), QRectF(),
                         cherry_kit::space::kCenterOnViewportTop);

  priv->m_expose_window->set_coordinates(menu_pos.x(), menu_pos.y());

  insert(priv->m_expose_window);
}

void desktop_panel_controller_impl::update_desktop_preview() {
/*
  if (!priv->m_expose_window)
      create_desktop_preview();

  priv->m_expose_window->show();
*/
}
