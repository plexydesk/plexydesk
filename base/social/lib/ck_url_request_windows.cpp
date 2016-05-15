#include "ck_url_request_windows.h"

#include <algorithm>
#include <iostream>
#include <string >
#include <vector>

#include <atlconv.h>
#include <windows.h>
#include <winhttp.h>

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

  HINTERNET m_http_session;
  std::vector<response_ready_callbcak_t> m_callback_list;
};

url_request::platform_url_request::platform_url_request()
    : ctx(new private_context) {
  ctx->m_http_session = WinHttpOpen( L"SocialKit WinHTTP RequestAPI/1.1",
                                   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                   WINHTTP_NO_PROXY_NAME,
                                   WINHTTP_NO_PROXY_BYPASS,
                                   WINHTTP_FLAG_ASYNC);
}

url_request::platform_url_request::~platform_url_request() {}

void url_request::platform_url_request::on_response_ready(
    response_ready_callbcak_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

void url_request::platform_url_request::send_message_async(
    url_request::url_request_type_t a_type, const std::string &a_message) {
  std::cout << "begin request "  << a_message << std::endl;
  USES_CONVERSION;
  LPWSTR _message = A2W(a_message.c_str());
  URL_COMPONENTS _service_url;
  WCHAR _host_name[256];

  ZeroMemory(&_service_url, sizeof(_service_url));
  _service_url.dwStructSize = sizeof(_service_url);
  _service_url.lpszHostName = _host_name;
  _service_url.dwHostNameLength = sizeof(_host_name) / sizeof(_host_name[0]);
  _service_url.dwUrlPathLength = -1;
  _service_url.dwSchemeLength = -1;

  if (!WinHttpCrackUrl(_message, 0, 0, &_service_url)) {
     //todo free memeory
     return;
  }

  WinHttpConnect(ctx->m_http_session, _host_name, _service_url.nPort, 0);
}
}
