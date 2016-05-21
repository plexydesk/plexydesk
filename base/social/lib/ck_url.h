#ifndef URL_H
#define URL_H

#include <config.h>
#include <social_kit_export.h>

#include <functional>

namespace social_kit {

class url_response;

typedef std::function<void(const url_response &)> response_ready_callbcak_t;

class DECL_SOCIAL_KIT_EXPORT url_encode {
public:
  url_encode(const cherry_kit::string &a_str);
  virtual ~url_encode();

  virtual cherry_kit::string to_string() const;

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

  cherry_kit::string uri() const;
  void set_uri(const cherry_kit::string &uri);

  cherry_kit::string response_body() const;
  void set_response_body(const cherry_kit::string &response_body);

  cherry_kit::string request_body() const;
  void set_request_body(const cherry_kit::string &request_body);

  cherry_kit::string http_version() const;
  void set_http_version(const cherry_kit::string &http_version);

  cherry_kit::string method() const;
  void set_method(const cherry_kit::string &method);

  unsigned int data_buffer_size() const;
  void set_data_buffer_size(unsigned int data_buffer_size);

  char *data_buffer() const;
  void set_data_buffer(const char *data_buffer, unsigned int a_size);

private:
  int m_status_code;
  cherry_kit::string m_uri;
  cherry_kit::string m_response_body;
  cherry_kit::string m_request_body;
  cherry_kit::string m_http_version;
  cherry_kit::string m_method;

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

  void send_message(url_request_type_t a_type, const cherry_kit::string &a_message);
  void on_response_ready(response_ready_callbcak_t a_callback);

  class platform_url_request;

private:
  platform_url_request *const ctx;
};
}

#endif // URL_H
