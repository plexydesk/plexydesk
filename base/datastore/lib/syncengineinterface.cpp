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
        void (const SyncObject &, const std::string &)> > m_search_handler_list;
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
        std::function<void (const SyncObject &, const std::string &)> a_handler)
{
  d->m_search_handler_list.push_back(a_handler);
}

void SyncEngineInterface::search_request_complete(const SyncObject &a_object,
                                                  std::string &a_app_name)
{
    std::for_each(d->m_search_handler_list.begin(),
                  d->m_search_handler_list.end(),
                  [&](std::function<void (const SyncObject &,
                                             const std::string &)> a_func) {
        if (a_func)
            a_func(a_object, a_app_name);
    });
}

}
