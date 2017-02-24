#ifndef WINDOWS_PLATFORM_URL_REQUEST_H
#define WINDOWS_PLATFORM_URL_REQUEST_H

#include <ck_url.h>
#include <social_kit_export.h>

#include <functional>

#ifndef __USER_AGENT_STRING__
#define __USER_AGENT_STRING__                                                   \
  "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) "      \
  "PlexyDesk/1.0.3.1"
#endif



namespace social_kit {

class url_request::platform_url_request {
public:
  platform_url_request();
  ~platform_url_request();

  void on_response_ready(response_ready_callbcak_t a_callback);
  void send_message_async(url_request::url_request_type_t a_type,
                          const std::string &a_message);
  void send_message_async(url_request_type_t a_type, const std::string &a_url,
                          const url_request_context &a_form_data);

  class private_context;

private:
  void append_headers(std::map<std::string, std::string> _header_data);
  private_context *const ctx;
};
}

#endif // WINDOWS_PLATFORM_URL_REQUEST_H
