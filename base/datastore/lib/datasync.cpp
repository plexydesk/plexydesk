#include "datasync.h"

#include <QDebug>

namespace QuetzalKit {
typedef std::function<void(SyncObject &, const std::string &, bool)> FoundFunc;

class DataSync::PrivateDataSync {
public:
  PrivateDataSync() : m_engine(0) {}
  ~PrivateDataSync() {
    if (m_engine)
      delete m_engine;
  }

  SyncEngineInterface *m_engine;
  std::string m_app_name;

  std::vector<FoundFunc> m_on_object_found_handler_list;
  std::vector<std::function<void(const SyncObject &)> > m_on_saved_handler_list;
};

DataSync::DataSync(const std::string &a_app_name)
    : m_priv(new PrivateDataSync) {
  m_priv->m_app_name = a_app_name;
}

DataSync::~DataSync() { delete m_priv; }

void DataSync::set_sync_engine(SyncEngineInterface *a_iface) {
  if (m_priv->m_engine)
    return;

  m_priv->m_engine = a_iface;
  m_priv->m_engine->set_app_name(m_priv->m_app_name);
}

void DataSync::add_object(const SyncObject &a_obj) {
  if (m_priv->m_engine) {
    m_priv->m_engine->on_insert_complete([this](const SyncObject &a_saved_obj) {
      std::for_each(m_priv->m_on_saved_handler_list.begin(),
                    m_priv->m_on_saved_handler_list.end(),
                    [&](std::function<void(const SyncObject &)> a_func) {
        if (a_func)
          a_func(a_saved_obj);
      });
    });
    m_priv->m_engine->insert_request(a_obj);
  }
}

void DataSync::save_object(const SyncObject &a_obj) {
  if (m_priv->m_engine) {
    m_priv->m_engine->update_request(a_obj);
  }
}

void DataSync::remove_object(const std::string &a_object_name,
                             const std::string &a_key,
                             const std::string &a_value) {
  if (m_priv->m_engine) {
    m_priv->m_engine->delete_request(a_object_name, a_key, a_value);
  }
}

void DataSync::find(const std::string &a_object_name,
                    const std::string &a_attrib, const std::string &a_value) {
  if (m_priv->m_engine) {

    m_priv->m_engine->on_search_complete([this](QuetzalKit::SyncObject &a_obj,
                                                const std::string &a_app_name,
                                                bool a_found) {

      a_obj.set_data_sync(this);
      std::for_each(m_priv->m_on_object_found_handler_list.begin(),
                    m_priv->m_on_object_found_handler_list.end(),
                    [&](FoundFunc a_func) {
        if (a_func) {
          a_func(a_obj, a_app_name, a_found);
        }
      });
    });

    m_priv->m_engine->find(a_object_name, a_attrib, a_value);
  }
}

void DataSync::on_object_found(
    std::function<void(SyncObject &, const std::string &, bool)> a_handler) {
  m_priv->m_on_object_found_handler_list.push_back(a_handler);
}

void
DataSync::on_object_saved(std::function<void(const SyncObject &)> a_handler) {
  m_priv->m_on_saved_handler_list.push_back(a_handler);
}
}
