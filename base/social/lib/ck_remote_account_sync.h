#ifndef CK_REMOTE_ACCOUNT_SYNC_H
#define CK_REMOTE_ACCOUNT_SYNC_H

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <functional>

#include <social_kit_export.h>

#include <ck_remote_account.h>

namespace social_kit {

typedef std::function<void(const ck_remote_account &)> discovery_notify_t;

class DECL_SOCIAL_KIT_EXPORT ck_remote_account_sync
{
public:
  ck_remote_account_sync *instance();
  virtual ~ck_remote_account_sync();

  virtual void on_account_discovered(discovery_notify_t a_callable);

private:
  ck_remote_account_sync();

  class private_sync_context;
  private_sync_context *const ctx;

  static ck_remote_account_sync *g_sync;
};
}

#endif // CK_REMOTE_ACCOUNT_SYNC_H
