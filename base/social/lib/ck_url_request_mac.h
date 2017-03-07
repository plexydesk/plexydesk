#ifndef PLATFORM_URL_REQUEST_H
#define PLATFORM_URL_REQUEST_H

#include <ck_url.h>
#include <social_kit_export.h>

#include <functional>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

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
  private_context *const ctx;
  void append_headers(CFHTTPMessageRef _message, std::map<std::string, std::string> _header_data);
  void append_multipart_data(CFHTTPMessageRef *_form_content,
                             std::map<std::string, std::string> _data);
};
}

#endif // PLATFORM_URL_REQUEST_H
