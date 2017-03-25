#include "test_url_post_request.h"

#include <QDebug>
#include <QDesktopServices>
#include <QImage>
#include <QMap>

#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <asyncimageloader.h>
#include <config.h>

#include <ck_remote_service.h>
#include <webservice.h>

#include <iostream>

#include <ck_image_io.h>
#include <ck_url.h>
#include <string>

#include <config.h>

#ifdef __WINDOWS_UWP_PLATFORM__
#define CK_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      qDebug() << "Assertion `" #condition "` failed in "         \
                << " line " << Q_FUNC_INFO << ": " << QString::fromStdString(std::string(message));       \
      std::exit(EXIT_FAILURE);                                                 \
    } else {                                                                   \
      qDebug() << Q_FUNC_INFO << "  " << #condition       \
                << "  [PASS]";                                    \
    }                                                                          \
  } while (false)

#else

#define CK_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__         \
                << " line " << __LINE__ << ": " << message << std::endl;       \
      std::exit(EXIT_FAILURE);                                                 \
    } else {                                                                   \
      std::cout << __LINE__ << " " << __FUNCTION__ << "  " << #condition       \
                << "  [PASS]" << std::endl;                                    \
    }                                                                          \
  } while (false)
#endif

test_url_post_request::test_url_post_request() {}

void test_url_post_request::run() {
   //validate_encoded_post_request();
   validate_auth_token();
   validate_plexydesk_org_init();
}

void test_url_post_request::validate_encoded_post_request() {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([&](const social_kit::url_response &response) {
    unsigned char *data = response.data_buffer();

    std::string buffer((const char *)data);
    qDebug() << Q_FUNC_INFO << buffer.c_str();
    CK_ASSERT(response.status_code() == 200, "Invalid Response From Server");
    CK_ASSERT(response.http_version() == "HTTP 1.1",
              "Wrong http version -> " + response.http_version());
  });

  /* service data */
  social_kit::url_request_context input_data;

  input_data.add("grant_type", "password");
  input_data.add("username", K_SOCIAL_KIT_ACCOUNT_USERNAME);
  input_data.add("password", K_SOCIAL_KIT_ACCOUNT_PASSWORD);
  input_data.add("client_id", K_SOCIAL_KIT_CLIENT_ID);
  input_data.add("client_secret", K_SOCIAL_KIT_CLIENT_SECRET);

  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  request->submit(social_kit::url_request::kPOSTRequest,
                  "https://plexydesk.org/o/token/", input_data);
}

void test_url_post_request::validate_header_submit() {
  social_kit::url_request *request = new social_kit::url_request();
  social_kit::remote_service srv_query("com.flickr.json.api.xml");

  request->on_response_ready([&](const social_kit::url_response &response) {
    CK_ASSERT(response.status_code() == 200, "Invalid Response From Server");
    CK_ASSERT(response.http_version() == "HTTP 1.1",
              "Invalid Response From Server");
  });

  /* service data */
  social_kit::url_request_context
      input_data; // social_kit::url_request_form_data();

  input_data.add("username", K_SOCIAL_KIT_ACCOUNT_USERNAME);
  input_data.add("password", K_SOCIAL_KIT_ACCOUNT_PASSWORD);
  input_data.add("api_key", K_SOCIAL_KIT_FLICKR_API_KEY);
  input_data.add("text", "sky");
  input_data.add("safe_search", "1");
  input_data.add("tags", "wallpapers,wallpaper");
  input_data.add("tag_mode", "all");
  input_data.add("page", "1");

  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  input_data.add_header("Authorization", "Bearer 000000");

  request->submit(social_kit::url_request::kGETRequest,
                  "https://plexydesk.org", input_data);
}

void test_url_post_request::validate_auth_token() {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([&](const social_kit::url_response &response) {
    qDebug() << Q_FUNC_INFO << QString((const char *)response.data_buffer());
    CK_ASSERT(response.status_code() == 200, "Invalid Response From Server");
    CK_ASSERT(response.http_version() == "HTTP 1.1",
              "Wrong http version -> " + response.http_version());

    social_kit::remote_service srv_query("org.plexydesk.api.xml");

    /* get access token */
    const social_kit::remote_result result =
        srv_query.response("plexydesk.oauth2.token", response);
    social_kit::remote_result_data token_data =
        result.get("access_token").at(0);
    CK_ASSERT(token_data.get("access_token").value().empty() == false,
              "Invalid access token");

    /* get access type */
    social_kit::remote_result_data token_type = result.get("token_type").at(0);
    CK_ASSERT(token_type.get("token_type").value().compare("Bearer") == 0,
              "Invalid access token type");

    //qDebug() << Q_FUNC_INFO << token_data.get("access_token").value().c_str();
    validate_account_register(token_data.get("access_token").value());
  });

  /* service data */
  social_kit::url_request_context input_data;

  input_data.add("grant_type", "password");
  input_data.add("username", K_SOCIAL_KIT_ACCOUNT_USERNAME);
  input_data.add("password", K_SOCIAL_KIT_ACCOUNT_PASSWORD);
  input_data.add("client_id", K_SOCIAL_KIT_CLIENT_ID);
  input_data.add("client_secret", K_SOCIAL_KIT_CLIENT_SECRET);

  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  request->submit(social_kit::url_request::kPOSTRequest,
                  "https://plexydesk.org/o/token/", input_data);
}

void test_url_post_request::validate_plexydesk_org_init() {
  social_kit::web_service *service = new social_kit::web_service(0);
  social_kit::service_query_parameters input_data;

  service->create("org.plexydesk.api.xml");

  input_data.insert("grant_type", "password");
  input_data.insert("username", K_SOCIAL_KIT_ACCOUNT_USERNAME);
  input_data.insert("password", K_SOCIAL_KIT_ACCOUNT_PASSWORD);
  input_data.insert("client_id", K_SOCIAL_KIT_CLIENT_ID);
  input_data.insert("client_secret", K_SOCIAL_KIT_CLIENT_SECRET);

  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  service->on_response_ready([&](const social_kit::remote_result &a_result,
                                 const social_kit::web_service *a_web_service) {

    /* get access token */
    CK_ASSERT(a_result.get("access_token").size() != 0, "Empty result");
    social_kit::remote_result_data token_data =
        a_result.get("access_token").at(0);
    CK_ASSERT(token_data.get("access_token").value().empty() == false,
              "Invalid access token");

    /* get access type */
    social_kit::remote_result_data token_type =
        a_result.get("token_type").at(0);
    CK_ASSERT(token_type.get("token_type").value().compare("Bearer") == 0,
              "Invalid access token type");

    validate_account_register(token_data.get("access_token").value());
  });

  service->submit("plexydesk.oauth2.token", &input_data);
}

void test_url_post_request::validate_account_register(
    const std::string &a_access_token) {
  social_kit::url_request *request = new social_kit::url_request();

  request->on_response_ready([&](const social_kit::url_response &response) {
    qDebug() << QString((const char *)response.data_buffer());
    CK_ASSERT(response.status_code() == 200, "Invalid Response From Server");
    CK_ASSERT(response.http_version() == "HTTP 1.1",
              "Wrong http version -> " + response.http_version());

    social_kit::remote_service srv_query("org.plexydesk.api.xml");

    /* get access token */
    const social_kit::remote_result result =
        srv_query.response("plexydesk.pixabay.token", response);

     social_kit::remote_result_data token_type =
        result.get("key_id").at(0);
     CK_ASSERT(token_type.get("key_id").value().compare("1") == 0,
              "Invalid access token type");

     social_kit::remote_result_data token_value =
             result.get("client_token").at(0);

     qDebug() << Q_FUNC_INFO << token_value.get("client_token").value().c_str();

     CK_ASSERT(token_value.get("client_token").value().compare("hola") == 1,
              "Invalid access token type");


  });

  /* service data */
  social_kit::url_request_context input_data;
  input_data.add("grant_type", "password");
  input_data.add("username", K_SOCIAL_KIT_ACCOUNT_USERNAME);
  input_data.add("password", K_SOCIAL_KIT_ACCOUNT_PASSWORD);
  input_data.add("client_id", K_SOCIAL_KIT_CLIENT_ID);
  input_data.add("client_secret", K_SOCIAL_KIT_CLIENT_SECRET);


  input_data.add_header("Authorization", "Bearer " + a_access_token);
  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  request->submit(social_kit::url_request::kPOSTRequest,
                  "https://plexydesk.org/api/social/pixabay/token/",
                  input_data);
}
