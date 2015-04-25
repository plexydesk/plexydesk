#include <QApplication>
#include <QDebug>

#include "datasyncruntime.h"

#include <datastore.h>
#include <syncobject.h>
#include <disksyncengine.h>
#include <datasync.h>

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

void test_object_add_child()
{
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //DataSyncRuntime runner;

  test_object_create();
  test_object_update();

  return app.exec();
}
