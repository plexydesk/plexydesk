#include "ck_url_request_windows.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <atlconv.h>
#include <atlstr.h>
#include <windows.h>
#include <winhttp.h>

namespace social_kit {

class url_request::platform_url_request::private_context {
public:
  private_context()
      : m_header_data(NULL), m_header_size(0), m_data_buffer(NULL) {}
  ~private_context() {
    if (m_header_data != NULL)
      delete[] m_header_data;

    m_header_size = 0;

    if (m_http_request) {
      WinHttpSetStatusCallback(m_http_request, NULL, NULL, NULL);
      WinHttpCloseHandle(m_http_request);
      m_http_request = NULL;
    }

    if (m_http_connection) {
      WinHttpCloseHandle(m_http_connection);
      m_http_connection = NULL;
    }

    if (m_http_session) {
      WinHttpCloseHandle(m_http_session);
      m_http_session = NULL;
    }
  }

  void notify_listners(
      url_request::platform_url_request::private_context *a_caller_ref,
      const url_response &a_response) {
    std::for_each(std::begin(m_callback_list), std::end(m_callback_list),
                  [&](response_ready_callbcak_t a_func) {
      if (a_func)
        a_func(a_response);
    });
  }

  void parse_http_header();
  bool query_http_data();
  bool get_http_data_block();
  void update_buffer(LPSTR a_buffer, DWORD dwBytesRead);
  void get_http_status_code();

  HINTERNET m_http_session;
  HINTERNET m_http_connection;
  HINTERNET m_http_request;

  /* data */
  DWORD m_header_size = 0;
  LPVOID m_header_data = NULL;

  DWORD m_current_block_size;
  DWORD m_total_block_size;
  LPSTR m_data_buffer;

  unsigned int m_status_code;

  std::vector<response_ready_callbcak_t> m_callback_list;
};

url_request::platform_url_request::platform_url_request()
    : ctx(new private_context) {
  ctx->m_http_session = WinHttpOpen(
      L"SocialKit WinHTTP RequestAPI/1.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
}

url_request::platform_url_request::~platform_url_request() {}

void url_request::platform_url_request::on_response_ready(
    response_ready_callbcak_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

void __stdcall _http_response_func(HINTERNET hInternet, DWORD_PTR dwContext,
                                   DWORD dwInternetStatus,
                                   LPVOID lpvStatusInformation,
                                   DWORD dwStatusInformationLength) {
  url_request::platform_url_request::private_context *ctx;
  ctx = (url_request::platform_url_request::private_context *)dwContext;

  switch (dwInternetStatus) {
  case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    if (WinHttpReceiveResponse(ctx->m_http_request, NULL) == FALSE) {
    }
    break;
  case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    ctx->parse_http_header();
    ctx->get_http_status_code();

    ctx->m_current_block_size = 0;
    ctx->m_total_block_size = 0;

    if (ctx->query_http_data() == FALSE) {
      // we are done.
    }
    break;
  case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    ctx->m_current_block_size = *((LPDWORD)lpvStatusInformation);
    if (ctx->m_current_block_size == 0) {
      if (ctx->m_total_block_size) {
        // we are done notify the caller with the data.
        social_kit::url_response response;

        response.set_status_code(ctx->m_status_code);
        if (ctx->m_status_code == 200) {
          response.set_http_version("HTTP 1.0");
        }

        response.set_data_buffer_size(ctx->m_total_block_size);
        response.set_data_buffer((const unsigned char *)ctx->m_data_buffer,
                                 ctx->m_total_block_size);

        ctx->notify_listners(ctx, response);
        delete[] ctx -> m_data_buffer;
        delete[] ctx -> m_header_data;
      }
    } else {
      if (ctx->get_http_data_block() == FALSE) {
        // error cleanup;
      }
    }

    break;
  case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    if (dwStatusInformationLength != 0) {
      // update internal data buffer;
      ctx->update_buffer((LPSTR)lpvStatusInformation,
                         dwStatusInformationLength);
      if (ctx->query_http_data() == FALSE) {
        // we are done;
      }
    }
    break;
  case WINHTTP_CALLBACK_STATUS_REDIRECT:
    std::cout << __FUNCTION__ << " status redirect" << std::endl;
    break;
  case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    std::cout << __FUNCTION__ << " request error" << std::endl;
    break;
  default:
    std::cout << __FUNCTION__ << " Unknown Response" << std::endl;
    break;
  }
}

void url_request::platform_url_request::send_message_async(
    url_request_type_t a_type, const std::string &a_url,
    const url_request_context &a_form_data) {
    #error "Not Implemented"
}

void url_request::platform_url_request::send_message_async(
    url_request::url_request_type_t a_type, const std::string &a_message) {
  std::cout << "begin request " << a_message << std::endl;
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
    // todo free memeory
    std::cout << __FUNCTION__ << " url parse .... [fail]" << std::endl;
    return;
  }

  ctx->m_http_connection =
      WinHttpConnect(ctx->m_http_session, _host_name, _service_url.nPort, 0);

  if (ctx->m_http_connection == NULL) {
    // todo free memeory
    std::cout << __FUNCTION__ << " open connection .... [fail]" << std::endl;
    return;
  }

  DWORD _http_request_flags =
      (INTERNET_SCHEME_HTTPS == _service_url.nScheme) ? WINHTTP_FLAG_SECURE : 0;

  ctx->m_http_request = WinHttpOpenRequest(
      ctx->m_http_connection, L"GET", _service_url.lpszUrlPath, NULL,
      WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, _http_request_flags);
  if (ctx->m_http_request == 0) {
    std::cout << __FUNCTION__ << " build request .... [fail]" << std::endl;
    return;
  }

  WINHTTP_STATUS_CALLBACK _service_callback = WinHttpSetStatusCallback(
      ctx->m_http_request, (WINHTTP_STATUS_CALLBACK)_http_response_func,
      WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT,
      NULL);
  if (_service_callback != NULL) {
    // todo : cleanup;
    std::cout << __FUNCTION__ << " set callback .... [fail]" << std::endl;
    return;
  }

  if (!WinHttpSendRequest(ctx->m_http_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                          WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR)ctx)) {
    // todo : cleanup
    std::cout << __FUNCTION__ << " sending request .... [fail]" << std::endl;
    return;
  }
}

void url_request::platform_url_request::private_context::parse_http_header() {
  if (!WinHttpQueryHeaders(m_http_request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                           WINHTTP_HEADER_NAME_BY_INDEX, NULL, &m_header_size,
                           WINHTTP_NO_HEADER_INDEX)) {
    DWORD _error = GetLastError();
    if (_error != ERROR_INSUFFICIENT_BUFFER) {
      std::cout << __FUNCTION__ << " header request .... [fail]" << std::endl;
    }
  } else {
    std::cout << __FUNCTION__ << " header request .... [success]" << std::endl;
  }

  m_header_data = new WCHAR[m_header_size];

  if (!WinHttpQueryHeaders(m_http_request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                           WINHTTP_HEADER_NAME_BY_INDEX, m_header_data,
                           &m_header_size, WINHTTP_NO_HEADER_INDEX)) {
    std::cout << __FUNCTION__ << "Header Query .... [fail]" << std::endl;
  } else {
#ifdef __DEBUG_BUILD__
    std::string header_data = CW2A((LPWSTR)m_header_data);
    std::cout << "header data : " << header_data << std::endl;
#endif
  }
}

bool url_request::platform_url_request::private_context::query_http_data() {
  if (WinHttpQueryDataAvailable(m_http_request, NULL) == FALSE) {
    return FALSE;
  }

  return TRUE;
}

bool url_request::platform_url_request::private_context::get_http_data_block() {
  LPSTR _buffer = new char[m_current_block_size + 1];
  ZeroMemory(_buffer, m_current_block_size + 1);

  if (WinHttpReadData(m_http_request, (LPVOID)_buffer, m_current_block_size,
                      NULL) == FALSE) {
    delete[] _buffer;
    return FALSE;
  }
  return TRUE;
}

void url_request::platform_url_request::private_context::update_buffer(
    LPSTR a_buffer, DWORD dwBytesRead) {
  m_current_block_size = dwBytesRead;

  if (!m_data_buffer) {
    m_data_buffer = a_buffer;
  } else {
    LPSTR current_buffer = m_data_buffer;
    m_data_buffer = new char[m_total_block_size + m_current_block_size];

    memcpy(m_data_buffer, current_buffer, m_total_block_size);
    memcpy(m_data_buffer + m_total_block_size, a_buffer, m_current_block_size);

    delete[] current_buffer;
    delete[] a_buffer;
  }

  m_total_block_size += m_current_block_size;
}

void
url_request::platform_url_request::private_context::get_http_status_code() {
  DWORD _status_code = 0;
  DWORD _status_code_size = sizeof(_status_code);

  if (!WinHttpQueryHeaders(m_http_request, WINHTTP_QUERY_STATUS_CODE |
                                               WINHTTP_QUERY_FLAG_NUMBER,
                           WINHTTP_HEADER_NAME_BY_INDEX, &_status_code,
                           &_status_code_size, WINHTTP_NO_HEADER_INDEX)) {
  }

  m_status_code = (unsigned int)_status_code;
}
}
