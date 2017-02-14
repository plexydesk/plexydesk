#include "pixabay_service.h"
#include <ck_remote_service.h>
#include <ck_image_io.h>
#include <asyncdatadownloader.h>

#include <string>
#include <memory>
#include <atomic>

class pixabay_service::pixabay_service_context {
public:
  pixabay_service_context()
      : m_current_progress(20), m_completion_count(0), m_page_count(0),
        m_per_page_count(8) {}
  ~pixabay_service_context() {
      m_search_ready_listners.clear();
      m_progress_listners.clear();
      std::cout << __FUNCTION__ << std::endl;
  }

  void notify(const pixabay_service *a_srv);

  std::vector<on_search_ready_t> m_search_ready_listners;
  std::vector<on_progress_t> m_progress_listners;

  social_kit::remote_result m_current_result;
  int m_page_count;
  int m_per_page_count;
  int m_current_progress;

  service_result_list m_service_list;

  std::atomic<int> m_completion_count;
};

pixabay_service::pixabay_service() : ctx(new pixabay_service_context) {}

pixabay_service::~pixabay_service() { delete ctx; }

void pixabay_service::get_page_count() {
  if (ctx->m_current_result.get("totalHits").size() > 1) {
    social_kit::remote_result_data query =
        ctx->m_current_result.get("totalHits").at(0);

    ctx->m_page_count =
        std::stoi(query.get("totalHits").value()) / ctx->m_per_page_count;
  }

  notify_progress(15);
}

void pixabay_service::get_hit_results() {
  social_kit::result_list_t list = ctx->m_current_result.get("hits");
  if (list.size() <= 0)
    return;

  std::for_each(std::begin(list), std::end(list),
                [=](const social_kit::remote_result_data &data) {
    pixabay_service_hit_result *result = new pixabay_service_hit_result();
    ctx->m_service_list.push_back(result);

    result->on_ready([=](pixabay_service_hit_result *result, bool a_valid) {
      if (!a_valid) {
        std::cout << "invalid hit result" << std::endl;
        std::unique_ptr<pixabay_service_hit_result>(result);
      }

      ctx->m_completion_count++;

      ctx->m_current_progress += 10;
      notify_progress(ctx->m_current_progress);

      if (ctx->m_completion_count == ctx->m_per_page_count) {
        ctx->notify(this);
      }
    });

    notify_progress(20);
    result->set_remote_data(data);
  });
}

void pixabay_service::notify_progress(int a_value) {
  std::for_each(std::begin(ctx->m_progress_listners),
                std::end(ctx->m_progress_listners), [&](on_progress_t a_func) {
    if (a_func)
      a_func(a_value);
  });
}

void pixabay_service::search(const std::string &a_keyword, int a_page) {
  social_kit::url_request *request = new social_kit::url_request();
  social_kit::remote_service srv_query("com.pixabay.json.api.xml");

  ctx->m_completion_count = 0;
  ctx->m_service_list.erase(ctx->m_service_list.begin(),
                            ctx->m_service_list.end());
  ctx->m_service_list.clear();

  notify_progress(0);

  request->on_response_ready([=](const social_kit::url_response &response) {
    if (response.status_code() == 200) {
      social_kit::remote_service srv_query("com.pixabay.json.api.xml");
      ctx->m_current_result =
          srv_query.response("pixabay.hd.photo.search", response);
      notify_progress(10);

      get_page_count();
      get_hit_results();
    } else {
      // error
      notify_progress(100);
      qDebug() << Q_FUNC_INFO << "Error In search result";
    }

    // delete request;
    qDebug() << Q_FUNC_INFO << "Deleteing request";
    std::unique_ptr<social_kit::url_request>(result);
  });

  /* service data */
  social_kit::service_query_parameters input_data;

  // make sure we don't pass zero as page number
  int page_num = a_page;

  if (page_num <= 0)
      page_num = 1;

  input_data.insert("key", K_SOCIAL_KIT_PIXABAY_API_KEY);
  input_data.insert("q", a_keyword);
  input_data.insert("safesearch", "1");
  input_data.insert("tag_mode", "all");
  input_data.insert("image_type", "photo");
  input_data.insert("orientation", "horizontal");
  input_data.insert("page", std::to_string(page_num));
  input_data.insert("per_page", "8");
  input_data.insert("min_width", "1920");
  input_data.insert("min_height", "1080");
  input_data.insert("editors_choise", "true");
  input_data.insert("response_group", "high_resolution");

  notify_progress(5);
  ctx->m_current_progress = 5;
  request->send_message(social_kit::url_request::kGETRequest,
                        srv_query.url("pixabay.hd.photo.search", &input_data));
}

void pixabay_service::on_ready(on_search_ready_t a_func) {
  ctx->m_search_ready_listners.push_back(a_func);
}

void pixabay_service::on_progress(on_progress_t a_callback) {
  ctx->m_progress_listners.push_back(a_callback);
}

service_result_list pixabay_service::get() const { return ctx->m_service_list; }

void
pixabay_service::pixabay_service_context::notify(const pixabay_service *a_srv) {
  std::for_each(std::begin(m_search_ready_listners),
                std::end(m_search_ready_listners),
                [&](on_search_ready_t a_func) {
    if (a_func)
      a_func(a_srv);
  });
}

class pixabay_service_hit_result::pixabay_service_hit_result_context {
public:
  pixabay_service_hit_result_context() {}
  ~pixabay_service_hit_result_context() {}

  social_kit::remote_result_data m_data;
  std::string m_id;
  std::string m_page_url;
  std::string m_type;
  std::string m_tags;
  std::string m_preview_url;
  int m_preview_width;
  int m_preview_height;
  std::string m_web_format_url;
  int m_web_format_url_width;
  int m_web_format_url_height;
  int m_views;
  int m_downloads;
  int m_favorites;
  int m_likes;
  int m_comments;
  std::string m_user_id;
  std::string m_user_name;
  std::string m_user_image_url;
 
  std::string m_large_image_url;
  std::string m_hd_image_url;
  std::string m_original_image_url;
  int m_original_image_width;
  int m_original_image_height;
  std::string m_image_hash;

  cherry_kit::image_io *m_preview_img;
  cherry_kit::image_io *m_image;

  std::vector<on_data_ready_t> m_callback_list;
};

pixabay_service_hit_result::pixabay_service_hit_result()
    : ctx(new pixabay_service_hit_result_context) {}

pixabay_service_hit_result::pixabay_service_hit_result(
    const pixabay_service_hit_result &a_copy)
    : ctx(new pixabay_service_hit_result_context) {
  ctx->m_data = a_copy.ctx->m_data;
  ctx->m_id = a_copy.ctx->m_id;
  ctx->m_page_url = a_copy.ctx->m_page_url;
  ctx->m_type = a_copy.ctx->m_type;
  ctx->m_tags = a_copy.ctx->m_tags;
  ctx->m_preview_url = a_copy.ctx->m_preview_url;
  ctx->m_preview_height = a_copy.ctx->m_preview_height;
  ctx->m_preview_width = a_copy.ctx->m_preview_width;
  ctx->m_web_format_url = a_copy.ctx->m_web_format_url;
  ctx->m_web_format_url_height = a_copy.ctx->m_web_format_url_height;
  ctx->m_web_format_url_width = a_copy.ctx->m_web_format_url_width;
  ctx->m_views = a_copy.ctx->m_views;
  ctx->m_downloads = a_copy.ctx->m_downloads;
  ctx->m_favorites = a_copy.ctx->m_favorites;
  ctx->m_likes = a_copy.ctx->m_likes;
  ctx->m_comments = a_copy.ctx->m_comments;
  ctx->m_user_id = a_copy.ctx->m_user_id;
  ctx->m_user_name = a_copy.ctx->m_user_name; 
  ctx->m_user_image_url = a_copy.ctx->m_user_image_url;
  
  ctx->m_large_image_url = a_copy.ctx->m_large_image_url;
  ctx->m_original_image_height = a_copy.ctx->m_original_image_height;
  ctx->m_original_image_url = a_copy.ctx->m_original_image_url;
  ctx->m_original_image_width = a_copy.ctx->m_original_image_width;
  ctx->m_hd_image_url = a_copy.ctx->m_hd_image_url;
  ctx->m_image_hash = a_copy.ctx->m_image_hash;

  ctx->m_image = 0;
  ctx->m_preview_img = 0;
  // todo : initate data gatering.
}

pixabay_service_hit_result::~pixabay_service_hit_result() { delete ctx; }

void pixabay_service_hit_result::set_remote_data(
    const social_kit::remote_result_data &a_data) {
  ctx->m_data = a_data;

  ctx->m_id = ctx->m_data.get("id").value();
  ctx->m_page_url = ctx->m_data.get("pageURL").value();
  ctx->m_type = ctx->m_data.get("type").value();
  ctx->m_preview_url = ctx->m_data.get("previewURL").value();
  ctx->m_preview_height = std::stoi(ctx->m_data.get("previewHeight").value());
  ctx->m_preview_width = std::stoi(ctx->m_data.get("previewWidth").value());
  ctx->m_web_format_url = ctx->m_data.get("webformatURL").value();
  ctx->m_web_format_url_width =
      std::stoi(ctx->m_data.get("webformatWidth").value());
  ctx->m_web_format_url_height =
      std::stoi(ctx->m_data.get("webformatHeight").value());
  ctx->m_user_id = ctx->m_data.get("user_id").value();
  ctx->m_user_name = ctx->m_data.get("user").value();
  ctx->m_user_image_url = ctx->m_data.get("userImageURL").value();
  ctx->m_original_image_url = ctx->m_data.get("imageURL").value();
  ctx->m_original_image_width = std::stoi(ctx->m_data.get("imageWidth").value());
  ctx->m_original_image_height = std::stoi(ctx->m_data.get("imageHeight").value());
  ctx->m_hd_image_url = ctx->m_data.get("fullHDURL").value();;
  ctx->m_large_image_url = ctx->m_data.get("largeImageURL").value();

  /* download Data */
  ctx->m_image = 0;
  ctx->m_preview_img = 0;

  social_kit::url_request *request = new social_kit::url_request();
  request->on_response_ready([&](const social_kit::url_response &response) {
    if (response.status_code() == 200) {
      cherry_kit::image_io *image = new cherry_kit::image_io(0, 0);
      image->on_ready([&](cherry_kit::image_io::buffer_load_status_t status,
                          cherry_kit::image_io *a_img) {
        if (status == cherry_kit::image_io::kSuccess) {
          ctx->m_preview_img = image;
          emit_ready(true);
        } else {
          emit_ready(false);
        }
      });

      image->create((response.data_buffer()), response.data_buffer_size());
    } else {
      emit_ready(false);
    }
  });

  request->send_message(social_kit::url_request::kGETRequest,
                        ctx->m_preview_url);
}

std::string pixabay_service_hit_result::id() const { return ctx->m_id; }

std::string pixabay_service_hit_result::page_url() const {
  return ctx->m_page_url;
}

std::string pixabay_service_hit_result::type() const { return ctx->m_type; }

std::string pixabay_service_hit_result::tags() const { return ctx->m_tags; }

std::string pixabay_service_hit_result::preview_url() const {
  return ctx->m_preview_url;
}

int pixabay_service_hit_result::preview_height() const {
  return ctx->m_preview_height;
}

int pixabay_service_hit_result::preview_width() const {
  return ctx->m_preview_width;
}

cherry_kit::io_surface *pixabay_service_hit_result::preview_image() const {
  return ctx->m_preview_img->surface();
}

std::string pixabay_service_hit_result::web_format_url() const {
  return ctx->m_web_format_url;
}

int pixabay_service_hit_result::web_format_url_height() const {
  return ctx->m_web_format_url_height;
}

int pixabay_service_hit_result::web_format_url_width() const {
  return ctx->m_web_format_url_width;
}

std::string pixabay_service_hit_result::large_image_url() const {
	return ctx->m_large_image_url;
}

std::string pixabay_service_hit_result::original_image_url() const {
	return ctx->m_original_image_url;
}

std::string pixabay_service_hit_result::hd_image_url() const {
	return ctx->m_hd_image_url;
}

void pixabay_service_hit_result::on_ready(on_data_ready_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

void pixabay_service_hit_result::emit_ready(bool a_valid) {
  std::for_each(std::begin(ctx->m_callback_list),
                std::end(ctx->m_callback_list), [&](on_data_ready_t a_func) {
    if (a_func)
      a_func(this, a_valid);
  });
}
