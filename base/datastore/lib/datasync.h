#ifndef DATASYNC_H
#define DATASYNC_H

#include <syncengineinterface.h>
#include <syncobject.h>
#include <string>

namespace QuetzalKit {
  class DataSync
  {
  public:
    DataSync(const std::string &a_app_name);
    virtual ~DataSync();

    virtual void set_sync_engine(SyncEngineInterface *a_iface);

    virtual void save_object(const SyncObject &a_obj);
    virtual void delete_object(const SyncObject &a_obj);

    virtual void find(const std::string &a_object_name);
    virtual void on_object_found(
            std::function<void (const std::string &a_app_name,
                                 const SyncObject &a_object)> a_handler);

  private:
    class PrivateDataSync;
    PrivateDataSync * const m_priv;
  };
}

#endif // DATASYNC_H
