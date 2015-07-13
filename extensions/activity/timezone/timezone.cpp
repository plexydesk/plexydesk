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
#include "timezone.h"
#include <widget.h>
#include <plexyconfig.h>

#include <QTimer>
#include <QTimeZone>
#include <QLocale>

#include <view_controller.h>
#include <item_view.h>
#include <label.h>
#include <button.h>
#include <lineedit.h>

class timezone_dialog::PrivateTimeZone {
public:
  PrivateTimeZone() {}
  ~PrivateTimeZone() {
    if (m_timezone_browser_ptr) {
      m_timezone_browser_ptr->clear();
      delete m_timezone_browser_ptr;
      m_timezone_browser_ptr = 0;
    }
    if (m_window_ptr) {
      delete m_window_ptr;
    }
    qDebug() << Q_FUNC_INFO << "Delete TimeZone Activity";
  }

  cherry_kit::window *m_window_ptr;
  cherry_kit::widget *m_content_widget_ptr;
  cherry_kit::ItemView *m_timezone_browser_ptr;
  cherry_kit::LineEdit *m_filter_widget_ptr;

  QVariantMap m_result_data;
};

timezone_dialog::timezone_dialog(QGraphicsObject *aParent)
    : cherry_kit::desktop_dialog(aParent), m_priv_ptr(new PrivateTimeZone) {}

timezone_dialog::~timezone_dialog() { delete m_priv_ptr; }

void timezone_dialog::create_window(const QRectF &aWindowGeometry,
                                     const QString &aWindowTitle,
                                     const QPointF &aWindowPos) {
  m_priv_ptr->m_window_ptr = new cherry_kit::window();
  m_priv_ptr->m_window_ptr->set_window_title(aWindowTitle);

  m_priv_ptr->m_content_widget_ptr =
      new cherry_kit::widget(m_priv_ptr->m_window_ptr);
  m_priv_ptr->m_content_widget_ptr->setGeometry(aWindowGeometry);

  m_priv_ptr->m_filter_widget_ptr =
      new cherry_kit::LineEdit(m_priv_ptr->m_content_widget_ptr);
  m_priv_ptr->m_filter_widget_ptr->setMinimumSize(aWindowGeometry.width() - 16,
                                                  32);
  m_priv_ptr->m_filter_widget_ptr->setGeometry(QRectF(8, 0, 0, 0));

  m_priv_ptr->m_timezone_browser_ptr =
      new cherry_kit::ItemView(m_priv_ptr->m_content_widget_ptr);
  /*
  m_priv_ptr->m_timezone_browser_ptr->setGeometry(
        QRectF(0, 0, aWindowGeometry.width(),
               aWindowGeometry.height() -
               m_priv_ptr->m_filter_widget_ptr->minimumHeight()));
               */
  m_priv_ptr->m_timezone_browser_ptr->set_view_geometry(
      QRectF(0, 0, aWindowGeometry.width() - 16,
             aWindowGeometry.height() -
                 m_priv_ptr->m_filter_widget_ptr->minimumHeight()));

  m_priv_ptr->m_timezone_browser_ptr->setPos(
      18, m_priv_ptr->m_filter_widget_ptr->minimumHeight() + 8);

  m_priv_ptr->m_filter_widget_ptr->show();

  m_priv_ptr->m_window_ptr->set_window_content(
      m_priv_ptr->m_content_widget_ptr);

  set_geometry(aWindowGeometry);

  exec(aWindowPos);

  m_priv_ptr->m_filter_widget_ptr->on_insert([&](const QString &a_txt) {
    m_priv_ptr->m_timezone_browser_ptr->set_filter(a_txt);
  });

  m_priv_ptr->m_window_ptr->on_window_closed([this](
      cherry_kit::window *aWindow) {
    if (m_priv_ptr->m_timezone_browser_ptr) {
      m_priv_ptr->m_timezone_browser_ptr->clear();
      delete m_priv_ptr->m_timezone_browser_ptr;
      m_priv_ptr->m_timezone_browser_ptr = 0;
    }
  });

  m_priv_ptr->m_timezone_browser_ptr->on_item_removed([](
      cherry_kit::ModelViewItem *a_item) {
    if (a_item)
      delete a_item;
  });

  loadTimeZones();
}

QVariantMap timezone_dialog::result() const {
  return m_priv_ptr->m_result_data;
}

void timezone_dialog::update_attribute(const QString &aName,
                                        const QVariant &aVariantData) {}

cherry_kit::window *timezone_dialog::activity_window() const {
  return m_priv_ptr->m_window_ptr;
}

void timezone_dialog::cleanup() {
  if (m_priv_ptr->m_timezone_browser_ptr)
    m_priv_ptr->m_timezone_browser_ptr->clear();

  if (m_priv_ptr->m_window_ptr) {
    delete m_priv_ptr->m_window_ptr;
  }
  m_priv_ptr->m_window_ptr = 0;
}

void timezone_dialog::loadTimeZones() {
  std::vector<cherry_kit::ModelViewItem *> _item_list;

  foreach(const QByteArray id, QTimeZone::availableTimeZoneIds()) {
    QString l_time_zone_lable_str = QString(id);
    l_time_zone_lable_str +=
        " " + QTimeZone(id).displayName(QDateTime::currentDateTime(),
                                        QTimeZone::OffsetName);

    cherry_kit::Label *lTimeZoneLabelPtr =
        new cherry_kit::Label(m_priv_ptr->m_timezone_browser_ptr);
    cherry_kit::ModelViewItem *l_item = new cherry_kit::ModelViewItem();

    l_item->set_data("label", l_time_zone_lable_str);
    l_item->set_data("zone_id", id);
    l_item->on_activated([&](cherry_kit::ModelViewItem *a_item) {
      if (a_item) {
        m_priv_ptr->m_result_data["timezone"] =
            a_item->data("label").toString();
        m_priv_ptr->m_result_data["zone_id"] =
            a_item->data("zone_id").toByteArray();
        notify_done();
      }
    });

    l_item->set_view(lTimeZoneLabelPtr);
    l_item->on_view_removed([&](cherry_kit::ModelViewItem *a_item) {
      if (a_item && a_item->view()) {
        cherry_kit::widget *view = a_item->view();
        if (view)
          delete view;
      }
    });
    l_item->on_filter([&](const cherry_kit::widget *a_view,
                          const QString &a_keyword) {
      const cherry_kit::Label *_lbl_widget =
          dynamic_cast<const cherry_kit::Label *>(a_view);

      if (_lbl_widget) {
        if (_lbl_widget->label().toLower().contains(a_keyword.toLower())) {
          return 1;
        }
      }

      return 0;
    });

    lTimeZoneLabelPtr->set_alignment(Qt::AlignLeft);

    lTimeZoneLabelPtr->setMinimumSize(
        m_priv_ptr->m_timezone_browser_ptr->geometry().width(), 32);
    lTimeZoneLabelPtr->set_size(
        QSizeF(m_priv_ptr->m_timezone_browser_ptr->boundingRect().width(), 32));
    lTimeZoneLabelPtr->set_label(l_time_zone_lable_str);
    _item_list.push_back(l_item);
  }

  std::sort(_item_list.begin(), _item_list.end(),
            [](cherry_kit::ModelViewItem *a_a, cherry_kit::ModelViewItem *a_b) {
    return a_a->data("label").toString() < a_b->data("label").toString();
  });

  std::for_each(_item_list.begin(), _item_list.end(),
                [this](cherry_kit::ModelViewItem *a) {
    if (a) {
      m_priv_ptr->m_timezone_browser_ptr->insert(a);
    }
  });

  _item_list.clear();
}
