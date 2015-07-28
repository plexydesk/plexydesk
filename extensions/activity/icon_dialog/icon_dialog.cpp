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

#include "icon_dialog.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_table_model.h>
#include <ck_TableView.h>
#include <ck_widget.h>
#include <ck_image_view.h>
#include <ck_resource_manager.h>
#include <ck_item_view.h>
#include <ck_label.h>
#include "chooseritem.h"
#include "chooseractiondelegate.h"

class Action {
public:
  Action() {}
  ~Action() {}

  QString controller_name() const;
  void set_controller_name(const QString &controller_name);

  widget *createActionItem(const QString &aIcon, const QString &aLabel,
                           const QString &aControllerName);
  void onActionActivated(std::function<void(const Action *aAction)> aHandler) {
    m_action_handler = aHandler;
  }

  QString label() const;
  void setLabel(const QString &label);

private:
  QString m_controller_name;
  QString m_label;
  QString m_icon;

  std::function<void(const Action *aAction)> m_action_handler;
};

class icon_dialog::PrivateIconGrid {
public:
  PrivateIconGrid() : window_ref(0) {}
  ~PrivateIconGrid() { qDeleteAll(m_action_list); }

  cherry_kit::window *window_ref;
  cherry_kit::item_view *m_grid_view;

  QString mSelection;

  QVariantMap m_activity_result;

  bool m_auto_scale_frame;

  QMap<int, QVariant> m_action_map;
  QList<Action *> m_action_list;
};

icon_dialog::icon_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object), priv(new PrivateIconGrid) {}

icon_dialog::~icon_dialog() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
}

void icon_dialog::create_window(const QRectF &window_geometry,
                                const QString &window_title,
                                const QPointF &window_pos) {
  if (priv->window_ref) {
    return;
  }

  priv->m_auto_scale_frame = false;

  priv->window_ref = new cherry_kit::window();
  priv->window_ref->set_window_title(window_title);
  priv->window_ref->setGeometry(window_geometry);

  priv->m_grid_view = new cherry_kit::item_view(
      priv->window_ref, cherry_kit::item_view::kGridModel);
  priv->m_grid_view->set_view_geometry(window_geometry);

  priv->m_grid_view->on_item_removed([](cherry_kit::model_view_item *a_item) {
    if (a_item)
      delete a_item;
  });

  priv->window_ref->set_window_content(priv->m_grid_view);

  on_arguments_updated([this]() {
    if (has_attribute("data")) {
      QVariantMap data = attributes()["data"].toMap();

      foreach(const QVariant & var, data.values()) {
        QVariantMap _item = var.toMap();
        Action *l_action_item = new Action;
        priv->m_action_list.append(l_action_item);
        l_action_item->onActionActivated([this](const Action *aAction) {
          priv->m_activity_result.clear();
          priv->m_activity_result["controller"] = aAction->controller_name();
          priv->m_activity_result["action"] = aAction->label();
          priv->mSelection = aAction->label();
          update_action();
        });

        cherry_kit::model_view_item *grid_item =
            new cherry_kit::model_view_item();

        grid_item->on_view_removed([](cherry_kit::model_view_item *a_item) {
          if (a_item && a_item->view()) {
            cherry_kit::widget *view = a_item->view();
            if (view)
              delete view;
          }
        });

        grid_item->set_view(l_action_item->createActionItem(
            _item["icon"].toString(), _item["label"].toString(),
            _item["controller"].toString()));

        priv->m_grid_view->insert(grid_item);
        QRectF _content_rect = priv->m_grid_view->boundingRect();

        set_geometry(_content_rect);

        priv->window_ref->setGeometry(_content_rect);
      }
    }

    if (has_attribute("auto_scale")) {
      priv->m_auto_scale_frame = attributes()["auto_scale"].toBool();
    }

    if (priv->m_auto_scale_frame) {
      QRectF _content_rect = priv->m_grid_view->boundingRect();
      _content_rect.setWidth(_content_rect.width() + 2);
      _content_rect.setHeight(_content_rect.height() + 2);

      priv->m_grid_view->set_view_geometry(_content_rect);
      priv->m_grid_view->setGeometry(_content_rect);
      priv->window_ref->resize(_content_rect.width(), _content_rect.height());
    }
  });

  exec(window_pos);
}

QVariantMap icon_dialog::result() const {
  priv->m_activity_result["action"] = priv->mSelection;
  return priv->m_activity_result;
}

window *icon_dialog::dialog_window() const { return priv->window_ref; }

void icon_dialog::cleanup() {
  if (priv->m_grid_view)
    priv->m_grid_view->clear();

  if (priv->window_ref) {
    delete priv->window_ref;
  }

  priv->window_ref = 0;
}

/// action class impl
widget *Action::createActionItem(const QString &aIcon, const QString &aLabel,
                                 const QString &aControllerName) {
  m_controller_name = aControllerName;
  m_label = aLabel;
  m_icon = aIcon;

  QSizeF item_icon_size(48, 48);
  QSizeF item_label_size(48, 16);

  widget *l_rv = new widget();

  cherry_kit::image_view *l_image_view = new cherry_kit::image_view(l_rv);
  cherry_kit::label *l_action_label = new cherry_kit::label(l_rv);
  l_action_label->set_text(aLabel);
  l_action_label->set_widget_name(aLabel);

  QPixmap l_view_pixmap(
      cherry_kit::resource_manager::instance()->drawable(aIcon, "mdpi"));
  l_image_view->set_pixmap(l_view_pixmap);
  l_image_view->setMinimumSize(item_icon_size);
  l_image_view->set_size(item_icon_size);

  l_action_label->set_size(item_label_size);
  l_action_label->setPos(0, l_image_view->geometry().height());

  l_rv->setGeometry(l_image_view->geometry());

  QSizeF l_action_item_size;
  l_action_item_size.setHeight(l_image_view->boundingRect().height() +
                               l_action_label->boundingRect().height());
  l_action_item_size.setWidth(l_image_view->boundingRect().width());

  l_rv->setMinimumSize(l_action_item_size);

  l_image_view->on_input_event([this](cherry_kit::widget::InputEvent aEvent,
                                      const widget *aWidget) {
    if (aEvent == cherry_kit::widget::kMouseReleaseEvent) {
      if (m_action_handler) {
        m_action_handler(this);
      }
    }
  });

  return l_rv;
}

QString Action::label() const { return m_label; }

void Action::setLabel(const QString &label) { m_label = label; }

QString Action::controller_name() const { return m_controller_name; }

void Action::set_controller_name(const QString &controller_name) {
  m_controller_name = controller_name;
}
