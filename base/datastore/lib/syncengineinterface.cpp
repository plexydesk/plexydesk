#include "syncengineinterface.h"

#include <QDebug>

namespace QuetzalKit
{

class SyncEngineInterface::PrivateSyncEngineIface
{
public:
    PrivateSyncEngineIface() {}
    ~PrivateSyncEngineIface() {}

    std::vector<std::function<
        void (const SyncObject &,
              const std::string &, bool)> > m_search_handler_list;
    std::vector<std::function<
        void (const SyncObject &)> > m_insert_handler_list;

};

SyncEngineInterface::SyncEngineInterface(QObject *parent) :
    QObject(parent),
    d(new PrivateSyncEngineIface)
{}

SyncEngineInterface::~SyncEngineInterface()
{
  delete d;
}

void SyncEngineInterface::setEngineName(const QString &name)
{
    // engine name;
}

void SyncEngineInterface::on_search_complete(
        std::function<void (const SyncObject &, const std::string &, bool)> a_handler)
{
  d->m_search_handler_list.push_back(a_handler);
}

void SyncEngineInterface::on_insert_complete(
    std::function<void (const SyncObject &)> a_handler)
{
  d->m_insert_handler_list.push_back(a_handler);
}

void SyncEngineInterface::search_request_complete(const SyncObject &a_object,
                                                  std::string &a_app_name,
                                                  bool a_found)
{
    std::for_each(d->m_search_handler_list.begin(),
                  d->m_search_handler_list.end(),
                  [&](std::function<void (const SyncObject &,
                                             const std::string &, bool)> a_func) {
        if (a_func)
            a_func(a_object, a_app_name, a_found);
    });
}

void SyncEngineInterface::insert_request_complete(const SyncObject &a_object)
{
    std::for_each(d->m_insert_handler_list.begin(),
                  d->m_insert_handler_list.end(),
                  [&](std::function<void (const SyncObject & )> a_func) {
        if (a_func)
            a_func(a_object);
    });
}

}
