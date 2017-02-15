#include "ck_url_request_linux.h"

#include <algorithm>

#include <libsoup/soup-session-async.h>

#include <iostream>

namespace social_kit {

class url_request::platform_url_request::private_context {
public:
  private_context() {
    m_session = soup_session_new_with_options(SOUP_SESSION_USER_AGENT,
                                              "social-kit-restful", NULL);
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
  response.set_data_buffer((const unsigned char *) a_msg->response_body->data,
                           response.data_buffer_size());

  ctx->notify_listners(ctx, response);
  g_object_unref(a_msg);
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
  g_object_ref(ctx->m_message);

  soup_session_queue_message(ctx->m_session, ctx->m_message,
                             platform_request_soup_stream_ready_cb, ctx);
}
}
