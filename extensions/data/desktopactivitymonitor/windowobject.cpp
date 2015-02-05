#include "windowobject.h"
#include <QSharedData>
#include <QVariant>

#include <datastore.h>
#include <syncobject.h>
#include <disksyncengine.h>

class WindowObjectData : public QSharedData
{
public:
  QString name;
  QString title;
  QTime time;
  int duration;
  QuetzalKit::DataStore *mStore;
  QuetzalKit::SyncObject *mTaskObject;
};

WindowObject::WindowObject() : data(new WindowObjectData)
{
  data->duration = 0;
}

WindowObject::WindowObject(const WindowObject &rhs) : data(rhs.data) {}

WindowObject &WindowObject::operator=(const WindowObject &rhs)
{
  if (this != &rhs) {
    data.operator = (rhs.data);
  }
  return *this;
}

WindowObject::~WindowObject() {}

QString WindowObject::name() const { return data->name; }

void WindowObject::setName(const QString &name) { data->name = name; }

QString WindowObject::title() const { return data->title; }

void WindowObject::setTitle(const QString &title) { data->title = title; }

QTime WindowObject::time() const { return data->time; }

void WindowObject::setTime(const QTime &time) { data->time = time; }

int WindowObject::duration() const { return data->duration; }

void WindowObject::updateDuration(int duration)
{
  data->duration += duration;

  data->mStore = new QuetzalKit::DataStore("DesktopActivity");
  QuetzalKit::DiskSyncEngine *engine =
    new QuetzalKit::DiskSyncEngine(data->mStore);
  data->mStore->setSyncEngine(engine);

  QuetzalKit::SyncObject *rootObject = data->mStore->begin("WindowList");

  data->mTaskObject = rootObject->createNewObject("Task");
  data->mTaskObject->setObjectAttribute("app", QVariant(""));

  // save the data.
  data->mTaskObject->setObjectAttribute("app", data->name);
  data->mTaskObject->setObjectAttribute("task", data->title);
  data->mTaskObject->setObjectAttribute("start", data->time);
  data->mTaskObject->setObjectAttribute("end", QTime::currentTime());

  data->mStore->insert(data->mTaskObject);

  delete data->mStore;
}
