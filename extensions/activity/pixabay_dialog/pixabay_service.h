#ifndef PIXABAY_SERVICE_H
#define PIXABAY_SERVICE_H

#include <config.h>
#include <functional>

#include <ck_remote_service.h>
#include <ck_image_io.h>

class pixabay_service;

class pixabay_service_hit_result {
public:
  pixabay_service_hit_result();
  pixabay_service_hit_result(const pixabay_service_hit_result &a_copy);
  virtual ~pixabay_service_hit_result();

  void set_remote_data(const social_kit::remote_result_data &a_data);

private:
  class pixabay_service_hit_result_context;
  pixabay_service_hit_result_context *const ctx;
};

typedef std::function<void(const pixabay_service *)> on_search_ready_t;

class pixabay_service {
public:
  pixabay_service();
  virtual ~pixabay_service();

  void search(const std::string &a_keyword);

  void on_ready(on_search_ready_t a_func);

private:
  class pixabay_service_context;
  pixabay_service_context *const ctx;
  void get_page_count();
  void get_hit_results();
};

#endif // PIXABAY_SERVICE_H
