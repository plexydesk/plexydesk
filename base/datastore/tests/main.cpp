#include <QApplication>
#include <QDebug>
#include <ck_image_io.h>

#include <ck_sync_object.h>
#include <ck_disk_engine.h>
#include <ck_data_sync.h>

#include <QImage>
#include <iostream>
#include <thread>

#include "px_bench.h"

#define CK_ASSERT(condition, message)                                          \
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

using namespace cherry_kit;

void test_object_create() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  cherry_kit::sync_object *obj = new cherry_kit::sync_object();
  obj->set_name("clock");

  obj->set_property("zone_id", "America/North");
  obj->set_property("id", std::to_string(1));

  sync->add_object(*obj);
  delete obj;
  delete sync;
}

void test_object_update() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "", "");

  for (int i = 0; i < 10; i++) {
    cherry_kit::sync_object *obj = new cherry_kit::sync_object();
    obj->set_name("clock");

    obj->set_property("zone_id", "Asia/South");
    obj->set_property("id", std::to_string(i));

    sync->add_object(*obj);
    delete obj;
  }

  // test update
  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {
    CK_ASSERT(a_found == 1, "Expected True");
    CK_ASSERT(a_object.name().compare("clock") == 0, "Expected clock");
    a_object.set_property("zone_id", "North/Africa");
    sync->save_object(a_object);
  });

  sync->find("clock", "id", "5");

  delete sync;
}

void test_object_find() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {

    CK_ASSERT(a_found == 1, "Expected True");
    CK_ASSERT(a_app_name.compare("Clock") == 0, "Expected Clock");
    CK_ASSERT(a_object.name().compare("clock") == 0, "Expected clock");
    CK_ASSERT(a_object.property_list().size() == 2,
              "Expected 2 got :" << a_object.property_list().size());
  });

  sync->find("clock", "zone_id", "Asia/South");

  delete sync;
}

void test_object_find_fail() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {

    CK_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("dock", "home", "alone");

  delete sync;
}

void test_object_delete_matching() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "zone_id", "Asia/South");

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {

    CK_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("clock", "zone_id", "Asia/South");

  delete sync;
}

void test_object_delete() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->remove_object("clock", "", "");

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {

    CK_ASSERT(a_found == 0, "Expected pected False : Got " << a_found);
  });

  sync->find("clock", "", "");

  delete sync;
}

void test_connected_objects() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("NoteList");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  cherry_kit::sync_object *obj = new cherry_kit::sync_object();
  obj->set_name("note");

  delete obj;
  delete sync;
}

void test_object_add_child() {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Clock");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  cherry_kit::sync_object *obj = new cherry_kit::sync_object();
  obj->set_name("clock");

  obj->set_property("zone_id", "America/North");
  obj->set_property("id", std::to_string(1));

  sync->add_object(*obj);

  delete obj;
  delete sync;
}

void test_save_controller_to_session(const cherry_kit::string &a_controller_name) {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync("test_default_space_0");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {
    if (!a_found) {
      cherry_kit::sync_object obj;
      obj.set_name("Controller");
      obj.set_property("name", a_controller_name);

      sync->add_object(obj);
      CK_ASSERT(a_found == 0, "Object Should not be found Error");
    }
  });

  sync->find("Controller", "name", a_controller_name);

  delete sync;
}

void test_find_all() {
  cherry_kit::data_sync *sync =
      new cherry_kit::data_sync("test_default_space_0");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const cherry_kit::string &a_app_name, bool a_found) {
    CK_ASSERT(a_found == 1, "All Items are Found");
  });

  sync->find("Controller", "", "");

  delete sync;
}
void test_stack_object_delete() {
  sync_object stack_object;
  stack_object.set_name("stack object");
}

void test_sync_object_delete() {
  using namespace cherry_kit;

  sync_object *object = new sync_object();

  object->set_name("parent");

  for (int i = 0; i < 10; i++) {
    sync_object *child = new sync_object(object);
    child->set_name(QString("%1").arg(i).toStdString());
  }

  for (int i = 0; i < 10; i++) {
    object->remove_object(i);
  }

  CK_ASSERT(object->child_count() == 0, "Zero Child Elements");

  delete object;

  test_stack_object_delete();
}

void test_session_list() {
  std::vector<cherry_kit::string> test_data_list;

  test_data_list.push_back("classicbackdrop");
  test_data_list.push_back("desktopclock");
  test_data_list.push_back("timezone");
  test_data_list.push_back("calendar");
  test_data_list.push_back("dock");
  test_data_list.push_back("fail_dock");
  test_data_list.push_back("panel");

  std::for_each(
      std::begin(test_data_list), std::end(test_data_list),
      [&](const cherry_kit::string &data) { test_save_controller_to_session(data); });
}

void test_image_io_surface_null_surface() {
  cherry_kit::image_io ck_img(1024, 768);
  cherry_kit::io_surface *ck_img_surface_ref = ck_img.surface();

  CK_ASSERT(ck_img_surface_ref == nullptr, "Should return a Null Surface");
}

void test_image_io_surface_create_with_size() {
  cherry_kit::image_io ck_img(0, 0);
  ck_img.create(800, 600);

  cherry_kit::io_surface *ck_img_surface_ref = ck_img.surface();

  CK_ASSERT(ck_img_surface_ref != nullptr, "Should return a Valid Surface");
  CK_ASSERT(ck_img_surface_ref->width == 800, "Expected to return 800");
  CK_ASSERT(ck_img_surface_ref->height == 600, "Expected to return 600");
}

void test_image_io_surface_create_from_file() {
  cherry_kit::image_io ck_img(0, 0);
  cherry_kit::io_surface *ck_img_surface_ref = nullptr;

  PxBenchData data;
  px_bench_run(&data);

  ck_img.on_ready([&](image_io::buffer_load_status_t a_status,
                      image_io *a_img) {
    ck_img_surface_ref = ck_img.surface();

    CK_ASSERT(a_status == image_io::kSuccess, "Should return a Success");
    CK_ASSERT(ck_img_surface_ref != nullptr, "Should return a Valid Surface");
    CK_ASSERT(ck_img_surface_ref->width == 1680, "Expected to return 800");
    CK_ASSERT(ck_img_surface_ref->height == 1050, "Expected to return 600");
    QImage test_img(ck_img_surface_ref->buffer, ck_img_surface_ref->width,
                    ck_img_surface_ref->height,
                    QImage::Format_ARGB32_Premultiplied);

    CK_ASSERT(test_img.isNull() == 0, "Expected non null qimage");

    px_bench_stop(&data);
    px_bench_print(&data, Q_FUNC_INFO);
  });

  ck_img.create("/home/siraj/Pictures/hard_and_soft_by_crazyivan969.jpg");

  qDebug() << Q_FUNC_INFO << "Sleep Till Async load is done";
  std::this_thread::sleep_for(std::chrono::seconds(4));
  qDebug() << Q_FUNC_INFO << "Main thread wakeup";
}

void test_image_io_surface_preview_from_file() {
  cherry_kit::image_io ck_img(0, 0);
  cherry_kit::io_surface *ck_img_surface_ref = nullptr;

  PxBenchData data;
  px_bench_run(&data);

  ck_img.on_ready([&](image_io::buffer_load_status_t a_status,
                      image_io *a_img) {
    ck_img_surface_ref = ck_img.surface();

    CK_ASSERT(a_status == image_io::kSuccess, "Should return a Success");
    CK_ASSERT(ck_img_surface_ref != nullptr, "Should return a Valid Surface");
    CK_ASSERT(
        ck_img_surface_ref->width == 204, "Expected 204"
                                              << " Got :"
                                              << ck_img_surface_ref->width);
    CK_ASSERT(
        ck_img_surface_ref->height == 128, "Expected 128"
                                               << " Got : "
                                               << ck_img_surface_ref->height);
    QImage test_img(ck_img_surface_ref->buffer, ck_img_surface_ref->width,
                    ck_img_surface_ref->height,
                    QImage::Format_ARGB32_Premultiplied);

    CK_ASSERT(test_img.isNull() == 0, "Expected non null qimage");

    px_bench_stop(&data);
    px_bench_print(&data, Q_FUNC_INFO);
  });

  ck_img.preview_image(
      "/home/siraj/Pictures/hard_and_soft_by_crazyivan969.jpg");

  qDebug() << Q_FUNC_INFO << "Sleep Till Async load is done";
  std::this_thread::sleep_for(std::chrono::seconds(4));
  qDebug() << Q_FUNC_INFO << "Main thread wakeup";
}

void test_image_io_surface_invalid_create_from_file() {
  cherry_kit::image_io ck_img(0, 0);
  cherry_kit::io_surface *ck_img_surface_ref = nullptr;

  PxBenchData data;
  px_bench_run(&data);

  ck_img.on_ready([&](image_io::buffer_load_status_t a_status,
                      image_io *a_img) {
    ck_img_surface_ref = ck_img.surface();
    CK_ASSERT(a_status != image_io::kSuccess, "Should return a Valid Surface");

    CK_ASSERT(ck_img_surface_ref == nullptr, "Should return a Valid Surface");

    px_bench_stop(&data);
    px_bench_print(&data, Q_FUNC_INFO);
  });

  ck_img.create("/root/home/siraj/Pictures/hard_and_soft_by_crazyivan969.jpg");

  qDebug() << Q_FUNC_INFO << "Sleep Till Async load is done";
  std::this_thread::sleep_for(std::chrono::seconds(4));

  qDebug() << Q_FUNC_INFO << "Main thread wakeup";
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  // test cases:
  test_sync_object_delete();
  test_object_create();
  test_object_update();
  test_object_find_fail();
  test_object_add_child();
  test_object_find();
  test_session_list();
  test_find_all();
  test_object_delete_matching();
  test_object_delete();

  // image surface tests.
  // test_image_io_surface_null_surface();
  // test_image_io_surface_create_with_size();
  test_image_io_surface_invalid_create_from_file();
  test_image_io_surface_preview_from_file();
  test_image_io_surface_create_from_file();

  // app.quit();

  // return app.exec();
}
