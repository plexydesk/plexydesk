#ifndef EVENT_STORE_H
#define EVENT_STORE_H

#include <ck_sync_object.h>
#include <ck_disk_engine.h>
#include <ck_data_sync.h>

class event_store {
public:
  event_store(const std::string &a_name);
  virtual ~event_store();

private:
  class event_store_context;
  event_store_context *const ctx;
};

#endif // EVENT_STORE_H
