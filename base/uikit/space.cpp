#include "space.h"

#include <QAction>
#include <QDebug>
#include <QDropEvent>
#include <QGraphicsItem>
#include <datastore.h>
#include <disksyncengine.h>
#include <extensionmanager.h>
#include <syncobject.h>
#include <QGraphicsScene>
#include <QWeakPointer>
#include <widget.h>

#include "window.h"
#include "workspace.h"

namespace UIKit
{
typedef std::function<void (Space::ViewportNotificationType,
                            const QVariant &,
                            const Space *)> NotifyFunc;
class Space::PrivateSpace
{
public:
  PrivateSpace() {}

  ~PrivateSpace();

  QString sessionNameForSpace();

  void initSessionStorage(Space *space);

  QString sessionNameForController(const QString &controllerName);

  void createActionsFromController(const Space *space, ViewControllerPtr ptr);

public:
  int mID;
  QString mName;
  QRectF mDesktopRect;
  WorkSpace *mWorkSpace;
  QGraphicsScene *mMainScene;
  QList<UIKit::DesktopActivityPtr> mActivityList;
  std::list<UIKit::Window *> mWindowList;
  QMap<QString, ViewControllerPtr> mCurrentControllerMap;
  QList<ViewControllerPtr> mControllerList;

  QList<NotifyFunc> m_notify_chain;
};

Space::Space(QObject *a_parent) : QObject(a_parent), m_priv_impl(new PrivateSpace)
{
  m_priv_impl->mWorkSpace = 0;
  m_priv_impl->mMainScene = 0;
}

Space::~Space()
{
  clear();
  delete m_priv_impl;
}

void Space::add_controller(const QString &a_name)
{
  if (m_priv_impl->mCurrentControllerMap.keys().contains(a_name)) {
    return;
  }

  QSharedPointer<ViewController> controllerPtr =
    (UIKit::ExtensionManager::instance()->controller(a_name));

  if (!controllerPtr) {
    qWarning() << Q_FUNC_INFO << "Error loading extension" << a_name;
    return;
  }

  m_priv_impl->mCurrentControllerMap[a_name] = controllerPtr;

  controllerPtr->setViewport(this);
  controllerPtr->setControllerName(a_name);

  controllerPtr->init();

  m_priv_impl->createActionsFromController(this, controllerPtr);

  register_controller(a_name);

  foreach(NotifyFunc l_notify_handler, m_priv_impl->m_notify_chain) {
    if (l_notify_handler)
      l_notify_handler(Space::kControllerAddedNotification,
                       QVariant(a_name),
                       this);
  }
}

UIKit::DesktopActivityPtr Space::create_activity(const QString &a_activity,
    const QString &a_title,
    const QPointF &a_pos,
    const QRectF &a_rect,
    const QVariantMap &a_data_map)
{
  UIKit::DesktopActivityPtr intent =
    UIKit::ExtensionManager::instance()->activity(a_activity);

  if (!intent) {
    qWarning() << Q_FUNC_INFO << "No such Activity: " << a_activity;
    return UIKit::DesktopActivityPtr();
  }

  intent->setActivityAttribute("data", QVariant(a_data_map));

  intent->createWindow(a_rect, a_title, QPointF(a_pos.x(), a_pos.y()));

  if (intent->window()) {
    intent->window()->set_window_title(a_title);
    intent->window()->set_window_viewport(this);
  }

  add_activity(intent);

  return intent;
}

void Space::update_session_value(const QString &a_name,
                                 const QString &a_key, const QString &a_value)
{
  if (a_key.isEmpty()) {
    return;
  }

  QuetzalKit::DataStore *dataStore = new QuetzalKit::DataStore(
    m_priv_impl->sessionNameForController(a_name), this);
  QuetzalKit::DiskSyncEngine *engine =
    new QuetzalKit::DiskSyncEngine(dataStore);

  dataStore->setSyncEngine(engine);

  QuetzalKit::SyncObject *_session_list_ptr = dataStore->begin("SessionData");

  if (!_session_list_ptr) {
    return;
  }

  if (!_session_list_ptr->hasChildren()) {
    QuetzalKit::SyncObject *_session_object_ptr =
      _session_list_ptr->createNewObject("Session");

    _session_object_ptr->setObjectAttribute(a_key, a_value);
    dataStore->insert(_session_object_ptr);
    delete dataStore;
  } else {
    Q_FOREACH(QuetzalKit::SyncObject * _child_session_object,
              _session_list_ptr->childObjects()) {
      if (!_child_session_object) {
        continue;
      }
      _child_session_object->setObjectAttribute(a_key, a_value);
      dataStore->updateNode(_child_session_object);
    }

    delete dataStore;
  }
}

void Space::add_activity(UIKit::DesktopActivityPtr a_activity_ptr)
{
  if (!a_activity_ptr) {
    return;
  }

  connect(a_activity_ptr.data(), SIGNAL(finished()), this,
          SLOT(on_activity_finished()));

  if (a_activity_ptr->window() && m_priv_impl->mMainScene) {
    if (m_priv_impl->mActivityList.contains(a_activity_ptr)) {
      qWarning() << Q_FUNC_INFO << "Space already contains the activity";
      return;
    }

    m_priv_impl->mActivityList << a_activity_ptr;

    a_activity_ptr->setViewport(this);

    insert_window_to_view(a_activity_ptr->window());
  }
}

void Space::insert_window_to_view(Window *a_window)
{
  if (!a_window) {
    return;
  }

  if (!m_priv_impl->mMainScene) {
    qWarning() << Q_FUNC_INFO << "Scene not Set";
    return;
  }

  QPointF _center_of_space_location = this->geometry().center();
  QPoint _widget_location;

  _widget_location.setX(_center_of_space_location.x() -
                        a_window->boundingRect().width() / 2);
  _widget_location.setY(_center_of_space_location.y() -
                        a_window->boundingRect().height() / 2);

  m_priv_impl->mMainScene->addItem(a_window);
  a_window->setPos(_widget_location);

  connect(a_window, SIGNAL(closed(UIKit::Widget *)), this,
          SLOT(remove_window_from_view(UIKit::Widget *)));

  a_window->show();

  if (a_window->controller()) {
    a_window->controller()->setViewRect(m_priv_impl->mDesktopRect);
  }

  m_priv_impl->mWindowList.push_front(a_window);

  a_window->on_window_closed([this](Window * window) {
    qDebug() << Q_FUNC_INFO << "Request Window Removal from Space";
    remove_window_from_view(window);
  });
}

void Space::remove_window_from_view(Window *a_window)
{
  if (!m_priv_impl->mMainScene) {
    return;
  }

  if (!a_window) {
    return;
  }

  if (a_window->controller()) {
    if (a_window->controller()->removeWidget(a_window)) {
      return;
    }
  }

  m_priv_impl->mMainScene->removeItem(a_window);

  qDebug() << Q_FUNC_INFO << "Before :" << m_priv_impl->mWindowList.size();
  m_priv_impl->mWindowList.remove(a_window);
  qDebug() << Q_FUNC_INFO << "After:" << m_priv_impl->mWindowList.size();

  a_window->discard();
}

void Space::on_viewport_event_notify(std::function<void (ViewportNotificationType,
                                     const QVariant &,
                                     const Space *)> a_notifyHandler)
{
  m_priv_impl->m_notify_chain.append(a_notifyHandler);
}

void Space::on_activity_finished()
{
  qDebug() << Q_FUNC_INFO;
  DesktopActivity *activity = qobject_cast<DesktopActivity *>(sender());

  if (activity) {

    if (activity->controller()) {
      activity->controller()->requestAction(
        activity->result()["action"].toString(), activity->result());
    }

    int i = 0;
    foreach(DesktopActivityPtr _activity, m_priv_impl->mActivityList) {
      if (_activity.data() == activity) {
        _activity.clear();
        qDebug() << Q_FUNC_INFO << "Before :" << m_priv_impl->mActivityList.count();
        m_priv_impl->mActivityList.removeAt(i);
        m_priv_impl->mActivityList.removeAll(_activity);
        qDebug() << Q_FUNC_INFO << "After :" << m_priv_impl->mActivityList.count();
      }
      i++;
    }
  }
}

void Space::register_controller(const QString &aControllerName)
{
  qDebug() << Q_FUNC_INFO << "Controller :" << aControllerName;
  QuetzalKit::DataStore *_data_store =
    new QuetzalKit::DataStore(m_priv_impl->sessionNameForSpace(), this);
  QuetzalKit::DiskSyncEngine *_engine =
    new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr =
    _data_store->begin("ControllerList");

  bool _has_controller = false;

  Q_FOREACH(QuetzalKit::SyncObject * _child_object_ptr,
            _session_list_ptr->childObjects()) {
    if (!_child_object_ptr) {
      continue;
    }

    if (_child_object_ptr->attributeValue("name").toString() == aControllerName) {
      _has_controller = true;
    }
  }

  if (!_has_controller) {
    QuetzalKit::SyncObject *_controller_ptr =
      _session_list_ptr->createNewObject("Controller");

    _controller_ptr->setObjectAttribute("name", aControllerName);
    _data_store->insert(_controller_ptr);
  }

  delete _data_store;

  // create a new data  Store to link it to this store.
  QuetzalKit::DataStore *_linked_sub_store = new QuetzalKit::DataStore(
    m_priv_impl->sessionNameForController(aControllerName), this);
  _engine = new QuetzalKit::DiskSyncEngine(_linked_sub_store);
  _linked_sub_store->setSyncEngine(_engine);

  if (!_linked_sub_store->beginsWith("SessionData")) {
    // create blank root
    QuetzalKit::SyncObject *_attribute_list =
      _linked_sub_store->begin("SessionData");
    delete _linked_sub_store;
    return;
  }

  QVariantMap _session_data;
  QuetzalKit::SyncObject *_session_attribute_list =
    _linked_sub_store->begin("SessionData");

  qDebug() << Q_FUNC_INFO
           << "Child Count:" << _session_attribute_list->childCount();

  Q_FOREACH(QuetzalKit::SyncObject * _session_ptr,
            _session_attribute_list->childObjects()) {
    if (!_session_ptr) {
      continue;
    }

    Q_FOREACH(const QString & attribute, _session_ptr->attributes()) {
      _session_data[attribute] =
        _session_ptr->attributeValue(attribute).toString();
    }
  }

  if (controller(aControllerName)) {
    controller(aControllerName)->revokeSession(_session_data);
  }

  delete _linked_sub_store;
}

void Space::PrivateSpace::createActionsFromController(const Space *space,
    ViewControllerPtr ptr)
{
  Q_FOREACH(const QAction * action, ptr->actions()) {
    qDebug() << action->text();
    qDebug() << action->icon();
  }
}

void Space::PrivateSpace::initSessionStorage(Space *space)
{
  QuetzalKit::DataStore *_data_store =
    new QuetzalKit::DataStore(sessionNameForSpace(), space);
  QuetzalKit::DiskSyncEngine *_engine =
    new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr =
    _data_store->begin("ControllerList");

  if (_session_list_ptr) {
    Q_FOREACH(const QuetzalKit::SyncObject * _object,
              _session_list_ptr->childObjects()) {
      if (!_object) {
        continue;
      }

      QString _current_controller_name =
        _object->attributeValue("name").toString();

      ViewControllerPtr _controller_ptr =
        space->controller(_current_controller_name);

      if (!_controller_ptr) {
        space->add_controller(_current_controller_name);
        continue;
      }
    }
  }

  delete _data_store;
}

Space::PrivateSpace::~PrivateSpace() { mCurrentControllerMap.clear(); }

QString Space::PrivateSpace::sessionNameForSpace()
{
  return QString("%1_Space_%2").arg(mName).arg(mID);
}

QString Space::PrivateSpace::sessionNameForController(
  const QString &controllerName)
{
  return QString("%1_Controller_%2").arg(sessionNameForSpace()).arg(
           controllerName);
}

QString Space::session_name() const { return m_priv_impl->sessionNameForSpace(); }

void Space::clear()
{
  int i = 0;
  foreach(DesktopActivityPtr _activity, m_priv_impl->mActivityList) {
    qDebug() << Q_FUNC_INFO << "Remove Activity: ";
    if (_activity) {
      m_priv_impl->mActivityList.removeAt(i);
    }
    i++;
  }

  // delete owner widgets
  for (Window *_widget : m_priv_impl->mWindowList) {
    if (_widget) {
      if (m_priv_impl->mMainScene->items().contains(_widget)) {
        m_priv_impl->mMainScene->removeItem(_widget);
      }
      _widget->discard();
      qDebug() << Q_FUNC_INFO << "Widget Deleted: OK";
    }
  }

  m_priv_impl->mWindowList.clear();
  // remove spaces which belongs to the space.
  foreach(const QString & _key, m_priv_impl->mCurrentControllerMap.keys()) {
    qDebug() << Q_FUNC_INFO << _key;
    ViewControllerPtr _controller_ptr = m_priv_impl->mCurrentControllerMap[_key];
    _controller_ptr->prepareRemoval();
    qDebug() << Q_FUNC_INFO
             << "Before Removal:" << m_priv_impl->mCurrentControllerMap.count();
    m_priv_impl->mCurrentControllerMap.remove(_key);
    qDebug() << Q_FUNC_INFO
             << "After Removal:" << m_priv_impl->mCurrentControllerMap.count();
  }

  m_priv_impl->mCurrentControllerMap.clear();
}

QPointF Space::cursor_pos() const
{
  if (!m_priv_impl->mWorkSpace) {
    return QCursor::pos();
  }

  QGraphicsView *_view_parent = qobject_cast<QGraphicsView *>(m_priv_impl->mWorkSpace);

  if (!_view_parent) {
    return QCursor::pos();
  }

  return _view_parent->mapToScene(QCursor::pos());
}

QPointF Space::center(const QRectF &a_view_geometry,
                      const ViewportLocation &a_location) const
{
  QPointF _rv;
  float _x_location;
  float _y_location;

  switch (a_location) {
  case kCenterOnViewport:
    _y_location = (geometry().height() / 2) - (a_view_geometry.height() / 2);
    _x_location = (geometry().width() / 2) - (a_view_geometry.width() / 2);
    break;
  case kCenterOnViewportLeft:
    _y_location = (geometry().height() / 2) - (a_view_geometry.height() / 2);
    _x_location = (geometry().topLeft().x());
    break;
  case kCenterOnViewportRight:
    _y_location = (geometry().height() / 2) - (a_view_geometry.height() / 2);
    _x_location = (geometry().width() - (a_view_geometry.width()));
    break;
  case kCenterOnViewportTop:
    _y_location = (0.0);
    _x_location = ((geometry().width() / 2) - (a_view_geometry.width() / 2));
    break;
  case kCenterOnViewportBottom:
    _y_location = (geometry().height() - (a_view_geometry.height()));
    _x_location = ((geometry().width() / 2) - (a_view_geometry.width() / 2));
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Error : Unknown Viewprot Location Type:";
  }

  _rv.setY(geometry().y() + _y_location);
  _rv.setX(_x_location);

  return _rv;

}

ViewControllerPtr Space::controller(const QString &a_name)
{
  if (!m_priv_impl->mCurrentControllerMap.keys().contains(a_name)) {
    return ViewControllerPtr();
  }

  return m_priv_impl->mCurrentControllerMap[a_name];
}

QStringList Space::current_controller_list() const
{
  return m_priv_impl->mCurrentControllerMap.keys();
}

void Space::set_name(const QString &a_name) { m_priv_impl->mName = a_name; }

QString Space::name() const { return m_priv_impl->mName; }

void Space::set_id(int a_id) { m_priv_impl->mID = a_id; }

int Space::id() const { return m_priv_impl->mID; }

void Space::setGeometry(const QRectF &a_geometry)
{
  m_priv_impl->mDesktopRect = a_geometry;

  foreach(DesktopActivityPtr _activity, m_priv_impl->mActivityList) {
    if (_activity && _activity->window()) {
      QPointF _activity_pos = _activity->window()->pos();

      if (_activity_pos.y() > a_geometry.height()) {
        _activity_pos.setY(_activity_pos.y() - a_geometry.height());
        _activity->window()->setPos(_activity_pos);
      }
    }
  }

  foreach(const QString & _key, m_priv_impl->mCurrentControllerMap.keys()) {
    ViewControllerPtr _controller_ptr = m_priv_impl->mCurrentControllerMap[_key];
    _controller_ptr->setViewRect(m_priv_impl->mDesktopRect);
  }
}

QObject *Space::workspace() { return m_priv_impl->mWorkSpace; }

void Space::set_workspace(WorkSpace *a_workspace_ptr) { m_priv_impl->mWorkSpace = a_workspace_ptr; }

void Space::restore_session() { m_priv_impl->initSessionStorage(this); }

void Space::set_qt_graphics_scene(QGraphicsScene *a_qt_graphics_scene_ptr)
{
  m_priv_impl->mMainScene = a_qt_graphics_scene_ptr;
}

QRectF Space::geometry() const { return m_priv_impl->mDesktopRect; }

void Space::drop_event_handler(QDropEvent *event,
                               const QPointF &local_event_location)
{
  if (m_priv_impl->mMainScene) {
    QList<QGraphicsItem *> items =
      m_priv_impl->mMainScene->items(local_event_location);

    Q_FOREACH(QGraphicsItem * item, items) {
      QGraphicsObject *itemObject = item->toGraphicsObject();

      if (!itemObject) {
        continue;
      }

      Widget *widget = qobject_cast<Widget *>(itemObject);

      if (!widget || !widget->controller()) {
        qDebug() << Q_FUNC_INFO << "Not a Valid Item";
        continue;
      }

      widget->controller()->handleDropEvent(widget, event);
      return;
    }
  }
}
}
