#include "ck_url_request_windows.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>



namespace social_kit {

class url_request::platform_url_request::private_context {
public:
  private_context() {}
  ~private_context() {}

  void notify_listners(
      url_request::platform_url_request::private_context *a_caller_ref,
      const url_response &a_response) {
    std::for_each(std::begin(m_callback_list), std::end(m_callback_list),
                  [&](response_ready_callbcak_t a_func) {
                    if (a_func)
                      a_func(a_response);
                  });
  }

  std::vector<response_ready_callbcak_t> m_callback_list;
};

url_request::platform_url_request::platform_url_request()
    : ctx(new private_context) {}

url_request::platform_url_request::~platform_url_request() {}

void url_request::platform_url_request::on_response_ready(
    response_ready_callbcak_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

void url_request::platform_url_request::send_message_async(
    url_request::url_request_type_t a_type, const std::string &a_message) {
  std::cout << "begin request "  << a_message << std::endl;
}
}
