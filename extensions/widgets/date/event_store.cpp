#include "event_store.h"

#include <string>
#include <iostream>

class event_store::event_store_context {
public:
  event_store_context(const std::string &a_name) : m_event_prefix(a_name) {}

  ~event_store_context() {
			printf("Deleting %s \n", __FUNCTION__);
  }

	cherry_kit::data_sync *db() {
    cherry_kit::data_sync *sync = new cherry_kit::data_sync(
				m_event_prefix + "_event_store");
    cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

    sync->set_sync_engine(engine);
	}

  std::string m_event_prefix;
};

event_store::event_store(const std::string &a_name)
    : ctx(new event_store_context(a_name)) {}

event_store::~event_store() { delete ctx; }
