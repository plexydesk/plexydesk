#include "datasyncruntime.h"
#include <QDebug>
#include <QImage>
#include <QApplication>

#include <syncobject.h>
#include <memorysyncengine.h>
#include <datastore.h>
#include <disksyncengine.h>

class DataSyncRuntime::PrivateDataSyncRuntime {
public:
  PrivateDataSyncRuntime() {}
  ~PrivateDataSyncRuntime() {}

  void printStep(const QString &message, uint step);
};

DataSyncRuntime::DataSyncRuntime(QObject *parent)
    : d(new PrivateDataSyncRuntime), QObject(parent) {
  qDebug() << Q_FUNC_INFO << "Runner Started";

  // testDiskEngine();
  // this->runDiskTest();
  // mergeTest();
  insertTest();
  updateTest();
}

DataSyncRuntime::~DataSyncRuntime() {
  qDebug() << Q_FUNC_INFO << "Runner Ended";
  delete d;
}

void DataSyncRuntime::testCreateDataStore() {
  QuetzalKit::DataStore *store = new QuetzalKit::DataStore("wallpapers", this);

  QuetzalKit::MemorySyncEngine *memoryEngine =
      new QuetzalKit::MemorySyncEngine(this);
  store->setSyncEngine(memoryEngine);

  QuetzalKit::SyncObject object;

  object.setName("wallpaper");
  object.setKey(10);

  object.setObjectAttribute("src", "http://www.google.com");
  // child object
  QuetzalKit::SyncObject childObject;

  childObject.setName("size");
  childObject.setObjectAttribute("width", 1920);
  childObject.setObjectAttribute("height", 1920);

  // meta object
  QuetzalKit::SyncObject metaData;

  metaData.setName("metadata");
  metaData.setKey(100);
  metaData.setObjectAttribute("compression", "PNG");
  metaData.setObjectAttribute("location", "Colomobo, Sri Lanka");

  // loop add many
  QList<QuetzalKit::SyncObject *> list;

  for (int i = 0; i < 10; i++) {
    QuetzalKit::SyncObject *manyObject = new QuetzalKit::SyncObject(this);

    manyObject->setName("colors");
    manyObject->setObjectAttribute("id", i);
    metaData.addChildObject(manyObject);
    list.append(manyObject);
  }

  childObject.addChildObject(&metaData);
  object.addChildObject(&childObject);

  store->addObject(&object);

  // return value test
  QuetzalKit::SyncObject *rootObject = store->rootObject();
  qDebug() << Q_FUNC_INFO << "Root:" << rootObject->name();

  Q_ASSERT(rootObject->hasChildren());

  Q_FOREACH(QuetzalKit::SyncObject * _objects, rootObject->childObjects()) {
    Q_ASSERT(_objects->hasChildren());
    qDebug() << Q_FUNC_INFO << "Child:" << _objects->name();
    Q_FOREACH(QuetzalKit::SyncObject * _child, _objects->childObjects()) {
      qDebug() << Q_FUNC_INFO << "Grand Child:" << _child->name();
      Q_ASSERT(_child->hasChildren());
      Q_FOREACH(QuetzalKit::SyncObject * _ggchild, _child->childObjects()) {
        qDebug() << Q_FUNC_INFO << "Great Grand Child:" << _ggchild->name()
                 << "(" << _ggchild->key() << ")";
        Q_ASSERT(_ggchild->hasChildren());
        Q_ASSERT(_ggchild->childObjects().count() == 10);
        Q_FOREACH(QuetzalKit::SyncObject * _gggchild,
                  _ggchild->childObjects()) {
          qDebug() << Q_FUNC_INFO
                   << "Great Great Grand Child:" << _gggchild->name();
          qDebug() << Q_FUNC_INFO
                   << "Great Great Grand Child:" << _gggchild->timeStamp();
          Q_ASSERT(!_gggchild->hasChildren());
          // qDebug() << Q_FUNC_INFO << "Has Children: " <<
          // _gggchild->hasChildren();
        }
      }
    }
  }

  // update test
  object.setObjectAttribute("src", "http://www.bing.com");

  childObject.setObjectAttribute("size", "800x600");

  metaData.setObjectAttribute("compression", "JPEG");

  store->update();

  delete store;

  qApp->quit();
}

void DataSyncRuntime::testDiskEngine() {
  QuetzalKit::DataStore *store = new QuetzalKit::DataStore("NoteDB", this);

  QuetzalKit::DiskSyncEngine *memoryEngine =
      new QuetzalKit::DiskSyncEngine(this);
  store->setSyncEngine(memoryEngine);

  // return a valid noteList with a name "NoteList" and key auto;
  QuetzalKit::SyncObject *noteListPtr = store->begin("NoteList");

  if (noteListPtr->key() != 0) {
    qFatal("Invalid ID");
  } else {
    qDebug() << Q_FUNC_INFO << "Object Key NoteList: " << noteListPtr->key();
  }

  if (!noteListPtr) {
    qFatal("Missing Object"); // << Q_FUNC_INFO << "Miss match";
  }

  if (noteListPtr->childObjects().count() <= 1) {
    // qFatal("[FAIL] => No Objects Found");
  }

  qDebug() << Q_FUNC_INFO << "Step 01: Object Count Of NoteList ->"
           << noteListPtr->childObjects().count();

  for (int i = 0; i < 10; i++) {
    QuetzalKit::SyncObject *note = noteListPtr->childObject(i);
    qDebug() << Q_FUNC_INFO << note;

    if (!note) {
      qDebug() << Q_FUNC_INFO << "Null Object Found";
      note = new QuetzalKit::SyncObject(store);
      note->setName(QString("Leaf-1").arg(i));

      /*
      QuetzalKit::SyncObject *metaData = new QuetzalKit::SyncObject(note);
      metaData->setName("metadata");
      metaData->setKey(100);

      note->addChildObject(metaData);
      */

      qDebug() << Q_FUNC_INFO
               << "SubTree Pre Insert:" << noteListPtr->childCount();
      noteListPtr->addChildObject(note);
      qDebug() << Q_FUNC_INFO
               << "SubTreePost Insert:" << noteListPtr->childCount();
    } else {
      /*
      qDebug() << Q_FUNC_INFO << "Found Object: " << note->key();
      QuetzalKit::SyncObject *metaData = note->childObject(0);

      if (!metaData) {
          metaData = new QuetzalKit::SyncObject(note);
          metaData->setName("metadata");
          metaData->setKey(100);

          note->addChildObject(metaData);
          store->updateObject(*note);
      }
      */
    }
  }

  qDebug() << Q_FUNC_INFO
           << "ChildCount NoteList:" << store->childCount(*noteListPtr);

  store->update();

  qFatal("Done");
  qApp->quit();
}

void DataSyncRuntime::testDataSyncObject() {
  QuetzalKit::SyncObject *object = new QuetzalKit::SyncObject(this);
  delete object;
}

void DataSyncRuntime::addTestNoteObejct(const QString &title,
                                        const QString &content) {
  QuetzalKit::DataStore *store = new QuetzalKit::DataStore("NoteDB", this);

  QuetzalKit::DiskSyncEngine *memoryEngine =
      new QuetzalKit::DiskSyncEngine(this);
  store->setSyncEngine(memoryEngine);

  QuetzalKit::SyncObject *noteListPtr = store->begin("NoteList");
  QuetzalKit::SyncObject *object = noteListPtr->createNewObject("Note");
  // object->setObjectAttribute("title", title);
  // object->attachTextNode("This is a test Note, from the test case.");
  // QuetzalKit::SyncObject *metadata = object->createNewObject("metadata");

  store->update();
}

void DataSyncRuntime::runDiskTest() {
  QMap<uint, QString> testData;

  testData[0] = "Milk";
  testData[1] = "Tea";
  testData[2] = "Orange";
  testData[3] = "Apples";
  testData[5] = "Laptop";

  Q_FOREACH(const QString & title, testData.values()) {
    qDebug() << Q_FUNC_INFO << "Adding : " << title;
    addTestNoteObejct(title);
  }

  qFatal("Done");
}

void DataSyncRuntime::mergeTest() {
  QuetzalKit::DataStore *store_x = new QuetzalKit::DataStore("NoteDB", this);
  QuetzalKit::DataStore *store_y = new QuetzalKit::DataStore("NoteDB", this);

  QuetzalKit::DiskSyncEngine *engine_x = new QuetzalKit::DiskSyncEngine(this);
  store_x->setSyncEngine(engine_x);

  QuetzalKit::DiskSyncEngine *engine_y = new QuetzalKit::DiskSyncEngine(this);
  store_y->setSyncEngine(engine_y);

  QuetzalKit::SyncObject *noteList_x = store_x->begin("NoteList");

  for (int i = 0; i < 1000000; i++) {
  };

  QuetzalKit::SyncObject *noteList_y = store_y->begin("NoteList");

  if (!noteList_x || !noteList_y) {
    qFatal("Root Node Error");
  }

  // check
  qDebug() << Q_FUNC_INFO
           << "X Note List Child Count: " << noteList_x->childCount();
  qDebug() << Q_FUNC_INFO
           << "Y Note List Child Count: " << noteList_y->childCount();

  //
  noteList_x->setDataStore(store_x);
  noteList_y->setDataStore(store_y);

  connect(store_x, SIGNAL(updated(QuetzalKit::SyncObject *)), this,
          SLOT(onStoreUpdated(QuetzalKit::SyncObject *)));

  QuetzalKit::SyncObject *note_x = noteList_x->createNewObject("Note");
  note_x->setObjectAttribute("title", "Object X");
  note_x->attachTextNode("This is a text Node Attachment for X");

  QuetzalKit::SyncObject *note_y = noteList_y->createNewObject("Note");
  note_y->setObjectAttribute("title", "Object Y");
  note_y->attachTextNode("This is a text Node Attachment for Y");

  // note_y->dumpContent();

  qDebug() << Q_FUNC_INFO << "Begin Updating X Object ======";
  store_x->update();
  qDebug() << Q_FUNC_INFO << "End Updating X Object =========";

  return;

  // qDebug() << Q_FUNC_INFO << "------------";
  // note_y->dumpContent();

  qDebug() << Q_FUNC_INFO << "Begin Updating Y Object ======";
  store_y->update();
  qDebug() << Q_FUNC_INFO << "End Updating Y Object =========";

  if (noteList_y->childCount() <= 1) {
    qDebug() << Q_FUNC_INFO << noteList_y->childCount();
    qFatal("It's fucked");
  } else {
    qDebug() << Q_FUNC_INFO << noteList_y->childCount();
  }

  //
  if (noteList_x->childCount() <= 1) {
    qDebug() << Q_FUNC_INFO << noteList_x->childCount();
    noteList_x->dumpContent();
    // qFatal("It's fucked");
  } else {
    qDebug() << Q_FUNC_INFO << noteList_x->childCount();
    // qFatal("DOoM");
  }

  // check increamental Updates
  QString data;
  for (int i = 0; i < 1; i++) {
    data += QString(" updated message : %1").arg(i);
    // qDebug() << Q_FUNC_INFO << data;
    // note_x->attachTextNode(data);
    // store_x->update();
  }
}

void DataSyncRuntime::insertTest() {
  QuetzalKit::DataStore *store = new QuetzalKit::DataStore("NoteDB", this);
  QuetzalKit::DiskSyncEngine *diskEngine = new QuetzalKit::DiskSyncEngine(this);

  store->setSyncEngine(diskEngine);

  QuetzalKit::SyncObject *noteList_x = store->begin("NoteList");

  QuetzalKit::SyncObject *object = new QuetzalKit::SyncObject(this);
  object->setName("Note");
  store->insert(object);

  object->setObjectAttribute("title", "Milk");
  object->setTextData("This is a test Note");
  store->updateNode(object);

  delete store;
}

void DataSyncRuntime::updateTest() {
  QuetzalKit::DataStore *store = new QuetzalKit::DataStore("NoteDB", this);
  QuetzalKit::DiskSyncEngine *diskEngine = new QuetzalKit::DiskSyncEngine(this);

  store->setSyncEngine(diskEngine);

  QuetzalKit::SyncObject *noteList_x = store->begin("NoteList");
  QuetzalKit::SyncObject *object = noteList_x->createNewObject("Note");

  store->updateNode(object);

  object->setObjectAttribute("title", "Tea");
  object->setTextData("This is a test Note Update");
  object->setKey(0);
  store->updateNode(object);

  delete store;
}

void DataSyncRuntime::onStoreUpdated(QuetzalKit::SyncObject *object) {
  // todo:write tests to check store updates.
}

void DataSyncRuntime::PrivateDataSyncRuntime::printStep(const QString &message,
                                                        uint step) {
  qDebug() << Q_FUNC_INFO << message << ": " << step;
}
