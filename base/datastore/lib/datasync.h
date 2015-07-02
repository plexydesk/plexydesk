#ifndef DATASYNC_H
#define DATASYNC_H

#include <syncengineinterface.h>
#include <syncobject.h>
#include <string>

namespace QuetzalKit {
class DataSync {
public:
  DataSync(const std::string &a_app_name);
  virtual ~DataSync();

  virtual void set_sync_engine(SyncEngineInterface *a_iface);

  virtual void add_object(const SyncObject &a_obj);
  virtual void save_object(const SyncObject &a_obj);
  virtual void remove_object(const std::string &a_object_name,
                             const std::string &a_key,
                             const std::string &a_value);

  virtual void find(const std::string &a_object_name,
                    const std::string &a_attrib, const std::string &a_value);

  virtual void on_object_found(
      std::function<void(SyncObject &, const std::string &, bool)> a_handler);
  virtual void
  on_object_saved(std::function<void(const SyncObject &a_object)> a_handler);

private:
  class PrivateDataSync;
  PrivateDataSync *const m_priv;
};
}

#endif // DATASYNC_H
