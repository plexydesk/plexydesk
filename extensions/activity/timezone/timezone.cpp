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
#include "timezone.h"
#include <widget.h>
#include <plexyconfig.h>

#include <QTimer>
#include <QTimeZone>
#include <QLocale>

#include <view_controller.h>
#include <modelview.h>
#include <label.h>
#include <button.h>

class TimeZoneActivity::PrivateTimeZone
{
public:
  PrivateTimeZone() {}
  ~PrivateTimeZone()
  {
    if (mWindowPtr) {
      delete mWindowPtr;
    }
    qDebug() << Q_FUNC_INFO << "Delete TimeZone Activity";
  }

  UIKit::Window *mWindowPtr;
  UIKit::ModelView *mTimeZoneBrowserPtr;

  QVariantMap m_result_data;
};

TimeZoneActivity::TimeZoneActivity(QGraphicsObject *aParent)
  : UIKit::DesktopActivity(aParent), mPrivConstPtr(new PrivateTimeZone) {}

TimeZoneActivity::~TimeZoneActivity() { delete mPrivConstPtr; }

void TimeZoneActivity::create_window(const QRectF &aWindowGeometry,
                                    const QString &aWindowTitle,
                                    const QPointF &aWindowPos)
{
  mPrivConstPtr->mWindowPtr = new UIKit::Window();

  mPrivConstPtr->mWindowPtr->set_window_title(aWindowTitle);
  mPrivConstPtr->mTimeZoneBrowserPtr =
    new UIKit::ModelView(mPrivConstPtr->mWindowPtr);
  mPrivConstPtr->mTimeZoneBrowserPtr->setGeometry(aWindowGeometry);
  mPrivConstPtr->mTimeZoneBrowserPtr->set_view_geometry(aWindowGeometry);

  mPrivConstPtr->mWindowPtr->set_window_content(
    mPrivConstPtr->mTimeZoneBrowserPtr);

  set_geometry(aWindowGeometry);

  exec(aWindowPos);

  mPrivConstPtr->mWindowPtr->on_window_discarded(
              [this](UIKit::Window * aWindow) {
    discard_activity();
  });

  loadTimeZones();
}

QVariantMap TimeZoneActivity::result() const
{
    return mPrivConstPtr->m_result_data;
}

void TimeZoneActivity::update_attribute(const QString &aName,
                                       const QVariant &aVariantData) {}

UIKit::Window *TimeZoneActivity::window() const
{
    return mPrivConstPtr->mWindowPtr;
}

void TimeZoneActivity::cleanup()
{
  if (mPrivConstPtr->mWindowPtr) {
    delete mPrivConstPtr->mWindowPtr;
  }
  mPrivConstPtr->mWindowPtr = 0;
}

void TimeZoneActivity::loadTimeZones()
{
  QStringList l_timezone_list;
  foreach(const QByteArray id,  QTimeZone::availableTimeZoneIds()) {
   QLocale::Country l_country_locale = QTimeZone(id).country();
   QString l_time_zone_lable_str =
           QLocale::countryToString(l_country_locale);
   l_timezone_list << l_time_zone_lable_str + " " + QTimeZone(id).displayName(
                          QDateTime::currentDateTime(), QTimeZone::OffsetName);
  }

  l_timezone_list.sort();

  foreach(const QString &label, l_timezone_list) {
    UIKit::Label *lTimeZoneLabelPtr =
      new UIKit::Label(mPrivConstPtr->mTimeZoneBrowserPtr);

    lTimeZoneLabelPtr->set_alignment(Qt::AlignLeft);

    lTimeZoneLabelPtr->setMinimumSize(
      mPrivConstPtr->mTimeZoneBrowserPtr->geometry().width(),
      32);
    lTimeZoneLabelPtr->set_size(
                QSizeF(
                    mPrivConstPtr->mTimeZoneBrowserPtr->boundingRect().width(),
                    32));
    lTimeZoneLabelPtr->set_label(label);
    mPrivConstPtr->mTimeZoneBrowserPtr->insert(lTimeZoneLabelPtr);

    lTimeZoneLabelPtr->on_input_event([this](UIKit::Widget::InputEvent a_type,
                                      const UIKit::Widget *a_widget_ptr) {
        if (a_type != UIKit::Widget::kMouseReleaseEvent)
            return;

        if (a_widget_ptr) {
            const UIKit::Label *l_label_ptr =
                    qobject_cast<const UIKit::Label *>(a_widget_ptr);
            if (l_label_ptr) {
                mPrivConstPtr->m_result_data["timezone"] = l_label_ptr->label();
                activate_response();
            }
        }
    });
  }
}
