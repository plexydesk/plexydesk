#ifndef URL_H
#define URL_H

#include <string>
#include <social_kit_export.h>

#include <functional>

namespace social_kit {

class url_response;

typedef std::function<void(const url_response &)> response_ready_callbcak_t;

class DECL_SOCIAL_KIT_EXPORT url_encode {
public:
  url_encode(const std::string &a_str);
  virtual ~url_encode();

  virtual std::string to_string() const;

private:
  class platform_url_handle;
  platform_url_handle *const ctx;
};

class DECL_SOCIAL_KIT_EXPORT url_response {
public:
  url_response();
  url_response(const url_response &a_copy);
  ~url_response();

  int status_code() const;
  void set_status_code(int status_code);

  std::string uri() const;
  void set_uri(const std::string &uri);

  std::string response_body() const;
  void set_response_body(const std::string &response_body);

  std::string request_body() const;
  void set_request_body(const std::string &request_body);

  std::string http_version() const;
  void set_http_version(const std::string &http_version);

  std::string method() const;
  void set_method(const std::string &method);

  unsigned int data_buffer_size() const;
  void set_data_buffer_size(unsigned int data_buffer_size);

  char *data_buffer() const;
  void set_data_buffer(const char *data_buffer, unsigned int a_size);

private:
  int m_status_code;
  std::string m_uri;
  std::string m_response_body;
  std::string m_request_body;
  std::string m_http_version;
  std::string m_method;

  unsigned int m_data_buffer_size;
  char * m_data_buffer;
};

class DECL_SOCIAL_KIT_EXPORT url_request {
public:
  typedef enum {
    kPOSTRequest,
    kGETRequest,
    kHEADRequest,
    kDELETERequest,
    kCONNECTRequest,
    kPUTRequest,
    kOPTIONSRequest,
    kUndefinedRequest
  } url_request_type_t;

  url_request();
  virtual ~url_request();

  void send_message(url_request_type_t a_type, const std::string &a_message);
  void on_response_ready(response_ready_callbcak_t a_callback);

  class platform_url_request;

private:
  platform_url_request *const ctx;
};
}

#endif // URL_H
