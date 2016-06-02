#include "ck_url_request_mac.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <QDebug>

namespace social_kit {

class url_request::platform_url_request::private_context {
public:
  private_context() : m_message(0) {}
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

  CFDataRef m_message;
  std::vector<response_ready_callbcak_t> m_callback_list;
};

url_request::platform_url_request::platform_url_request()
    : ctx(new private_context) {}

url_request::platform_url_request::~platform_url_request() {}

void url_request::platform_url_request::on_response_ready(
    response_ready_callbcak_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

static void web_response_callback(CFReadStreamRef readStream,
                                  CFStreamEventType type, void *a_context) {
  std::cout << "request response recived " << std::endl;
  CFMutableDataRef responseBytes = CFDataCreateMutable(kCFAllocatorDefault, 0);
  CFIndex numberOfBytesRead = 0;
  do {
    UInt8 buf[1024];
    numberOfBytesRead = CFReadStreamRead(readStream, buf, sizeof(buf));
    if (numberOfBytesRead > 0) {
      CFDataAppendBytes(responseBytes, buf, numberOfBytesRead);
    }
  } while (numberOfBytesRead > 0);

  CFHTTPMessageRef response = (CFHTTPMessageRef)CFReadStreamCopyProperty(
      readStream, kCFStreamPropertyHTTPResponseHeader);
  if (responseBytes) {
    if (response) {
      CFHTTPMessageSetBody(response, responseBytes);
    }
    CFRelease(responseBytes);
  }

  // close and cleanup
  CFReadStreamClose(readStream);
  CFReadStreamUnscheduleFromRunLoop(readStream, CFRunLoopGetCurrent(),
                                    kCFRunLoopCommonModes);
  CFRelease(readStream);

  // get class context
  url_request::platform_url_request::private_context *ctx =
      (url_request::platform_url_request::private_context *)a_context;

  if (!ctx)
    return;

  social_kit::url_response response_notify;

  // print response
  if (response) {

    CFDataRef response_body = CFHTTPMessageCopyBody(response);

    CFIndex response_body_length = CFDataGetLength(response_body);
    UInt8 *response_body_data = (UInt8 *)malloc(response_body_length);

    CFDataGetBytes(response_body, CFRangeMake(0, response_body_length),
                   response_body_data);

    response_notify.set_data_buffer_size(response_body_length);
    response_notify.set_data_buffer((const unsigned char *)response_body_data,
                                    response_body_length);
    CFRelease(response);
    CFRelease(response_body);
    free(response_body_data);

    /* get status code */
    CFIndex response_status_code = CFHTTPMessageGetResponseStatusCode(response);
    response_notify.set_status_code(response_status_code);

    /* get version */
    CFStringRef response_version = CFHTTPMessageCopyVersion(response);

    if (response_version) {
      if (CFStringCompare(response_version, kCFHTTPVersion1_1,
                          kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
        response_notify.set_http_version("HTTP 1.1");
      } else if (CFStringCompare(response_version, kCFHTTPVersion1_0,
                                 kCFCompareCaseInsensitive) ==
                 kCFCompareEqualTo) {
        response_notify.set_http_version("HTTP 1.0");
      }

      CFRelease(response_version);
    }

    /* notify */
    ctx->notify_listners(ctx, response_notify);
  }
}
void url_request::platform_url_request::send_message_async(
    url_request::url_request_type_t a_type, const std::string &a_message) {
  std::cout << "begin request " << std::endl;
  CFStringRef request_str = CFStringCreateWithBytes(
      kCFAllocatorDefault, (const UInt8 *)a_message.c_str(), a_message.size(),
      kCFStringEncodingMacRoman, false);
  CFURLRef request_url =
      CFURLCreateWithString(kCFAllocatorDefault, request_str, NULL);
  CFHTTPMessageRef request_message = CFHTTPMessageCreateRequest(
      kCFAllocatorDefault, CFSTR("GET"), request_url, kCFHTTPVersion1_0);

  CFReadStreamRef request_stream =
      CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, request_message);

  CFRelease(request_url);
  CFRelease(request_message);

  // submit request.
  CFReadStreamScheduleWithRunLoop(request_stream, CFRunLoopGetCurrent(),
                                  kCFRunLoopCommonModes);
  CFOptionFlags flags =
      (kCFStreamEventHasBytesAvailable | kCFStreamEventErrorOccurred |
       kCFStreamEventEndEncountered);
  CFStreamClientContext context = {0, ctx, NULL, NULL, NULL};
  CFReadStreamSetClient(request_stream, flags, web_response_callback, &context);
  CFReadStreamOpen(request_stream);
}
}
