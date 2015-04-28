#include "datasync.h"

#include <QDebug>

namespace QuetzalKit
{
typedef std::function<void (const SyncObject &, const std::string &)> FoundFunc;

  class DataSync::PrivateDataSync {
  public:
    PrivateDataSync() : m_engine (0){}
    ~PrivateDataSync()
    {
      if (m_engine)
        delete m_engine;
    }

    SyncEngineInterface *m_engine;
    std::string m_app_name;

    std::vector<FoundFunc> m_on_object_found_handler_list;

  };

  DataSync::DataSync(const std::string &a_app_name) :
    m_priv(new PrivateDataSync)
  {
    m_priv->m_app_name = a_app_name;
  }

  DataSync::~DataSync()
  {
    delete m_priv;
  }

  void DataSync::set_sync_engine(SyncEngineInterface *a_iface)
  {
    if (m_priv->m_engine)
      return;

    m_priv->m_engine = a_iface;
    m_priv->m_engine->set_app_name(m_priv->m_app_name);
  }

  void DataSync::save_object(const SyncObject &a_obj)
  {
    if (m_priv->m_engine)
      m_priv->m_engine->save_request(a_obj);
  }

  void DataSync::delete_object(const SyncObject &a_obj)
  {
    if (m_priv->m_engine)
      m_priv->m_engine->remove_object_request(a_obj);
  }

  void DataSync::find(const std::string &a_object_name)
  {
    if (m_priv->m_engine) {

        m_priv->m_engine->on_search_complete(
              [this](const QuetzalKit::SyncObject &a_obj,
              const std::string &a_app_name) {

            std::for_each(m_priv->m_on_object_found_handler_list.begin(),
                          m_priv->m_on_object_found_handler_list.end(),
                          [&](FoundFunc a_func){
              if (a_func) {
                  a_func(a_obj, a_app_name);
                }
            });
          });

        m_priv->m_engine->find(a_object_name);
      }
  }

  void DataSync::on_object_found(
      std::function<void (const SyncObject &,
                          const std::string &)> a_handler)
  {
      m_priv->m_on_object_found_handler_list.push_back(a_handler);
  }
}
