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

#include "chooser.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <tableviewcellinterface.h>
#include <tableview.h>
#include <widget.h>
#include <imageview.h>
#include <themepackloader.h>
#include <modelview.h>
#include <label.h>
#include "chooseritem.h"
#include "chooseractiondelegate.h"

class Action
{
public:
  Action() {}
  ~Action() {}

  QString controller_name() const;
  void set_controller_name(const QString &controller_name);

  Widget *createActionItem(const QString &aIcon,
                           const QString &aLabel,
                           const QString &aControllerName);
  void onActionActivated(
    std::function<void(const Action *aAction)> aHandler)
  {
    m_action_handler = aHandler;
  }

  QString label() const;
  void setLabel(const QString &label);

private:
  QString m_controller_name;
  QString m_label;
  QString m_icon;

  std::function<void (const Action *aAction)> m_action_handler;
};

class IconGridActivity::PrivateIconGrid
{
public:
  PrivateIconGrid() : m_activity_window_ptr(0) {}
  ~PrivateIconGrid()
  {
    qDeleteAll(m_action_list);
  }

  UIKit::Window *m_activity_window_ptr;
  UIKit::TableView *mTable;
  UIKit::ModelView *m_grid_view;

  QString mSelection;

  QVariantMap m_activity_result;

  ChooserActionDelegate *m_action_delegate;
  bool m_auto_scale_frame;

  QMap<int, QVariant> m_action_map;
  QList<Action *> m_action_list;

};

IconGridActivity::IconGridActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateIconGrid) {}

IconGridActivity::~IconGridActivity()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void IconGridActivity::create_window(const QRectF &window_geometry,
                                    const QString &window_title,
                                    const QPointF &window_pos)
{
  if (d->m_activity_window_ptr) {
    return;
  }

  d->m_auto_scale_frame = false;

  d->m_activity_window_ptr = new UIKit::Window();
  d->m_activity_window_ptr->set_window_title(window_title);
  d->m_activity_window_ptr->setGeometry(window_geometry);


  d->m_grid_view = new UIKit::ModelView(d->m_activity_window_ptr,
                                        UIKit::ModelView::kGridModel);
  d->m_grid_view->setViewGeometry(window_geometry);

  on_arguments_updated([this]() {
    if (has_attribute("data")) {
      QVariantMap data = attributes()["data"].toMap();

      foreach(const QVariant & var, data.values()) {
        QVariantMap _item = var.toMap();
        Action *l_action_item = new Action;
        d->m_action_list.append(l_action_item);
        l_action_item->onActionActivated([this](const Action * aAction) {
          qDebug() << Q_FUNC_INFO << "Hello world : " <<
                   aAction->controller_name();
          d->m_activity_result.clear();
          d->m_activity_result["controller"] =
            aAction->controller_name();
          d->m_activity_result["action"] = aAction->label();
          d->mSelection = aAction->label();
          update_action();
        });

        d->m_grid_view->insert(
          l_action_item->createActionItem(_item["icon"].toString(),
                                          _item["label"].toString(),
                                          _item["controller"].toString()));
      }
    }

    if (has_attribute("auto_scale")) {
      d->m_auto_scale_frame = attributes()["auto_scale"].toBool();
    }

    if (d->m_auto_scale_frame) {
      QRectF _content_rect = d->m_grid_view->boundingRect();
      _content_rect.setHeight(_content_rect.height() + 64);
      set_geometry(_content_rect);
      d->m_activity_window_ptr->setGeometry(_content_rect);
    }
  });

  d->m_activity_window_ptr->set_window_content(d->m_grid_view);
  d->m_activity_window_ptr->on_window_discarded([this](UIKit::Window * aWindow) {
    discard_activity();
  });

  exec(window_pos);
}

QVariantMap IconGridActivity::result() const
{
  d->m_activity_result["action"] = d->mSelection;
  return d->m_activity_result;
}

Window *IconGridActivity::window() const { return d->m_activity_window_ptr; }

void IconGridActivity::cleanup()
{
  if (d->m_activity_window_ptr) {
    delete d->m_activity_window_ptr;
  }

  d->m_activity_window_ptr = 0;
}

void IconGridActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onDiscard()));
  discard_activity();
}

void IconGridActivity::onDiscard() { Q_EMIT finished(); }

///action class impl
Widget *Action::createActionItem(const QString &aIcon,
                                 const QString &aLabel,
                                 const QString &aControllerName)
{
  m_controller_name = aControllerName;
  m_label = aLabel;
  m_icon = aIcon;

  Widget *l_rv = new Widget();

  UIKit::ImageView *l_image_view = new UIKit::ImageView(l_rv);
  UIKit::Label *l_action_label = new UIKit::Label(l_rv);
  l_action_label->setLabel(aLabel);
  l_action_label->set_widget_name(aLabel);

  QPixmap l_view_pixmap(UIKit::Theme::instance()->drawable(
                          aIcon, "hdpi"));
  l_image_view->setMinimumSize(l_view_pixmap.size());
  l_image_view->setSize(l_view_pixmap.size());
  l_image_view->setPixmap(l_view_pixmap);

  l_action_label->setSize(QSizeF(l_image_view->boundingRect().width(), 32));
  l_action_label->setPos(0, l_image_view->boundingRect().height());

  l_rv->setGeometry(l_image_view->geometry());

  QSizeF l_action_item_size;
  l_action_item_size.setHeight(l_image_view->boundingRect().height() +
                               l_action_label->boundingRect().height());
  l_action_item_size.setWidth(l_image_view->boundingRect().width());

  l_rv->setMinimumSize(l_action_item_size);

  l_image_view->on_input_event([this](UIKit::Widget::InputEvent aEvent,
  const Widget * aWidget) {
    if (aEvent == UIKit::Widget::kMouseReleaseEvent) {
      if (m_action_handler) {
        m_action_handler(this);
      }
    }
  });

  return l_rv;
}
QString Action::label() const
{
  return m_label;
}

void Action::setLabel(const QString &label)
{
  m_label = label;
}

QString Action::controller_name() const
{
  return m_controller_name;
}

void Action::set_controller_name(const QString &controller_name)
{
  m_controller_name = controller_name;
}
