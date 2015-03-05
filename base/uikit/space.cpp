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
  std::list<UIKit::Widget *> mWindowList;
  QMap<QString, ViewControllerPtr> mCurrentControllerMap;
  QList<ViewControllerPtr> mControllerList;

  QList<NotifyFunc> m_notify_chain;
};

Space::Space(QObject *aParent) : QObject(aParent), mPrivImpl(new PrivateSpace)
{
  mPrivImpl->mWorkSpace = 0;
  mPrivImpl->mMainScene = 0;
}

Space::~Space()
{
  clear();
  delete mPrivImpl;
}

void Space::addController(const QString &aName)
{
  if (mPrivImpl->mCurrentControllerMap.keys().contains(aName)) {
    return;
  }

  QSharedPointer<ViewController> controllerPtr =
    (UIKit::ExtensionManager::instance()->controller(aName));

  if (!controllerPtr) {
    qWarning() << Q_FUNC_INFO << "Error loading extension" << aName;
    return;
  }

  mPrivImpl->mCurrentControllerMap[aName] = controllerPtr;

  controllerPtr->setViewport(this);
  controllerPtr->setControllerName(aName);

  controllerPtr->init();

  mPrivImpl->createActionsFromController(this, controllerPtr);

  registerController(aName);

  foreach(NotifyFunc l_notify_handler, mPrivImpl->m_notify_chain) {
      if (l_notify_handler)
          l_notify_handler(Space::kControllerAddedNotification,
                           QVariant(aName),
                           this);
  }
}

UIKit::DesktopActivityPtr Space::createActivity(const QString &aActivity,
    const QString &aTitle,
    const QPointF &aPos,
    const QRectF &aRect,
    const QVariantMap &aDataMap)
{
  UIKit::DesktopActivityPtr intent =
    UIKit::ExtensionManager::instance()->activity(aActivity);

  if (!intent) {
    qWarning() << Q_FUNC_INFO << "No such Activity: " << aActivity;
    return UIKit::DesktopActivityPtr();
  }

  intent->setActivityAttribute("data", QVariant(aDataMap));

  intent->createWindow(aRect, aTitle, QPointF(aPos.x(), aPos.y()));

  if (intent->window()) {
      intent->window()->setWindowTitle(aTitle);
      intent->window()->setWindowViewport(this);
  }

  addActivity(intent);

  return intent;
}

void Space::updateSessionValue(const QString &aName,
                               const QString &aKey, const QString &aValue)
{
  if (aKey.isEmpty()) {
    return;
  }

  QuetzalKit::DataStore *dataStore = new QuetzalKit::DataStore(
    mPrivImpl->sessionNameForController(aName), this);
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

    _session_object_ptr->setObjectAttribute(aKey, aValue);
    dataStore->insert(_session_object_ptr);
    delete dataStore;
  } else {
    Q_FOREACH(QuetzalKit::SyncObject * _child_session_object,
              _session_list_ptr->childObjects()) {
      if (!_child_session_object) {
        continue;
      }
      _child_session_object->setObjectAttribute(aKey, aValue);
      dataStore->updateNode(_child_session_object);
    }

    delete dataStore;
  }
}

void Space::addActivity(UIKit::DesktopActivityPtr aActivity)
{
  if (!aActivity) {
    return;
  }

  connect(aActivity.data(), SIGNAL(finished()), this,
          SLOT(onActivityFinished()));

  if (aActivity->window() && mPrivImpl->mMainScene) {
    if (mPrivImpl->mActivityList.contains(aActivity)) {
       qWarning() << Q_FUNC_INFO << "Space already contains the activity";
      return;
    }

    mPrivImpl->mActivityList << aActivity;

    aActivity->setViewport(this);

    insertWindowToView(aActivity->window());
  }
}

void Space::insertWindowToView(Window *aWindow)
{
  if (!aWindow) {
    return;
  }

  if (!mPrivImpl->mMainScene) {
    qWarning() << Q_FUNC_INFO << "Scene not Set";
    return;
  }

  QPointF _center_of_space_location = this->geometry().center();
  QPoint _widget_location;

  _widget_location.setX(_center_of_space_location.x() -
                        aWindow->boundingRect().width() / 2);
  _widget_location.setY(_center_of_space_location.y() -
                        aWindow->boundingRect().height() / 2);

  mPrivImpl->mMainScene->addItem(aWindow);
  aWindow->setPos(_widget_location);

  connect(aWindow, SIGNAL(closed(UIKit::Widget *)), this,
          SLOT(removeWindowFromView(UIKit::Widget *)));

  aWindow->show();

  if (aWindow->controller()) {
    aWindow->controller()->setViewRect(mPrivImpl->mDesktopRect);
  }

  mPrivImpl->mWindowList.push_front(aWindow);

  aWindow->setWindowCloseCallback([this](Window *window){
      qDebug() << Q_FUNC_INFO << "Request Window Removal from Space";
      this->removeWindowFromView(window);
  });
}

void Space::removeWindowFromView(Window *aWindow)
{
  if (!mPrivImpl->mMainScene) {
    return;
  }

  if (!aWindow) {
    return;
  }

  if (aWindow->controller()) {
    if (aWindow->controller()->removeWidget(aWindow)) {
      return;
    }
  }

  mPrivImpl->mMainScene->removeItem(aWindow);

  qDebug() << Q_FUNC_INFO << "Before :" << mPrivImpl->mWindowList.size();
  mPrivImpl->mWindowList.remove(aWindow);
  qDebug() << Q_FUNC_INFO << "After:" << mPrivImpl->mWindowList.size();

  if (aWindow) {
    delete aWindow;
  }
}

void Space::onViewportEventNotify(
        std::function<void (ViewportNotificationType,
                            const QVariant &,
                            const Space *)> aNotifyHandler)
{
    mPrivImpl->m_notify_chain.append(aNotifyHandler);
}

void Space::onActivityFinished()
{
  qDebug() << Q_FUNC_INFO;
  DesktopActivity *activity = qobject_cast<DesktopActivity *>(sender());

  if (activity) {

    if (activity->controller()) {
      activity->controller()->requestAction(
        activity->result()["action"].toString(), activity->result());
    }

    int i = 0;
    foreach(DesktopActivityPtr _activity, mPrivImpl->mActivityList) {
      if (_activity.data() == activity) {
        _activity.clear();
        qDebug() << Q_FUNC_INFO << "Before :" << mPrivImpl->mActivityList.count();
        mPrivImpl->mActivityList.removeAt(i);
        mPrivImpl->mActivityList.removeAll(_activity);
        qDebug() << Q_FUNC_INFO << "After :" << mPrivImpl->mActivityList.count();
      }
      i++;
    }
  }
}

void Space::registerController(const QString &aControllerName)
{
  qDebug() << Q_FUNC_INFO << "Controller :" << aControllerName;
  QuetzalKit::DataStore *_data_store =
    new QuetzalKit::DataStore(mPrivImpl->sessionNameForSpace(), this);
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
    mPrivImpl->sessionNameForController(aControllerName), this);
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
        space->addController(_current_controller_name);
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

QString Space::sessionName() const { return mPrivImpl->sessionNameForSpace(); }

void Space::clear()
{
  int i = 0;
  foreach(DesktopActivityPtr _activity, mPrivImpl->mActivityList) {
    qDebug() << Q_FUNC_INFO << "Remove Activity: ";
    if (_activity) {
      Widget *_widget = _activity->window();
      if (_widget) {
        if (mPrivImpl->mMainScene->items().contains(_widget)) {
          mPrivImpl->mMainScene->removeItem(_widget);
          delete _widget;
        }
      }

      mPrivImpl->mActivityList.removeAt(i);
    }
    i++;
  }

  // delete owner widgets
  for (Widget *_widget : mPrivImpl->mWindowList) {
    if (_widget) {
      if (mPrivImpl->mMainScene->items().contains(_widget)) {
        mPrivImpl->mMainScene->removeItem(_widget);
      }
      delete _widget;
      qDebug() << Q_FUNC_INFO << "Widget Deleted: OK";
    }
  }

  mPrivImpl->mWindowList.clear();
  // remove spaces which belongs to the space.
  foreach(const QString & _key, mPrivImpl->mCurrentControllerMap.keys()) {
    qDebug() << Q_FUNC_INFO << _key;
    ViewControllerPtr _controller_ptr = mPrivImpl->mCurrentControllerMap[_key];
    _controller_ptr->prepareRemoval();
    qDebug() << Q_FUNC_INFO
             << "Before Removal:" << mPrivImpl->mCurrentControllerMap.count();
    mPrivImpl->mCurrentControllerMap.remove(_key);
    qDebug() << Q_FUNC_INFO
             << "After Removal:" << mPrivImpl->mCurrentControllerMap.count();
  }

  mPrivImpl->mCurrentControllerMap.clear();
}

QPointF Space::mousePointerPos() const
{
  if (!mPrivImpl->mWorkSpace) {
    return QCursor::pos();
  }

  QGraphicsView *_view_parent = qobject_cast<QGraphicsView *>(mPrivImpl->mWorkSpace);

  if (!_view_parent) {
    return QCursor::pos();
  }

  return _view_parent->mapToScene(QCursor::pos());
}

QPointF Space::center(const QRectF &aViewGeometry,
                      const ViewportLocation &aLocation) const
{
  QPointF _rv;
  float _x_location;
  float _y_location;

  switch (aLocation) {
  case kCenterOnViewport:
    _y_location = (geometry().height() / 2) - (aViewGeometry.height() / 2);
    _x_location = (geometry().width() / 2) - (aViewGeometry.width() / 2);
    break;
  case kCenterOnViewportLeft:
    _y_location = (geometry().height() / 2) - (aViewGeometry.height() / 2);
    _x_location = (geometry().topLeft().x());
    break;
  case kCenterOnViewportRight:
    _y_location = (geometry().height() / 2) - (aViewGeometry.height() / 2);
    _x_location = (geometry().width() - (aViewGeometry.width()));
    break;
  case kCenterOnViewportTop:
    _y_location = (0.0);
    _x_location = ((geometry().width() / 2) - (aViewGeometry.width() / 2));
    break;
  case kCenterOnViewportBottom:
    _y_location = (geometry().height() - (aViewGeometry.height()));
    _x_location = ((geometry().width() / 2) - (aViewGeometry.width() / 2));
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Error : Unknown Viewprot Location Type:";
  }

  _rv.setY(geometry().y() + _y_location);
  _rv.setX(_x_location);

  return _rv;

}

ViewControllerPtr Space::controller(const QString &aName)
{
  if (!mPrivImpl->mCurrentControllerMap.keys().contains(aName)) {
    return ViewControllerPtr();
  }

  return mPrivImpl->mCurrentControllerMap[aName];
}

QStringList Space::currentControllerList() const
{
  return mPrivImpl->mCurrentControllerMap.keys();
}

void Space::setName(const QString &aName) { mPrivImpl->mName = aName; }

QString Space::name() const { return mPrivImpl->mName; }

void Space::setId(int aId) { mPrivImpl->mID = aId; }

int Space::id() const { return mPrivImpl->mID; }

void Space::setGeometry(const QRectF &aRealRect)
{
  mPrivImpl->mDesktopRect = aRealRect;

  foreach(DesktopActivityPtr _activity, mPrivImpl->mActivityList) {
    if (_activity && _activity->window()) {
      QPointF _activity_pos = _activity->window()->pos();

      if (_activity_pos.y() > aRealRect.height()) {
        _activity_pos.setY(_activity_pos.y() - aRealRect.height());
        _activity->window()->setPos(_activity_pos);
      }
    }
  }

  foreach(const QString & _key, mPrivImpl->mCurrentControllerMap.keys()) {
    ViewControllerPtr _controller_ptr = mPrivImpl->mCurrentControllerMap[_key];
    _controller_ptr->setViewRect(mPrivImpl->mDesktopRect);
  }
}

QObject *Space::workspace() { return mPrivImpl->mWorkSpace; }

void Space::setWorkspace(WorkSpace *aWorkspace) { mPrivImpl->mWorkSpace = aWorkspace; }

void Space::restoreSession() { mPrivImpl->initSessionStorage(this); }

void Space::setScene(QGraphicsScene *aScenePtr)
{
  mPrivImpl->mMainScene = aScenePtr;
}

QRectF Space::geometry() const { return mPrivImpl->mDesktopRect; }

void Space::handleDropEvent(QDropEvent *event,
                            const QPointF &local_event_location)
{
  if (mPrivImpl->mMainScene) {
    QList<QGraphicsItem *> items =
            mPrivImpl->mMainScene->items(local_event_location);

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
