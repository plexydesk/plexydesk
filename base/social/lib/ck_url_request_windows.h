#ifndef WINDOWS_PLATFORM_URL_REQUEST_H
#define WINDOWS_PLATFORM_URL_REQUEST_H

#include <ck_url.h>
#include <social_kit_export.h>

#include <functional>

namespace social_kit {

class url_request::platform_url_request {
public:
  platform_url_request();
  ~platform_url_request();

  void on_response_ready(response_ready_callbcak_t a_callback);
  void send_message_async(url_request::url_request_type_t a_type,
                          const std::string &a_message);
  class private_context;

private:
  private_context *const ctx;
};
}

#endif // WINDOWS_PLATFORM_URL_REQUEST_H
