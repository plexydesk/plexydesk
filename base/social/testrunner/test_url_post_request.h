#ifndef TEST_URL_POST_REQUEST_H
#define TEST_URL_POST_REQUEST_H

#include <string>

class test_url_post_request
{
public:
  test_url_post_request();

  void run();

  void validate_encoded_post_request();
  void validate_header_submit();

  void validate_auth_token();

  void validate_plexydesk_org_init();

private:
  void validate_account_register(const std::string &a_access_token);
};

#endif // TEST_URL_POST_REQUEST_H
