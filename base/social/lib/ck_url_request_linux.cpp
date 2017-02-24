#include "ck_url_request_linux.h"

#include <algorithm>

#include <libsoup/soup.h>
#include <libsoup/soup-session-async.h>

#include <iostream>

#include <QDebug>

#define __USER_AGENT_STRING                                                    \
  "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) "      \
  "PlexyDesk/1.0.3.1"

namespace social_kit {

typedef std::map<std::string, std::string>::iterator _map_iterator_t;

class url_request::platform_url_request::private_context {
public:
  private_context() {
    m_session = soup_session_new_with_options(SOUP_SESSION_USER_AGENT,
                                              __USER_AGENT_STRING, NULL);
    g_object_ref(m_session);
  }
  ~private_context() {
    g_object_unref(m_message);
    g_object_unref(m_session);
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

  GTask *m_current_task;
  SoupSession *m_session;
  SoupMessage *m_message;
  std::vector<response_ready_callbcak_t> m_callback_list;
};

static void platform_request_soup_stream_ready_cb(SoupSession *a_session,
                                                  SoupMessage *a_msg,
                                                  gpointer a_user_data) {
  GError *error = NULL;
  url_request::platform_url_request::private_context *ctx =
      (url_request::platform_url_request::private_context *)a_user_data;
  social_kit::url_response response;
  SoupHTTPVersion http_version_value;
  guint request_status_code = a_msg->status_code;

  response.set_status_code(request_status_code);

  if (request_status_code == SOUP_STATUS_OK) {
    http_version_value = soup_message_get_http_version(a_msg);

    if (http_version_value == SOUP_HTTP_1_0) {
      response.set_http_version("HTTP 1.0");
    } else if (http_version_value == SOUP_HTTP_1_1) {
      response.set_http_version("HTTP 1.1");
    } else {
      response.set_http_version("UNKNOWN VERSION");
    }
  } else {
    // error state;
  }

  response.set_data_buffer_size(a_msg->response_body->length);
  response.set_data_buffer((const unsigned char *)a_msg->response_body->data,
                           a_msg->response_body->length);

  ctx->notify_listners(ctx, response);
  /*
   * soup_session_queue_message is slightly unusual in that it s
   * teals a reference to the message object,
   * and unrefs it after the last callback is invoked on it.
   * So when using this API, you should not unref the message yourself.
   */
}

url_request::platform_url_request::platform_url_request()
    : ctx(new private_context) {}

url_request::platform_url_request::~platform_url_request() {}

void url_request::platform_url_request::on_response_ready(
    response_ready_callbcak_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

void url_request::platform_url_request::send_message_async(
    url_request::url_request_type_t a_type, const std::string &a_message) {
  const char *_method = SOUP_METHOD_GET;

  if (a_type == url_request::kPOSTRequest) {
    _method = SOUP_METHOD_POST;
  }

  ctx->m_message = soup_message_new(_method, a_message.c_str());

  soup_session_queue_message(ctx->m_session, ctx->m_message,
                             platform_request_soup_stream_ready_cb, ctx);
}

void url_request::platform_url_request::append_headers(
    std::map<std::string, std::string> _header_data) {
  for (_map_iterator_t iterator = _header_data.begin();
       iterator != _header_data.end(); iterator++) {
    soup_message_headers_append(ctx->m_message->request_headers,
                                iterator->first.c_str(),
                                iterator->second.c_str());
  }
}

void url_request::platform_url_request::append_multipart_data(
    SoupMultipart *_form_content, std::map<std::string, std::string> _data) {
  for (_map_iterator_t iterator = _data.begin(); iterator != _data.end();
       iterator++) {
    soup_multipart_append_form_string(_form_content, iterator->first.c_str(),
                                      iterator->second.c_str());
  }
}

void url_request::platform_url_request::send_message_async(
    url_request_type_t a_type, const std::string &a_url,
    const url_request_context &a_form_data) {
  const char *_mime_type = 0;
  std::map<std::string, std::string> _data = a_form_data.multipart_data();
  std::map<std::string, std::string> _header_data = a_form_data.header();
  SoupMultipart *_form_content = 0;
  std::string _url = a_url + "?" + a_form_data.encode();
  const char *_method = SOUP_METHOD_GET;

  if (a_type == url_request::kPOSTRequest) {
    _method = SOUP_METHOD_POST;
  }

  if (a_form_data.mime_type() == url_request_context::kMimeTypeMultipart) {
    _mime_type = SOUP_FORM_MIME_TYPE_MULTIPART;
    _form_content = soup_multipart_new(_mime_type);

    append_multipart_data(_form_content, _data);
    ctx->m_message =
        soup_form_request_new_from_multipart(_url.c_str(), _form_content);
    soup_multipart_free(_form_content);
  } else {
    _mime_type = SOUP_FORM_MIME_TYPE_URLENCODED;
    ctx->m_message = soup_message_new(_method, a_url.c_str());

    append_headers(_header_data);

    soup_message_set_request(
        ctx->m_message, "application/x-www-form-urlencoded", SOUP_MEMORY_COPY,
        a_form_data.encode().c_str(), a_form_data.encode().length());

    soup_session_queue_message(ctx->m_session, ctx->m_message,
                               platform_request_soup_stream_ready_cb, ctx);
    return;
  }

  if (!ctx->m_message) {
    std::cout << "Error : failed to submit empty ctx->m_message" << std::endl;
    return;
  }

  /* update request header */
  append_headers(_header_data);

  soup_session_queue_message(ctx->m_session, ctx->m_message,
                             platform_request_soup_stream_ready_cb, ctx);

  /* clean up */
}
}
