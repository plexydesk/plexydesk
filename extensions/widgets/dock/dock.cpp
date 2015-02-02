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

#include <menu.h>
#include <toolbar.h>
#include <imagebutton.h>
#include <tableview.h>

#include <workspace.h>

// models
#include <default_table_model.h>
#include <default_table_component.h>
#include <modelview.h>

// core
#include <plexyconfig.h>
#include <extensionmanager.h>
#include <imageview.h>

using namespace PlexyDesk;

class DockControllerImpl::PrivateDock {
public:
  PrivateDock() {}

  ~PrivateDock() { qDebug() << Q_FUNC_INFO; }

public:
  QGraphicsLinearLayout* m_linear_layout;
  PlexyDesk::ToolBar* m_navigation_dock;
  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;
  PlexyDesk::DesktopActivityPtr m_action_activity;
  PlexyDesk::ImageButton* m_add_new_workspace_button_ptr;
  QSharedPointer<PlexyDesk::DesktopActivityMenu> m_desktop_actions_popup;
  PlexyDesk::ActionList m_supported_action_list;

  //spaces preview
  PlexyDesk::ModelView *m_preview_widget;
};

DockControllerImpl::DockControllerImpl(QObject* object)
    : PlexyDesk::ViewController(object), d(new PrivateDock) {
  d->m_actions_map["ToggleDock"] = 1;
  d->m_actions_map["ShowDock"] = 2;
  d->m_actions_map["HideDock"] = 3;
  d->m_actions_map["HideDock"] = 4;
  d->m_actions_map["ShowMenu"] = 5;

  d->m_main_panel_is_hidden = true;
  // navigation
  d->m_navigation_dock = new PlexyDesk::ToolBar();
  d->m_navigation_dock->setController(this);
  d->m_navigation_dock->setOrientation(Qt::Vertical);
  d->m_navigation_dock->setIconResolution("mdpi");
  d->m_navigation_dock->setIconSize(QSize(32, 32));

  d->m_navigation_dock->addAction(tr("Up"), "pd_up_icon", false);
  d->m_navigation_dock->addAction(tr("Expose"), "pd_space_icon", false);
  d->m_navigation_dock->addAction(tr("Add"), "pd_add_new_icon", false);
  d->m_navigation_dock->addAction(tr("Menu"), "pd_menu_icon", false);
  d->m_navigation_dock->addAction(tr("Seamless"), "pd_eye_icon", false);
  d->m_navigation_dock->addAction(tr("Close"), "pd_eye_icon", false);
  d->m_navigation_dock->addAction(tr("Down"), "pd_down_icon", false);

  d->m_navigation_dock->setGeometry(d->m_navigation_dock->frameGeometry());

  connect(d->m_navigation_dock, SIGNAL(action(QString)), this,
          SLOT(onNavigationPanelClicked(QString)));
  // menu
  d->m_preview_widget = new PlexyDesk::ModelView();
  d->m_preview_widget->hide();

  d->m_desktop_actions_popup = QSharedPointer<PlexyDesk::DesktopActivityMenu>(
      new PlexyDesk::DesktopActivityMenu(this));
}

DockControllerImpl::~DockControllerImpl() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void DockControllerImpl::init() {
  d->m_supported_action_list << createAction(1, tr("Menu"), "pd_menu_icon.png");
  d->m_supported_action_list
      << createAction(2, tr("show-dock"), "pd_menu_icon.png");
  d->m_supported_action_list
      << createAction(3, tr("hide-dock"), "pd_menu_icon.png");
  d->m_supported_action_list
      << createAction(4, tr("show-expose"), "pd_menu_icon.png");
  d->m_supported_action_list
      << createAction(5, tr("hide-expose"), "pd_menu_icon.png");

  if (!viewport())
    return;

  PlexyDesk::Space* _space = qobject_cast<PlexyDesk::Space*>(viewport());
  if (_space) {
    d->m_action_activity =
        createActivity("", "icongrid", "", QPoint(), QVariantMap());
    d->m_desktop_actions_popup->setSpace(_space);
    d->m_desktop_actions_popup->setActivity(d->m_action_activity);
    _space->addActivityPoupToView(d->m_desktop_actions_popup);
  }

  // loads the controllers before dock was created;
  Q_FOREACH(const QString & name, viewport()->currentDesktopControllers()) {
    this->onControllerAdded(name);
  }

  connect(viewport(), SIGNAL(controllerAdded(QString)), this,
          SLOT(onControllerAdded(QString)));

  insert(d->m_navigation_dock);
  insert(d->m_preview_widget);

  d->m_preview_widget->hide();
}

void DockControllerImpl::revokeSession(const QVariantMap& args) {}

void DockControllerImpl::setViewRect(const QRectF& rect) {
  if (!viewport())
    return;

  d->m_navigation_dock->setPos(
      viewport()->center(d->m_navigation_dock->frameGeometry(),
                         DesktopViewport::kCenterOnViewportLeft));

  d->m_preview_widget->setViewGeometry(
      QRectF(0.0, 0.0, 256, rect.height() - 24.0));

  d->m_preview_widget->setPos(
      rect.x() + d->m_navigation_dock->frameGeometry().width() + 5,
      rect.y() + 24.0);

 d->m_preview_widget->hide();

  if (d->m_action_activity) {
    d->m_action_activity->window()->setPos(0.0, 0.0);
  }
}

ActionList DockControllerImpl::actions() const {
  return d->m_supported_action_list;
}

void DockControllerImpl::requestAction(const QString& actionName,
                                       const QVariantMap& args) {
  if (actionName.toLower() == "menu") {
    d->m_desktop_actions_popup->exec(args["menu_pos"].toPoint());
    return;
  } else if (actionName.toLower() == "show-dock") {
    d->m_navigation_dock->show();
    return;
  } else if (actionName.toLower() == "hide-dock") {
    d->m_navigation_dock->hide();
    return;
  } else if (actionName.toLower() == "show-expose") {
      if (d->m_preview_widget->isVisible()) {
          d->m_preview_widget->hide();
      } else {
          updatePreview();
          d->m_preview_widget->show();
      }
  } else if (actionName.toLower() == "hide-expose") {
      d->m_preview_widget->hide();
  }

  if (viewport() && viewport()->controller(args["controller"].toString())) {
    viewport()->controller(args["controller"].toString())->requestAction(
        actionName, args);
  } else {
    qWarning() << Q_FUNC_INFO << "Unknown Action";
  }
}

QString DockControllerImpl::icon() const {
  return QString("pd_desktop_icon.png");
}

void DockControllerImpl::createActionForController(const QString& name,
                                                   const QPointF& pos) {
  if (!viewport()) {
    return;
  }

  viewport()->controller(name)->configure(pos);
}

void DockControllerImpl::createActivityForController(const QString& name) {
  if (!viewport()) {
    return;
  }
}

DesktopActivityPtr DockControllerImpl::createActivity(
    const QString& controllerName, const QString& activity,
    const QString& title, const QPoint& pos, const QVariantMap& dataItem) {
  PlexyDesk::DesktopActivityPtr _intent =
      PlexyDesk::ExtensionManager::instance()->activity(activity);

  if (!_intent) {
    qWarning() << Q_FUNC_INFO << "No such Activity: " << activity;
    return DesktopActivityPtr();
  }

  QPoint _activity_location = pos;

  _activity_location.setY(viewport()->geometry().y());
  _intent->setActivityAttribute("data", QVariant(dataItem));
  _intent->setActivityAttribute("auto_scale", QVariant(1));

  _intent->setController(PlexyDesk::ControllerPtr(this));

  connect(_intent.data(), SIGNAL(showAnimationFinished()), this,
          SLOT(onActivityAnimationFinished()));

  _intent->createWindow(QRectF(0.0, _activity_location.y(), 330.0, 320.0),
                        title, QPointF());

  PlexyDesk::UIWidget* _activity_widget =
      qobject_cast<PlexyDesk::UIWidget*>(_intent->window());

  if (_activity_widget) {
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow, true);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType,
                                    false);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow, true);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground, true);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderWindowTitle,
                                    true);
  }

  return _intent;
}

void DockControllerImpl::nextSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    PlexyDesk::WorkSpace* _workspace =
        qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->exposeNextSpace();
    }
  }
}

void DockControllerImpl::toggleSeamless() {
  if (!viewport())
    return;

  PlexyDesk::ControllerPtr controller =
      viewport()->controller("classicbackdrop");

  if (!controller) {
    qWarning() << Q_FUNC_INFO << "Controller Not Found";
    return;
  }

  controller->requestAction("Seamless");
}

void DockControllerImpl::prepareRemoval() {
  if (viewport() && viewport()->workspace()) {
    QGraphicsView* _workspace =
        qobject_cast<QGraphicsView*>(viewport()->workspace());

    if (_workspace) {
      if (d->m_action_activity->window()) {
        d->m_action_activity.clear();
      }

      d->m_desktop_actions_popup.clear();
      d->m_action_activity.clear();
    } else {
      if (d->m_action_activity) {
        d->m_action_activity.clear();
      }
    }
  } else {
    qWarning() << Q_FUNC_INFO << "Error : Missing workspace or viewport";
  }
}

void DockControllerImpl::previousSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    PlexyDesk::WorkSpace* _workspace =
        qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->exposePreviousSpace();
    }
  }
}

void DockControllerImpl::toggleDesktopPanel() {
  if (this->viewport() && this->viewport()->workspace()) {
    PlexyDesk::WorkSpace* _workspace =
        qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

    if (_workspace) {
      QRectF _work_area = viewport()->geometry();

      if (!d->m_main_panel_is_hidden) {
        _work_area.setX(331.0f);
        _work_area.setWidth(_work_area.width() + 330.0f);
      }

      d->m_main_panel_is_hidden = !d->m_main_panel_is_hidden;

      _workspace->exposeSubRegion(_work_area);
    }
  }
}

void DockControllerImpl::onControllerAdded(const QString& name) {
  if (d->m_controller_name_list.contains(name) || !viewport())
    return;

  PlexyDesk::ControllerPtr controller = viewport()->controller(name);

  if (!controller)
    return;

  if (controller->actions().count() <= 0)
    return;

  QVariantMap _data;

  Q_FOREACH(QAction * action, controller->actions()) {
    if (!action)
      continue;

    QVariantMap _item;

    bool _is_hidden = action->property("hidden").toBool();

    if (_is_hidden)
      continue;

    _item["label"] = action->text();
    _item["icon"] = action->property("icon_name");
    _item["controller"] = QVariant(name);
    _item["id"] = action->property("id");

    _data[QString("%1.%2").arg(name).arg(action->text())] = _item;
  }

  if (_data.keys().count() <= 0)
    return;

  d->m_action_activity->updateAttribute("data", _data);
}

void DockControllerImpl::onActivityAnimationFinished() {
  if (!sender())
    return;

  PlexyDesk::DesktopActivity* activity =
      qobject_cast<PlexyDesk::DesktopActivity*>(sender());

  if (!activity)
    return;

  PlexyDesk::UIWidget* _activity_widget =
      qobject_cast<PlexyDesk::UIWidget*>(activity->window());

  if (_activity_widget) {
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow,
                                    false);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType,
                                    false);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow, false);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground, true);
    _activity_widget->setWindowFlag(PlexyDesk::UIWidget::kRenderWindowTitle,
                                    true);

    _activity_widget->setFlag(QGraphicsItem::ItemIsMovable, false);
    _activity_widget->setPos(QPoint());
  }
}

void DockControllerImpl::onActivityFinished() {
  PlexyDesk::DesktopActivity* _activity =
      qobject_cast<PlexyDesk::DesktopActivity*>(sender());

  if (!_activity)
    return;

  PlexyDesk::ControllerPtr _controller =
      viewport()->controller(_activity->result()["controller"].toString());

  if (!_controller)
    return;

  _controller->requestAction(_activity->result()["action"].toString(),
                             QVariantMap());
}

void DockControllerImpl::removeSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    PlexyDesk::WorkSpace* _workspace =
        qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());
    if (_workspace) {
      _workspace->removeSpace(qobject_cast<PlexyDesk::Space*>(viewport()));
    }
  }
}

void DockControllerImpl::onNavigationPanelClicked(const QString& action) {
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
    if (d->m_preview_widget->isVisible()) {
        qDebug() << Q_FUNC_INFO << "curren visible";
        d->m_preview_widget->hide();
    } else {
        updatePreview();
        d->m_preview_widget->show();
    }
    return;
  } else if (action == tr("Menu")) {
    if (!viewport() || !viewport()->workspace())
      return;

    QPointF _menu_pos =
        viewport()->center(d->m_action_activity->window()->boundingRect(),
                           PlexyDesk::DesktopViewport::kCenterOnViewportLeft);
    _menu_pos.setX(d->m_navigation_dock->frameGeometry().width() + 5);
    d->m_desktop_actions_popup->exec(_menu_pos);
  } else if (action == tr("Add")) {
    onAddSpaceButtonClicked();
  }
}

void DockControllerImpl::onAddSpaceButtonClicked() {
  if (this->viewport() && this->viewport()->workspace()) {
    PlexyDesk::WorkSpace* _workspace =
        qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

    if (_workspace) {
      _workspace->addSpace();
    }
  }
}

QAction* DockControllerImpl::createAction(int id, const QString& action_name,
                                          const QString& icon_name) {
  QAction* _add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 1);

  return _add_clock_action;
}

void DockControllerImpl::updatePreview()
{
    d->m_preview_widget->clear();

    if (this->viewport() && this->viewport()->workspace()) {
        PlexyDesk::WorkSpace* _workspace =
                qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

        if (_workspace) {
            foreach(PlexyDesk::Space * _space, _workspace->currentSpaces()) {
                QPixmap _preview = _workspace->previewSpace(_space);

                PlexyDesk::ImageView *p = new PlexyDesk::ImageView();
                p->setMinimumSize(_preview.size());
                p->setPixmap(_preview);

                d->m_preview_widget->insert(p);
            }
        }
    }
}

void DockControllerImpl::onPreviewItemClicked(TableViewItem* item) {
  DefaultTableComponent* _item = qobject_cast<DefaultTableComponent*>(item);

  if (_item) {
    bool _ok = false;
    int _space_id = _item->label().toUInt(&_ok);

    if (_ok) {
      if (this->viewport() && this->viewport()->workspace()) {
        PlexyDesk::WorkSpace* _workspace =
            qobject_cast<PlexyDesk::WorkSpace*>(viewport()->workspace());

        if (_workspace) {
          _workspace->exposeSpace(_space_id);
        }
      }
    }
  }
}
