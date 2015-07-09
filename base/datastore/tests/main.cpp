#include <QApplication>
#include <QDebug>

#include <syncobject.h>
#include <disksyncengine.h>
#include <datasync.h>

#include <iostream>

#define QZ_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__         \
                << " line " << __LINE__ << ": " << message << std::endl;       \
      std::exit(EXIT_FAILURE);                                                 \
    } else {                                                                   \
      std::cout << __LINE__ << " " << __func__ << "  " << #condition           \
                << "  [PASS]" << std::endl;                                    \
    }                                                                          \
  } while (false)

using namespace ck;

void test_object_create() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  ck::sync_object *obj = new ck::sync_object();
  obj->set_name("clock");

  obj->set_property("zone_id", "America/North");
  obj->set_property("id", std::to_string(1));

  sync->add_object(*obj);
  delete sync;
}

void test_object_update() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "", "");

  for (int i = 0; i < 10; i++) {
    ck::sync_object *obj = new ck::sync_object();
    obj->set_name("clock");

    obj->set_property("zone_id", "Asia/South");
    obj->set_property("id", std::to_string(i));

    sync->add_object(*obj);
  }

  // test update
  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    QZ_ASSERT(a_found == 1, "Expected True");
    QZ_ASSERT(a_object.name().compare("clock") == 0, "Expected clock");
    a_object.set_property("zone_id", "North/Africa");
    sync->save_object(a_object);
  });

  sync->find("clock", "id", "5");

  delete sync;
}

void test_object_find() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {

    QZ_ASSERT(a_found == 1, "Expected True");
    QZ_ASSERT(a_app_name.compare("Clock") == 0, "Expected Clock");
    QZ_ASSERT(a_object.name().compare("clock") == 0, "Expected clock");
    QZ_ASSERT(a_object.property_list().size() == 2,
              "Expected 2 got :" << a_object.property_list().size());
  });

  sync->find("clock", "zone_id", "Asia/South");

  delete sync;
}

void test_object_find_fail() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {

    QZ_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("dock", "home", "alone");

  delete sync;
}

void test_object_delete_matching() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "zone_id", "Asia/South");

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {

    QZ_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("clock", "zone_id", "Asia/South");

  delete sync;
}

void test_object_delete() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "", "");

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {

    QZ_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("clock", "", "");

  delete sync;
}

void test_connected_objects() {
  ck::data_sync *sync = new ck::data_sync("NoteList");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  ck::sync_object *obj = new ck::sync_object();
  obj->set_name("note");

  delete sync;
}

void test_object_add_child() {
  ck::data_sync *sync = new ck::data_sync("Clock");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  ck::sync_object *obj = new ck::sync_object();
  obj->set_name("clock");

  obj->set_property("zone_id", "America/North");
  obj->set_property("id", std::to_string(1));

  sync->add_object(*obj);
  delete sync;
}

void test_save_controller_to_session(const std::string &a_controller_name) {
  ck::data_sync *sync = new ck::data_sync("test_default_space_0");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (!a_found) {
      ck::sync_object obj;
      obj.set_name("Controller");
      obj.set_property("name", a_controller_name);

      sync->add_object(obj);
      QZ_ASSERT(a_found == 0, "Object Should not be found Error");
    }
  });

  sync->find("Controller", "name", a_controller_name);

  delete sync;
}

void test_find_all() {
  ck::data_sync *sync = new ck::data_sync("test_default_space_0");
  ck::DiskSyncEngine *engine = new ck::DiskSyncEngine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](ck::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    QZ_ASSERT(a_found == 1, "All Items are Found");
  });

  sync->find("Controller", "", "");

  delete sync;
}
void test_stack_object_delete() {
  sync_object stack_object;
  stack_object.set_name("stack object");
}

void test_sync_object_delete() {
  using namespace ck;

  sync_object *object = new sync_object();

  object->set_name("parent");

  for (int i = 0; i < 10; i++) {
    sync_object *child = new sync_object(object);
    child->set_name(QString("%1").arg(i).toStdString());
  }

  delete object;

  test_stack_object_delete();
}

void test_session_list() {
  std::vector<std::string> test_data_list;

  test_data_list.push_back("classicbackdrop");
  test_data_list.push_back("desktopclock");
  test_data_list.push_back("timezone");
  test_data_list.push_back("calendar");
  test_data_list.push_back("dock");
  test_data_list.push_back("fail_dock");
  test_data_list.push_back("panel");

  std::for_each(
      std::begin(test_data_list), std::end(test_data_list),
      [&](const std::string &data) { test_save_controller_to_session(data); });
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // test cases:
  test_sync_object_delete();

  test_object_create();
  test_object_update();
  test_object_find_fail();
  test_object_add_child();
  test_object_add_child();
  test_object_find();
  test_session_list();
  test_find_all();
  test_object_delete_matching();
  test_object_delete();

  qDebug() << Q_FUNC_INFO << "Done";

  return app.exec();
}
