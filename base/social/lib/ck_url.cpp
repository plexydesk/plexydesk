#include "ck_url.h"

#include <config.h>

#include <stdlib.h>
#include <cstring>

#include <QDebug>

#ifdef __GNU_LINUX_PLATFORM__
#include "ck_url_request_linux.h"
#endif

#ifdef __APPLE_PLATFORM__
#include "ck_url_request_mac.h"
#endif

#ifdef __WINDOWS_PLATFORM__
#include "ck_url_request_windows.h"
#endif

#include <iostream>

namespace social_kit {

/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *ck_url_encode(char *str) {
  char *pstr = str, *buf = (char *)malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' ||
        *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
static char *ck_url_decode(char *str) {
  char *pstr = str;
  char *buf = (char *)malloc(strlen(str) + 1);
  char *pbuf = buf;

  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') {
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

class url_encode::platform_url_handle {
public:
  platform_url_handle() {}
  ~platform_url_handle() {}

  std::string m_url_data;
};

url_encode::url_encode(const std::string &a_str)
    : ctx(new platform_url_handle) {
  const char *const_data = a_str.c_str();
  char *data = (char *)malloc(strlen(const_data) + 1);
#ifdef __WINDOWS_PLATFORM__
  strcpy_s(data, strlen(const_data) + 1, const_data);
#else
  strcpy(data, const_data);
#endif
  ctx->m_url_data = std::string(ck_url_encode(data));
  free(data);
}

url_encode::~url_encode() { delete ctx; }

std::string url_encode::to_string() const { return ctx->m_url_data; }

url_request::url_request() : ctx(new platform_url_request) {}

url_request::~url_request() {
  std::cout << "delete -> " << __FUNCTION__ << std::endl;
  qDebug() << Q_FUNC_INFO;
  delete ctx;
}

void url_request::send_message(url_request::url_request_type_t a_type,
                               const std::string &a_message) {
  ctx->send_message_async(a_type, a_message);
}

void url_request::on_response_ready(response_ready_callbcak_t a_callback) {
  ctx->on_response_ready(a_callback);
}

url_response::url_response() : m_data_buffer(0), m_data_buffer_size(0) {}

url_response::url_response(const url_response &a_copy) {
  m_status_code = a_copy.m_status_code;
  m_uri = a_copy.m_uri;
  m_response_body = a_copy.m_response_body;
  m_request_body = a_copy.m_request_body;
  m_http_version = a_copy.m_http_version;
  m_method = a_copy.m_method;

  if (a_copy.m_data_buffer && (m_data_buffer_size > 0)) {
    m_data_buffer = (unsigned char *)malloc(m_data_buffer_size);
    memcpy(m_data_buffer, a_copy.m_data_buffer, m_data_buffer_size);
  }
}

url_response::~url_response() {}

int url_response::status_code() const { return m_status_code; }

void url_response::set_status_code(int status_code) {
  m_status_code = status_code;
}

std::string url_response::uri() const { return m_uri; }

void url_response::set_uri(const std::string &uri) { m_uri = uri; }

std::string url_response::response_body() const { return m_response_body; }

void url_response::set_response_body(const std::string &response_body) {
  m_response_body = response_body;
}

std::string url_response::request_body() const { return m_request_body; }

void url_response::set_request_body(const std::string &request_body) {
  m_request_body = request_body;
}

std::string url_response::http_version() const { return m_http_version; }

void url_response::set_http_version(const std::string &http_version) {
  m_http_version = http_version;
}

std::string url_response::method() const { return m_method; }

void url_response::set_method(const std::string &method) { m_method = method; }

unsigned int url_response::data_buffer_size() const {
  return m_data_buffer_size;
}

void url_response::set_data_buffer_size(unsigned int data_buffer_size) {
  m_data_buffer_size = data_buffer_size;
}

unsigned char *url_response::data_buffer() const { return m_data_buffer; }

void url_response::set_data_buffer(const unsigned char *data_buffer,
                                   unsigned int a_size) {
  // m_data_buffer = data_buffer;
  m_data_buffer = (unsigned char *)malloc(a_size);
  memcpy(m_data_buffer, data_buffer, a_size);
  m_data_buffer_size = a_size;
}

/* multi part form data handling */
class url_form_data::platform_multipart_data {
public:
    platform_multipart_data() {}
    ~platform_multipart_data() {
        m_data.clear();
    }

    std::map<std::string, std::string> m_data;
};

url_form_data::url_form_data() :
    ctx(new platform_multipart_data()) {
}

url_form_data::~url_form_data() {
    delete ctx;
}

void url_form_data::add(const std::string &a_key,
                                     const std::string &a_value)
{
    ctx->m_data[a_key] = a_value;
}

void url_form_data::add_file(const std::string &a_path,
                                          const std::string &a_mime_type) {
}

}
