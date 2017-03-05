#include "ck_remote_account_sync.h"

namespace social_kit {
typedef std::vector<discovery_notify_t> discovery_notify_chain_t;

ck_remote_account_sync *ck_remote_account_sync::g_sync = 0;

class ck_remote_account_sync::private_sync_context {
public:
  private_sync_context() {}
  ~private_sync_context() {
    m_discovery_notify_callables.clear();
  }

  discovery_notify_chain_t m_discovery_notify_callables;
};

ck_remote_account_sync *ck_remote_account_sync::instance()
{
  if (g_sync == 0) {
    g_sync = new ck_remote_account_sync();
    return g_sync;
  } else {
    return g_sync;
  }
}

ck_remote_account_sync::ck_remote_account_sync() :
  ctx(new private_sync_context)
{
}

ck_remote_account_sync::~ck_remote_account_sync()
{
  delete ctx;

  if (g_sync)
    delete g_sync;

  g_sync = 0;
}

void ck_remote_account_sync::on_account_discovered(
    discovery_notify_t a_callable)
{
  ctx->m_discovery_notify_callables.push_back(a_callable);
}
}
