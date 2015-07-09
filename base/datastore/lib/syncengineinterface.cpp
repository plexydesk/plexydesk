#include "syncengineinterface.h"

#include <QDebug>

namespace cherry {

class sync_engine_interface::PrivateSyncEngineIface {
public:
  PrivateSyncEngineIface() {}
  ~PrivateSyncEngineIface() {}

  std::vector<std::function<void(sync_object &, const std::string &, bool)> >
  m_search_handler_list;
  std::vector<std::function<void(const sync_object &)> > m_insert_handler_list;
};

sync_engine_interface::sync_engine_interface(QObject *parent)
    : QObject(parent), d(new PrivateSyncEngineIface) {}

sync_engine_interface::~sync_engine_interface() { delete d; }

void sync_engine_interface::setEngineName(const QString &name) {
  // engine name;
}

void sync_engine_interface::on_search_complete(
    std::function<void(sync_object &, const std::string &, bool)> a_handler) {
  d->m_search_handler_list.push_back(a_handler);
}

void sync_engine_interface::on_insert_complete(
    std::function<void(const sync_object &)> a_handler) {
  d->m_insert_handler_list.push_back(a_handler);
}

void sync_engine_interface::search_request_complete(sync_object &a_object,
                                                  std::string &a_app_name,
                                                  bool a_found) {
  std::for_each(
      d->m_search_handler_list.begin(), d->m_search_handler_list.end(),
      [&](std::function<void(sync_object &, const std::string &, bool)> a_func) {
        if (a_func)
          a_func(a_object, a_app_name, a_found);
      });
}

void sync_engine_interface::insert_request_complete(const sync_object &a_object) {
  std::for_each(d->m_insert_handler_list.begin(),
                d->m_insert_handler_list.end(),
                [&](std::function<void(const sync_object &)> a_func) {
    if (a_func)
      a_func(a_object);
  });
}
}
