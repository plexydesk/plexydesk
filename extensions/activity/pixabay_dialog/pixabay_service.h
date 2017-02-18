#ifndef PIXABAY_SERVICE_H
#define PIXABAY_SERVICE_H

#include <config.h>
#include <functional>

#include <ck_remote_service.h>
#include <ck_image_io.h>

class pixabay_service;
class pixabay_service_hit_result;

typedef std::function<void(pixabay_service_hit_result *, bool)> on_data_ready_t;
typedef std::function<void(int)> on_progress_t;

class pixabay_service_hit_result {
public:
  pixabay_service_hit_result();
  pixabay_service_hit_result(const pixabay_service_hit_result &a_copy);
  virtual ~pixabay_service_hit_result();

  void set_remote_data(const social_kit::remote_result_data &a_data);

  std::string id() const;
  std::string page_url() const;
  std::string type() const;
  std::string tags() const;

  std::string preview_url() const;
  int preview_height() const;
  int preview_width() const;
  cherry_kit::io_surface *preview_image() const;

  std::string web_format_url() const;
  int web_format_url_height() const;
  int web_format_url_width() const;

  std::string large_image_url() const;
  std::string original_image_url() const;
  std::string hd_image_url() const;

  void on_ready(on_data_ready_t a_callback);
  
private:
  class pixabay_service_hit_result_context;
  pixabay_service_hit_result_context *const ctx;

  void emit_ready(bool a_valid);
};

typedef std::function<void(const pixabay_service *)> on_search_ready_t;
typedef std::vector<pixabay_service_hit_result *> service_result_list;

class pixabay_service {
public:
  pixabay_service();
  virtual ~pixabay_service();

  void search(const std::string &a_keyword, int a_page = 0);

  void on_ready(on_search_ready_t a_func);
  void on_progress(on_progress_t a_callback);

  service_result_list get() const;
  void remote_query(int a_page,
                    const std::string &a_keyword,
                    const std::string &a_social_key);
private:
  class pixabay_service_context;
  pixabay_service_context *const ctx;
  void get_page_count();
  void get_hit_results();

  void notify_progress(int a_value);
};

#endif // PIXABAY_SERVICE_H
