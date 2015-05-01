#include <QApplication>
#include <QDebug>

#include "datasyncruntime.h"

#include <datastore.h>
#include <syncobject.h>
#include <disksyncengine.h>
#include <datasync.h>

#include <iostream>

#define QZ_ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } else { std::cout <<  __LINE__ << " " << __func__ << "  " \
                           << #condition << "  [PASS]" << std::endl; } \
    } while (false)

using namespace QuetzalKit;

void test_object_create()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  QuetzalKit::SyncObject *obj = new QuetzalKit::SyncObject();
  obj->setName("clock");

  obj->setObjectAttribute("zone_id", "America/North");
  obj->setObjectAttribute("id", 2);

  sync->save_object(*obj);
  delete sync;
}

void test_object_update()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  QuetzalKit::SyncObject *obj = new QuetzalKit::SyncObject();
  obj->setName("clock");

  obj->setObjectAttribute("zone_id", "Asia/South");
  obj->setObjectAttribute("id", 1);

  sync->save_object(*obj);
  delete sync;
}

void test_object_find()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](const QuetzalKit::SyncObject &a_object,
                        const std::string &a_app_name, bool a_found){

      QZ_ASSERT(a_found == 1, "Expected True");
      QZ_ASSERT(a_app_name.compare("Clock") == 0, "Expected Clock");
      QZ_ASSERT(a_object.name().compare("clock") == 0, "Expected clock");
      QZ_ASSERT(a_object.attributes().count() == 2, "Expected 2 got :"
                << a_object.attributes().count() );
  });

  sync->find("clock", "zone_id", "Asia/South");

  delete sync;
}

void test_object_find_fail()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](const QuetzalKit::SyncObject &a_object,
                        const std::string &a_app_name, bool a_found){

      QZ_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("dock", "home", "alone");

  delete sync;
}


void test_object_delete()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock");

  delete sync;
}

void test_connected_objects()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("NoteList");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  QuetzalKit::SyncObject *obj = new QuetzalKit::SyncObject();
  obj->setName("note");

  delete sync;
}

void test_object_add_child()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("Clock");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  QuetzalKit::SyncObject *obj = new QuetzalKit::SyncObject();
  obj->setName("clock");

  obj->setObjectAttribute("zone_id", "America/North");
  obj->setObjectAttribute("id", 1);

  sync->add_object(*obj);
  delete sync;
}

void test_save_controller_to_session(const QString &a_controller_name)
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("test_default_space_0");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](const QuetzalKit::SyncObject &a_object,
                        const std::string &a_app_name, bool a_found){
      if (!a_found) {
        QuetzalKit::SyncObject obj;
        obj.setName("Controller");
        obj.setObjectAttribute("name", a_controller_name);

        sync->add_object(obj);
        QZ_ASSERT(a_found == 0, "Object Should not be found Error");
      }

  });

  sync->find("Controller", "name", a_controller_name.toStdString());

  delete sync;
}

void test_find_all()
{
  QuetzalKit::DataSync *sync = new QuetzalKit::DataSync("test_default_space_0");
  QuetzalKit::DiskSyncEngine *engine = new QuetzalKit::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](const QuetzalKit::SyncObject &a_object,
                        const std::string &a_app_name, bool a_found){
        QZ_ASSERT(a_found == 1, "All Items are Found");
  });

  sync->find("Controller", "", "");

  delete sync;
}

void test_session_list()
{
  QStringList test_data_list;

  test_data_list << "classicbackdrop"
                 << "desktopclock"
                 << "timezone"
                 << "calendar"
                 << "dock"
                 << "fail_dock"
                 << "panel";

  foreach(const QString &data, test_data_list) {
      test_save_controller_to_session(data);
  }
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //test cases:
  test_object_create();
  test_object_update();
  test_object_find_fail();
  test_object_add_child();
  test_object_add_child();
  test_object_find();
  test_object_delete();
  test_session_list();
  test_find_all();

  qDebug() << Q_FUNC_INFO << "Done";

  return app.exec();
}
