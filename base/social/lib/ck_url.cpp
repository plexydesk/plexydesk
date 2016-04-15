#include "ck_url.h"

namespace social_kit {

class url::platform_url_handle {
public:
  platform_url_handle() {}
  ~platform_url_handle() {}
};

url::url() : ctx(new platform_url_handle) {}

url::~url() { delete ctx;}
}
