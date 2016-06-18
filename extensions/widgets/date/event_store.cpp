#include "event_store.h"

#include <iostream>

class event_store::event_store_context {
public:
  event_store_context(const std::string &a_name) : m_event_prefix(a_name) {
    cherry_kit::data_sync *sync = new cherry_kit::data_sync("EventStore");
    cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

    sync->set_sync_engine(engine);

    cherry_kit::sync_object *obj = new cherry_kit::sync_object();
    obj->set_name("event");
    obj->set_property("zone_id", "America/North");
    obj->set_property("id", std::to_string(1));

    sync->add_object(*obj);
    delete obj;
    delete sync;
  }

  ~event_store_context() {
      std::cout << __FUNCTION__ << std::endl;
  }

  std::string m_event_prefix;
};

event_store::event_store(const std::string &a_name)
    : ctx(new event_store_context(a_name)) {}

event_store::~event_store() { delete ctx; }
