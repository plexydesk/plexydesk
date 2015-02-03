#include "space.h"

#include <QAction>
#include <QDebug>
#include <QDropEvent>
#include <QGraphicsItem>
#include <datastore.h>
#include <disksyncengine.h>
#include <extensionmanager.h>
#include <syncobject.h>
#include <desktopactivitymenu.h>
#include <QGraphicsScene>
#include <QWeakPointer>
#include <desktopwidget.h>

#include "workspace.h"

namespace UI {

typedef QWeakPointer<DesktopActivityMenu> ActivityPoupWeekPtr;

class Space::PrivateSpace {
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
  QRectF m_desktop_rect;
  WorkSpace *mWorkSpace;
  QGraphicsScene *m_main_scene;
  QList<UI::DesktopActivityPtr> m_activity_list;
  std::list<UI::UIWidget *> m_window_list;
  QMap<QString, ViewControllerPtr> m_current_controller_map;
  QList<ActivityPoupWeekPtr> m_activity_popup_list;
  QList<ViewControllerPtr> m_controller_list;
};

Space::Space(QObject *parent) : QObject(parent), d(new PrivateSpace) {
  d->mWorkSpace = 0;
  d->m_main_scene = 0;
}

Space::~Space() {
  clear();
  delete d;
}

void Space::addController(const QString &aName) {

  if (d->m_current_controller_map.keys().contains(aName))
    return;

  QSharedPointer<ViewController> controller_ptr =
      (UI::ExtensionManager::instance()->controller(aName));

  if (!controller_ptr) {
    qWarning() << Q_FUNC_INFO << "Error loading extension" << aName;
    return;
  }

  d->m_current_controller_map[aName] = controller_ptr;

  controller_ptr->setViewport(this);
  controller_ptr->setControllerName(aName);

  controller_ptr->init();

  d->createActionsFromController(this, controller_ptr);

  registerController(aName);

  Q_EMIT controllerAdded(aName);
}

UI::DesktopActivityPtr Space::createActivity(const QString &activity,
                                         const QString &title,
                                         const QPointF &pos, const QRectF &rect,
                                         const QVariantMap &dataItem) {
  UI::DesktopActivityPtr intent =
      UI::ExtensionManager::instance()->activity(activity);

  if (!intent) {
    qWarning() << Q_FUNC_INFO << "No such Activity: " << activity;
    return UI::DesktopActivityPtr();
  }

  intent->setActivityAttribute("data", QVariant(dataItem));

  intent->createWindow(rect, title, QPointF(pos.x(), pos.y()));

  addActivity(intent);

  /// d->m_activity_map[activity] = intent;
  d->m_activity_list << intent;

  return intent;
}

void Space::updateSessionValue(const QString &aName,
                               const QString &aKey, const QString &aValue) {
  if (aKey.isEmpty())
    return;

  QuetzalKit::DataStore *_data_store = new QuetzalKit::DataStore(
      d->sessionNameForController(aName), this);
  QuetzalKit::DiskSyncEngine *engine =
      new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(engine);

  QuetzalKit::SyncObject *_session_list_ptr = _data_store->begin("SessionData");

  if (!_session_list_ptr) {
    return;
  }

  if (!_session_list_ptr->hasChildren()) {
    QuetzalKit::SyncObject *_session_object_ptr =
        _session_list_ptr->createNewObject("Session");

    _session_object_ptr->setObjectAttribute(aKey, aValue);
    _data_store->insert(_session_object_ptr);
    delete _data_store;
  } else {
    Q_FOREACH(QuetzalKit::SyncObject * _child_session_object,
              _session_list_ptr->childObjects()) {
      if (!_child_session_object)
        continue;
      _child_session_object->setObjectAttribute(aKey, aValue);
      _data_store->updateNode(_child_session_object);
    }

    delete _data_store;
  }
}

void Space::addActivity(UI::DesktopActivityPtr activity) {
  if (!activity)
    return;

  connect(activity.data(), SIGNAL(finished()), this,
          SLOT(onActivityFinished()));

  if (activity->window() && d->m_main_scene) {
    if (d->m_activity_list.contains(activity)) {
      // qWarning() << Q_FUNC_INFO << "Space already contains the activity";
      return;
    }
    qDebug() << Q_FUNC_INFO << "Before Adding: " << d->m_activity_list.count();
    d->m_activity_list << activity;

    qDebug() << Q_FUNC_INFO << "After Adding: " << d->m_activity_list.count();
    activity->setViewport(this);
    d->m_main_scene->addItem(activity->window());
  }
}

void Space::addActivityPoupToView(QSharedPointer<DesktopActivityMenu> menu) {
  d->m_activity_popup_list.append(menu.toWeakRef());
}

void Space::addWidgetToView(UIWidget *widget) {
  if (!widget)
    return;

  if (!d->m_main_scene) {
    qWarning() << Q_FUNC_INFO << "Scene not Set";
    return;
  }

  QPointF _center_of_space_location = this->geometry().center();
  QPoint _widget_location;

  _widget_location.setX(_center_of_space_location.x() -
                        widget->boundingRect().width() / 2);
  _widget_location.setY(_center_of_space_location.y() -
                        widget->boundingRect().height() / 2);

  d->m_main_scene->addItem(widget);
  widget->setPos(_widget_location);

  connect(widget, SIGNAL(closed(UI::UIWidget *)), this,
          SLOT(onWidgetClosed(UI::UIWidget *)));

  widget->show();

  if (widget->controller()) {
    widget->controller()->setViewRect(d->m_desktop_rect);
  }

  d->m_window_list.push_front(widget);
}

void Space::onWidgetClosed(UIWidget *widget) {
  qDebug() << Q_FUNC_INFO << __LINE__;
  if (!d->m_main_scene)
    return;

  if (!widget)
    return;

  if (widget->controller()) {
    if (widget->controller()->removeWidget(widget))
      return;
  }

  d->m_main_scene->removeItem(widget);

  qDebug() << Q_FUNC_INFO << "Before :" << d->m_window_list.size();
  d->m_window_list.remove(widget);
  qDebug() << Q_FUNC_INFO << "After:" << d->m_window_list.size();

  if (widget)
    delete widget;
}

void Space::onActivityFinished() {
  qDebug() << Q_FUNC_INFO;
  DesktopActivity *activity = qobject_cast<DesktopActivity *>(sender());

  if (activity) {

    if (activity->controller()) {
      activity->controller()->requestAction(
          activity->result()["action"].toString(), activity->result());
    }

    int i = 0;
    foreach(DesktopActivityPtr _activity, d->m_activity_list) {
      if (_activity.data() == activity) {
        _activity.clear();
        qDebug() << Q_FUNC_INFO << "Before :" << d->m_activity_list.count();
        d->m_activity_list.removeAt(i);
        d->m_activity_list.removeAll(_activity);
        qDebug() << Q_FUNC_INFO << "After :" << d->m_activity_list.count();
      }
      i++;
    }
  }
}

void Space::registerController(const QString &controllerName) {
  qDebug() << Q_FUNC_INFO << "Controller :" << controllerName;
  QuetzalKit::DataStore *_data_store =
      new QuetzalKit::DataStore(d->sessionNameForSpace(), this);
  QuetzalKit::DiskSyncEngine *_engine =
      new QuetzalKit::DiskSyncEngine(_data_store);

  _data_store->setSyncEngine(_engine);

  QuetzalKit::SyncObject *_session_list_ptr =
      _data_store->begin("ControllerList");

  bool _has_controller = false;

  Q_FOREACH(QuetzalKit::SyncObject * _child_object_ptr,
            _session_list_ptr->childObjects()) {
    if (!_child_object_ptr)
      continue;

    if (_child_object_ptr->attributeValue("name").toString() == controllerName)
      _has_controller = true;
  }

  if (!_has_controller) {
    QuetzalKit::SyncObject *_controller_ptr =
        _session_list_ptr->createNewObject("Controller");

    _controller_ptr->setObjectAttribute("name", controllerName);
    _data_store->insert(_controller_ptr);
  }

  delete _data_store;

  // create a new data  Store to link it to this store.
  QuetzalKit::DataStore *_linked_sub_store = new QuetzalKit::DataStore(
      d->sessionNameForController(controllerName), this);
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
    if (!_session_ptr)
      continue;

    Q_FOREACH(const QString & attribute, _session_ptr->attributes()) {
      _session_data[attribute] =
          _session_ptr->attributeValue(attribute).toString();
    }
  }

  if (controller(controllerName))
    controller(controllerName)->revokeSession(_session_data);

  delete _linked_sub_store;
}

void Space::PrivateSpace::createActionsFromController(const Space *space,
                                                      ViewControllerPtr ptr) {
  Q_FOREACH(const QAction * action, ptr->actions()) {
    qDebug() << action->text();
    qDebug() << action->icon();
  }
}

void Space::PrivateSpace::initSessionStorage(Space *space) {
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
      if (!_object)
        continue;

      QString _current_controller_name =
          _object->attributeValue("name").toString();

      ViewControllerPtr _controller_ptr =
          space->controller(_current_controller_name);

      if (!_controller_ptr) {
        space->addController(_current_controller_name);
        continue;
      }
    }
  }

  delete _data_store;
}

Space::PrivateSpace::~PrivateSpace() { m_current_controller_map.clear(); }

QString Space::PrivateSpace::sessionNameForSpace() {
  return QString("%1_Space_%2").arg(mName).arg(mID);
}

QString Space::PrivateSpace::sessionNameForController(
    const QString &controllerName) {
  return QString("%1_Controller_%2").arg(sessionNameForSpace()).arg(
      controllerName);
}

QString Space::sessionName() const { return d->sessionNameForSpace(); }

void Space::clear() {
  int i = 0;
  foreach(DesktopActivityPtr _activity, d->m_activity_list) {
    qDebug() << Q_FUNC_INFO << "Remove Activity: ";
    if (_activity) {
      UIWidget *_widget = _activity->window();
      if (_widget) {
        if (d->m_main_scene->items().contains(_widget)) {
          d->m_main_scene->removeItem(_widget);
          delete _widget;
        }
      }

      d->m_activity_list.removeAt(i);
    }
    i++;
  }

  // delete owner widgets
  for (UIWidget *_widget : d->m_window_list) {
    if (_widget) {
      if (d->m_main_scene->items().contains(_widget)) {
        d->m_main_scene->removeItem(_widget);
      }
      delete _widget;
      qDebug() << Q_FUNC_INFO << "Widget Deleted: OK";
    }
  }

  d->m_window_list.clear();
  // remove spaces which belongs to the space.
  foreach(const QString & _key, d->m_current_controller_map.keys()) {
    qDebug() << Q_FUNC_INFO << _key;
    ViewControllerPtr _controller_ptr = d->m_current_controller_map[_key];
    _controller_ptr->prepareRemoval();
    qDebug() << Q_FUNC_INFO
             << "Before Removal:" << d->m_current_controller_map.count();
    d->m_current_controller_map.remove(_key);
    qDebug() << Q_FUNC_INFO
             << "After Removal:" << d->m_current_controller_map.count();
  }

  d->m_current_controller_map.clear();
}

void Space::dismissActivityPopup() {
  Q_FOREACH(ActivityPoupWeekPtr popup, d->m_activity_popup_list) {
    QSharedPointer<DesktopActivityMenu> _strong_ptr = popup.toStrongRef();
    if (_strong_ptr) {
      _strong_ptr->hide();
    } else {
      // activity is deleted so we remove from the list
      qDebug() << Q_FUNC_INFO << "Error no ref";
      d->m_activity_popup_list.removeAll(popup);
    }
  }
}

QPointF Space::mousePointerPos() const {
  if (!d->mWorkSpace)
    return QCursor::pos();

  QGraphicsView *_view_parent = qobject_cast<QGraphicsView *>(d->mWorkSpace);

  if (!_view_parent)
    return QCursor::pos();

  return _view_parent->mapToScene(QCursor::pos());
}

QPointF Space::center(const QRectF &viewGeometry,
                      const ViewportLocation &loc) const
{
  QPointF _rv;
  float _x_location;
  float _y_location;

  switch (loc) {
    case kCenterOnViewport:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().width() / 2) - (viewGeometry.width() / 2);
      break;
    case kCenterOnViewportLeft:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().topLeft().x());
      break;
    case kCenterOnViewportRight:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().width() - (viewGeometry.width()));
      break;
    case kCenterOnViewportTop:
      _y_location = (0.0);
      _x_location = ((geometry().width() / 2) - (viewGeometry.width() / 2));
      break;
    case kCenterOnViewportBottom:
      _y_location = (geometry().height() - (viewGeometry.height()));
      _x_location = ((geometry().width() / 2) - (viewGeometry.width() / 2));
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Error : Unknown Viewprot Location Type:";
  }

  _rv.setY(geometry().y() + _y_location);
  _rv.setX(_x_location);

  return _rv;

}

ViewControllerPtr Space::controller(const QString &aName) {
  if (!d->m_current_controller_map.keys().contains(aName))
    return ViewControllerPtr();

  return d->m_current_controller_map[aName];
}

QStringList Space::getSessionControllers() const {
  return d->m_current_controller_map.keys();
}

void Space::setName(const QString &aName) { d->mName = aName; }

QString Space::name() const { return d->mName; }

void Space::setId(int aId) { d->mID = aId; }

int Space::id() const { return d->mID; }

void Space::setGeometry(const QRectF &aRealRect) {
  d->m_desktop_rect = aRealRect;

  foreach(DesktopActivityPtr _activity, d->m_activity_list) {
    if (_activity && _activity->window()) {
      QPointF _activity_pos = _activity->window()->pos();

      if (_activity_pos.y() > aRealRect.height()) {
        _activity_pos.setY(_activity_pos.y() - aRealRect.height());
        _activity->window()->setPos(_activity_pos);
      }
    }
  }

  foreach(const QString & _key, d->m_current_controller_map.keys()) {
    ViewControllerPtr _controller_ptr = d->m_current_controller_map[_key];
    _controller_ptr->setViewRect(d->m_desktop_rect);
  }
}

QObject *Space::workspace() { return d->mWorkSpace; }

void Space::setWorkspace(WorkSpace *aWorkspace) { d->mWorkSpace = aWorkspace; }

void Space::restoreSession() { d->initSessionStorage(this); }

void Space::setScene(QGraphicsScene *aScenePtr) {
  d->m_main_scene = aScenePtr;
}

QRectF Space::geometry() const { return d->m_desktop_rect; }

void Space::handleDropEvent(QDropEvent *event,
                            const QPointF &local_event_location) {
  if (d->m_main_scene) {
    QList<QGraphicsItem *> items = d->m_main_scene->items(local_event_location);

    Q_FOREACH(QGraphicsItem * item, items) {
      QGraphicsObject *itemObject = item->toGraphicsObject();

      if (!itemObject) {
        continue;
      }

      UIWidget *widget = qobject_cast<UIWidget *>(itemObject);

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
