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
#include "dock.h"
#include "snapframe.h"

// Qt
#include <QGraphicsLinearLayout>

#include <toolbar.h>
#include <imagebutton.h>
#include <tableview.h>

#include <workspace.h>

// models
#include <default_table_model.h>
#include <default_table_component.h>
#include <item_view.h>

// core
#include <plexyconfig.h>
#include <extensionmanager.h>
#include <imageview.h>
#include <viewbuilder.h>

using namespace CherryKit;

class DockControllerImpl::PrivateDock {
public:
  PrivateDock() {}
  ~PrivateDock() { qDebug() << Q_FUNC_INFO; }

public:
  Window *m_dock_window;
  Window *m_preview_window;

  CherryKit::ItemView *m_preview_widget;

  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;

  CherryKit::DesktopActivityPtr m_action_activity;
  CherryKit::ImageButton *m_add_new_workspace_button_ptr;
  CherryKit::ActionList m_supported_action_list;
};

DockControllerImpl::DockControllerImpl(QObject *object)
    : CherryKit::ViewController(object), o_view_controller(new PrivateDock) {
  o_view_controller->m_actions_map["ToggleDock"] = 1;
  o_view_controller->m_actions_map["ShowDock"] = 2;
  o_view_controller->m_actions_map["HideDock"] = 3;
  o_view_controller->m_actions_map["HideDock"] = 4;
  o_view_controller->m_actions_map["ShowMenu"] = 5;

  o_view_controller->m_main_panel_is_hidden = true;

  // menu
  o_view_controller->m_preview_widget = new CherryKit::ItemView();
  o_view_controller->m_preview_widget->on_item_removed([this](
      CherryKit::ModelViewItem *a_item) {
    if (a_item) {
      delete a_item;
    }
  });
  o_view_controller->m_preview_widget->on_activated([this](int index) {
    if (this->viewport() && this->viewport()->workspace()) {
      CherryKit::WorkSpace *_workspace =
          qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

      if (_workspace) {
        _workspace->expose(index);
      }
    }
  });
}

DockControllerImpl::~DockControllerImpl() {
  qDebug() << Q_FUNC_INFO;
  delete o_view_controller;
}

void DockControllerImpl::init() {
  o_view_controller->m_supported_action_list << createAction(1, tr("Menu"), "pd_menu_icon.png");
  o_view_controller->m_supported_action_list << createAction(2, tr("show-dock"),
                                             "pd_menu_icon.png");
  o_view_controller->m_supported_action_list << createAction(3, tr("hide-dock"),
                                             "pd_menu_icon.png");
  o_view_controller->m_supported_action_list << createAction(4, tr("show-expose"),
                                             "pd_menu_icon.png");
  o_view_controller->m_supported_action_list << createAction(5, tr("hide-expose"),
                                             "pd_menu_icon.png");

  if (!viewport()) {
    return;
  }

  CherryKit::Space *_space = viewport();
  if (_space) {
    o_view_controller->m_action_activity =
        createActivity("", "icongrid", "", QPoint(), QVariantMap());

    if (o_view_controller->m_action_activity && o_view_controller->m_action_activity->window()) {
      o_view_controller->m_action_activity->window()->set_window_type(Window::kPopupWindow);
      o_view_controller->m_action_activity->window()->setVisible(false);
    }
  }

  // loads the controllers before dock was created;
  Q_FOREACH(const QString & name, viewport()->current_controller_list()) {
    loadControllerActions(name);
  }

  viewport()->on_viewport_event_notify([this](
      Space::ViewportNotificationType aType, const QVariant &aData,
      const Space *aSpace) {

    if (aType == Space::kControllerAddedNotification) {
      loadControllerActions(aData.toString());
    }
  });

  o_view_controller->m_dock_window = new CherryKit::Window();
  o_view_controller->m_dock_window->set_window_type(Window::kPanelWindow);

  o_view_controller->m_preview_window = new CherryKit::Window();

  o_view_controller->m_preview_window->set_window_type(Window::kPopupWindow);
  o_view_controller->m_preview_window->enable_window_background(false);

  // navigation
  CherryKit::HybridLayout *build =
      new CherryKit::HybridLayout(o_view_controller->m_dock_window);
  build->set_content_margin(6, 10, 10, 10);
  build->set_geometry(0, 0, 48 + 16, 48 * 7);
  build->set_horizontal_segment_count(7);

  std::string default_height = std::to_string((48.0 / (48.0 * 7)) * 100) + "%";

  for (int i = 0; i < 7; i++) {
    build->add_horizontal_segments(i, 1);
    build->set_horizontal_height(i, default_height);
  }

  CherryKit::WidgetProperties accept_button_prop;

  accept_button_prop["label"] = "Up";
  accept_button_prop["icon"] = "actions/pd_to_top.png";
  build->add_widget(0, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Expose";
  accept_button_prop["icon"] = "actions/pd_view_grid.png";
  build->add_widget(1, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Add";
  accept_button_prop["icon"] = "actions/pd_add.png";
  build->add_widget(2, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Menu";
  accept_button_prop["icon"] = "actions/pd_overflow_tab.png";
  build->add_widget(3, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Desktop";
  accept_button_prop["icon"] = "actions/pd_browser.png";
  build->add_widget(4, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Close";
  accept_button_prop["icon"] = "actions/pd_delete.png";
  build->add_widget(5, 0, "image_button", accept_button_prop);

  accept_button_prop["label"] = "Dow";
  accept_button_prop["icon"] = "actions/pd_to_bottom.png";
  build->add_widget(6, 0, "image_button", accept_button_prop);

  // connect to notifications.
  for (int i = 0; i < 7; i++) {
    CherryKit::ImageButton *btn =
        dynamic_cast<CherryKit::ImageButton *>(build->at(i, 0));

    if (!btn)
      continue;

    CherryKit::Widget::InputCallback callback = [=](
        CherryKit::Widget::InputEvent a_event,
        const CherryKit::Widget *a_widget) {
      if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
        onNavigationPanelClicked(btn->label());
      }
    };

    btn->on_input_event(callback);
  }

  // base->setGeometry(build->ui()->geometry());
  o_view_controller->m_dock_window->set_window_content(build->viewport());
  o_view_controller->m_preview_window->set_window_content(o_view_controller->m_preview_widget);

  insert(o_view_controller->m_dock_window);
  insert(o_view_controller->m_preview_window);

  o_view_controller->m_preview_window->hide();
}

void DockControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_sesion_root) {}

void DockControllerImpl::submit_session_data(QuetzalKit::SyncObject *a_obj) {}

void DockControllerImpl::set_view_rect(const QRectF &rect) {
  if (!viewport()) {
    return;
  }

  o_view_controller->m_dock_window->setPos(viewport()->center(
      o_view_controller->m_dock_window->geometry(), QRectF(), Space::kCenterOnViewportLeft));

  o_view_controller->m_preview_widget->set_view_geometry(
      QRectF(0.0, 0.0, 256, rect.height() - 24.0));

  o_view_controller->m_preview_window->setGeometry(QRectF(0.0, 0.0, 256, rect.height()));

  o_view_controller->m_preview_window->setPos(
      rect.x() + o_view_controller->m_dock_window->geometry().width() + 5, rect.y() + 24.0);

  o_view_controller->m_preview_window->hide();

  if (o_view_controller->m_action_activity) {
    o_view_controller->m_action_activity->window()->setPos(rect.x(), rect.y());
  }
}

ActionList DockControllerImpl::actions() const {
  return o_view_controller->m_supported_action_list;
}

void DockControllerImpl::request_action(const QString &actionName,
                                        const QVariantMap &args) {
  if (actionName.toLower() == "menu") {

    if (o_view_controller->m_action_activity && o_view_controller->m_action_activity->window()) {
      o_view_controller->m_action_activity->window()->setPos(args["menu_pos"].toPoint());
      o_view_controller->m_action_activity->window()->show();
    }

    return;
  } else if (actionName.toLower() == "show-dock") {
    o_view_controller->m_dock_window->show();
    return;
  } else if (actionName.toLower() == "hide-dock") {
    o_view_controller->m_dock_window->hide();
    return;
  } else if (actionName.toLower() == "show-expose") {
    if (o_view_controller->m_preview_window->isVisible()) {
      o_view_controller->m_preview_window->hide();
    } else {
      updatePreview();
      o_view_controller->m_preview_window->show();
    }
  } else if (actionName.toLower() == "hide-expose") {
    o_view_controller->m_preview_window->hide();
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

QString DockControllerImpl::icon() const {
  return QString("pd_desktop_icon.png");
}

void DockControllerImpl::createActionForController(const QString &name,
                                                   const QPointF &pos) {
  if (!viewport()) {
    return;
  }

  viewport()->controller(name)->configure(pos);
}

void DockControllerImpl::createActivityForController(const QString &name) {
  if (!viewport()) {
    return;
  }
}

DesktopActivityPtr DockControllerImpl::createActivity(
    const QString &controllerName, const QString &activity,
    const QString &title, const QPoint &pos, const QVariantMap &dataItem) {
  QPoint _activity_location = pos;

  CherryKit::DesktopActivityPtr _intent = viewport()->create_activity(
      activity, title, _activity_location,
      QRectF(0, _activity_location.y(), 484, 420), dataItem);
  _intent->set_controller(CherryKit::ViewControllerPtr(this));
  _intent->set_activity_attribute("data", QVariant(dataItem));
  _intent->set_activity_attribute("auto_scale", QVariant(1));

  return _intent;
}

void DockControllerImpl::nextSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->expose_next();
    }
  }
}

void DockControllerImpl::toggleSeamless() {
  if (!viewport()) {
    return;
  }

  CherryKit::ViewControllerPtr controller =
      viewport()->controller("classicbackdrop");

  if (!controller) {
    qWarning() << Q_FUNC_INFO << "Controller Not Found";
    return;
  }

  controller->request_action("Seamless");
}

void DockControllerImpl::prepare_removal() {
  if (viewport() && viewport()->workspace()) {
    QGraphicsView *_workspace =
        qobject_cast<QGraphicsView *>(viewport()->workspace());

    if (_workspace) {
      o_view_controller->m_action_activity.clear();
    } else {
      if (o_view_controller->m_action_activity) {
        o_view_controller->m_action_activity.clear();
      }
    }
  } else {
    qWarning() << Q_FUNC_INFO << "Error : Missing workspace or viewport";
  }
}

void DockControllerImpl::previousSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->expose_previous();
    }
  }
}

void DockControllerImpl::toggleDesktopPanel() {
  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      QRectF _work_area = viewport()->geometry();

      if (!o_view_controller->m_main_panel_is_hidden) {
        _work_area.setX(331.0f);
        _work_area.setWidth(_work_area.width() + 330.0f);
      }

      o_view_controller->m_main_panel_is_hidden = !o_view_controller->m_main_panel_is_hidden;

      _workspace->expose_sub_region(_work_area);
    }
  }
}

void DockControllerImpl::loadControllerActions(const QString &name) {
  if (o_view_controller->m_controller_name_list.contains(name) || !viewport()) {
    return;
  }

  CherryKit::ViewControllerPtr controller = viewport()->controller(name);

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

  o_view_controller->m_action_activity->update_attribute("data", _data);
}

void DockControllerImpl::onActivityAnimationFinished() {
  if (!sender()) {
    return;
  }

  CherryKit::DesktopActivity *activity =
      qobject_cast<CherryKit::DesktopActivity *>(sender());

  if (!activity) {
    return;
  }

  CherryKit::Widget *_activity_widget =
      qobject_cast<CherryKit::Widget *>(activity->window());

  if (_activity_widget) {
    _activity_widget->set_widget_flag(CherryKit::Widget::kRenderDropShadow,
                                      false);
    _activity_widget->set_widget_flag(CherryKit::Widget::kConvertToWindowType,
                                      false);
    _activity_widget->set_widget_flag(CherryKit::Widget::kRenderBackground,
                                      true);
    _activity_widget->setFlag(QGraphicsItem::ItemIsMovable, false);
    _activity_widget->setPos(QPoint());
  }
}

void DockControllerImpl::onActivityFinished() {
  CherryKit::DesktopActivity *_activity =
      qobject_cast<CherryKit::DesktopActivity *>(sender());

  if (!_activity) {
    return;
  }

  CherryKit::ViewControllerPtr _controller =
      viewport()->controller(_activity->result()["controller"].toString());

  if (!_controller) {
    return;
  }

  _controller->request_action(_activity->result()["action"].toString(),
                              QVariantMap());
}

void DockControllerImpl::removeSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());
    if (_workspace) {
      _workspace->remove(viewport());
    }
  }
}

void DockControllerImpl::onNavigationPanelClicked(const QString &action) {
  if (action == tr("Close")) {
    removeSpace();
    return;
  } else if (action == tr("Up")) {
    this->previousSpace();
  } else if (action == tr("Down")) {
    this->nextSpace();
  } else if (action == tr("Seamless")) {
    this->toggleSeamless();
  } else if (action == tr("Expose")) {
    if (o_view_controller->m_preview_window->isVisible()) {
      o_view_controller->m_preview_window->hide();
      if (o_view_controller->m_preview_widget)
        o_view_controller->m_preview_widget->clear();
    } else {
      updatePreview();
      o_view_controller->m_preview_window->show();
    }
    return;
  } else if (action == tr("Menu")) {
    if (!viewport() || !viewport()->workspace()) {
      return;
    }

    QPointF _menu_pos =
        viewport()->center(o_view_controller->m_action_activity->window()->boundingRect(),
                           QRectF(), CherryKit::Space::kCenterOnViewportLeft);
    _menu_pos.setX(o_view_controller->m_dock_window->geometry().width() + 5);

    if (o_view_controller->m_action_activity && o_view_controller->m_action_activity->window()) {
      o_view_controller->m_action_activity->window()->setPos(_menu_pos);
      o_view_controller->m_action_activity->window()->show();
    }

  } else if (action == tr("Add")) {
    onAddSpaceButtonClicked();
  }
}

void DockControllerImpl::onAddSpaceButtonClicked() {
  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      _workspace->add_default_space();
    }
  }
}

QAction *DockControllerImpl::createAction(int id, const QString &action_name,
                                          const QString &icon_name) {
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 1);

  return _add_clock_action;
}

void DockControllerImpl::updatePreview() {
  o_view_controller->m_preview_widget->clear();

  if (this->viewport() && this->viewport()->workspace()) {
    CherryKit::WorkSpace *_workspace =
        qobject_cast<CherryKit::WorkSpace *>(viewport()->workspace());

    float lHeight = 10;
    float lWidth = 0;

    if (_workspace) {
      foreach(CherryKit::Space * _space, _workspace->current_spaces()) {
        QPixmap _preview = _workspace->thumbnail(_space);

        CherryKit::ImageView *p = new CherryKit::ImageView();

        p->setMinimumSize(_preview.size());
        p->set_pixmap(_preview);
        lHeight += _preview.size().height();
        lWidth = _preview.size().width();

        CherryKit::ModelViewItem *model_item = new CherryKit::ModelViewItem();
        model_item->set_view(p);
        model_item->on_view_removed([=](CherryKit::ModelViewItem *a_item) {
          if (a_item && a_item->view()) {
            CherryKit::Widget *view = a_item->view();
            if (view)
              delete view;
          }
        });

        o_view_controller->m_preview_widget->insert(model_item);
      }
    }

    QPointF lMenuPos =
        viewport()->center(o_view_controller->m_action_activity->window()->boundingRect(),
                           QRectF(), CherryKit::Space::kCenterOnViewportLeft);

    lMenuPos.setX(o_view_controller->m_dock_window->geometry().width() + 5.0);
    o_view_controller->m_preview_window->setGeometry(
        QRectF(lMenuPos.x(), lMenuPos.y(), lWidth, lHeight + 24));
  }
}
