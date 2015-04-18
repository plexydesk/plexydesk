/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  :
*
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
// internal
#include "clock.h"
#include "clockwidget.h"

#include <QDebug>

// uikit
#include <view_controller.h>
#include <extensionmanager.h>

// Qt
#include <QAction>

class _clock_session {
public:
  _clock_session() : m_purged(0){}
  ~_clock_session(){}

  void set_session_data(const QString &a_key, const QVariant &a_data);
  QVariant session_data(const QString &a_key) const;
  QList<QString> session_keys() const;

  void mark();
  bool is_purged();

  void set_session_id(int a_id);
  int session_id();

private:
  int m_session_id;
  QVariantMap m_session_data;
  bool m_purged;
};

class Clock::PrivateClockController
{
public:
  PrivateClockController() : m_clock_activity_count(0) {}
  ~PrivateClockController() {}

  void _new_session();
  void _end_session(int a_id);
  void _save_session();

  UIKit::ActionList m_supported_action_list;
  int m_clock_activity_count;

  QList<_clock_session> m_session_list;
};

Clock::Clock(QObject *parent)
  : UIKit::ViewController(parent), d(new PrivateClockController)
{
}

Clock::~Clock()
{
  qDebug() << Q_FUNC_INFO << "Deleted";
  delete d;
}

QAction *Clock::createAction(int id, const QString &action_name,
                             const QString &icon_name)
{
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 0);

  return _add_clock_action;
}

void Clock::init()
{
  d->m_supported_action_list << createAction(1, tr("Clock"),
                                          "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(2, tr("Timer"),
                                          "pd_clock_frame_icon.png");
  d->m_supported_action_list << createAction(3, tr("Alarm"),
                                          "pd_clock_frame_icon.png");
}

void Clock::set_view_rect(const QRectF &rect)
{
}

void Clock::session_data_available(QuetzalKit::SyncObject *a_session_root)
{
  if (!a_session_root || !a_session_root->hasChildren()) {
    return;
  }

  QuetzalKit::SyncObject *_session_object =
      a_session_root->childObject("Session");

  if (!_session_object || !_session_object->hasChildren())
    return;

  foreach(QuetzalKit::SyncObject *child_object, _session_object->childObjects()) {
      if (!child_object)
        continue;
      int _id = child_object->attributeValue("clock_id").toInt();
      QByteArray _zone = child_object->attributeValue("zone").toByteArray();

      QVariantMap _args;
      _args["id"] = _id;
      _args["zone_id"] = _zone;

      request_action("Clock", _args);
  }
}

void Clock::submit_session_data(QuetzalKit::SyncObject *a_obj,
                                QuetzalKit::DataStore *a_store)
{
  qDebug() << Q_FUNC_INFO
           << "Start Session Item List :"
           << d->m_session_list.count();

  if (!a_obj) {
    qDebug() << Q_FUNC_INFO << "Invalid session object root";
    return;
  }

  QuetzalKit::SyncObject *session_object = a_obj->childObject("Session");

  if (!session_object) {
    qDebug() << Q_FUNC_INFO << "Invalid session object child"
             << "object name ->"
             << a_obj->name()
             << " Child Object count ->"
             << a_obj->childCount();
    return;
  }

  qDebug() << Q_FUNC_INFO << "Start Session (item count): "
           << d->m_session_list.count();

  foreach(_clock_session session_ref, d->m_session_list) {
      if (session_ref.is_purged())
        continue;

     QuetzalKit::SyncObject *clock = session_object->createNewObject("clock");
     clock->setObjectAttribute("zone_id", session_ref.session_data("zone_id"));
     //session_object->addChildObject(clock);
     //a_store->insert(session_object);
     a_store->updateNode(session_object);
     qDebug() << Q_FUNC_INFO << "insert new clock to Session";
  }

  a_store->updateNode(session_object);
}

bool Clock::remove_widget(UIKit::Widget *widget)
{
  disconnect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)));
  int index = 0;

  return 1;
}

UIKit::ActionList Clock::actions() const { return d->m_supported_action_list; }

void Clock::sync_session()
{
  if (viewport()) {
      viewport()->update_session_value(controller_name(), "id", "");
      qDebug() << Q_FUNC_INFO;
  }
}

void Clock::request_action(const QString &actionName, const QVariantMap &args)
{
  if (!viewport()) {
    return;
  }

  if (actionName == tr("Clock")) {
    QRectF _view_geomeetry(0.0, 0.0, 260.0, 512.0);

    UIKit::DesktopActivityPtr _clock_activity =
      UIKit::ExtensionManager::instance()->activity("desktopclock");

    _clock_activity->create_window(_view_geomeetry, args["zone_id"].toString(),
                                  viewport()->center(_view_geomeetry));
    _clock_activity->update_attribute("id",
                                      QString("clock-%1").arg(
                                        d->m_clock_activity_count));
    d->_new_session();

    d->m_clock_activity_count++;
    _clock_activity->on_discarded([&](const UIKit::DesktopActivity *a_activity) {
       //remove from current session.
        if (!a_activity)
          return;

        QVariantMap attrib = a_activity->attributes();

        qDebug() << Q_FUNC_INFO << attrib;
        d->_end_session(0);

        sync_session();
      });

    if (viewport()) {
      viewport()->add_activity(_clock_activity);
      sync_session();
    }
  }
}

QString Clock::icon() const { return QString("pd_clock_frame_icon.png"); }

void Clock::onDataUpdated(const QVariantMap &data)
{
}

void Clock::PrivateClockController::_new_session()
{
  _clock_session session_ref;
  session_ref.set_session_id(m_session_list.count());
  m_session_list.push_back(session_ref);
  qDebug() << Q_FUNC_INFO << "New Session ID :" << m_session_list.count();
}

void Clock::PrivateClockController::_end_session(int a_id)
{
  foreach (_clock_session session_ref, m_session_list) {
      if (session_ref.session_id() == a_id) {
          session_ref.mark();
          qDebug() << Q_FUNC_INFO << " Delete from Session !";
      }
  }
}

void Clock::PrivateClockController::_save_session()
{
}

void _clock_session::set_session_data(const QString &a_key,
                                      const QVariant &a_data)
{
  m_session_data[a_key] = a_data;
}

QVariant _clock_session::session_data(const QString &a_key) const
{
  return m_session_data[a_key];
}

QList<QString> _clock_session::session_keys() const
{
  return m_session_data.keys();
}

void _clock_session::mark()
{
  m_purged = true;
}

bool _clock_session::is_purged()
{
  return m_purged;
}

void _clock_session::set_session_id(int a_id)
{
  m_session_id = a_id;
}

int _clock_session::session_id()
{
  return m_session_id;
}
