#ifndef PLATFORM_URL_REQUEST_H
#define PLATFORM_URL_REQUEST_H

#include <social_kit_export.h>
#include <ck_url.h>

#include <functional>

#include <libsoup/soup.h>

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
  void append_headers(std::map<std::string, std::string> _header_data);
  void append_multipart_data(SoupMultipart *_form_content,
                             std::map<std::string, std::string> _data);

private:
  private_context *const ctx;
};
}

#endif // PLATFORM_URL_REQUEST_H
