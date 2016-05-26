#include "pixabay_service.h"
#include <ck_remote_service.h>

#include <memory>

class pixabay_service::pixabay_service_context {
public:
  pixabay_service_context() : m_page_count(0), m_per_page_count(8) {}
  ~pixabay_service_context() {}

  void notify(const pixabay_service *a_srv);

  std::vector<on_search_ready_t> m_search_ready_listners;

  social_kit::remote_result m_current_result;
  int m_page_count;
  int m_per_page_count;
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
}

void pixabay_service::get_hit_results() {
  social_kit::result_list_t list = ctx->m_current_result.get("hits");
  if (list.size() <= 0)
    return;

  std::for_each(std::begin(list), std::end(list),
                [&](const social_kit::remote_result_data &data) {
    pixabay_service_hit_result result;
  });
}

void pixabay_service::search(const std::string &a_keyword) {
  social_kit::url_request *request = new social_kit::url_request();
  social_kit::remote_service srv_query("com.pixabay.json.api.xml");

  request->on_response_ready([&](const social_kit::url_response &response) {
    if (response.status_code() == 200) {
      social_kit::remote_service srv_query("com.pixabay.json.api.xml");
      ctx->m_current_result =
          srv_query.response("pixabay.photo.search", response);

      get_page_count();

      ctx->notify(this);
      delete request;
    } else {
      // error
    }
  });

  /* service data */
  social_kit::service_query_parameters input_data;

  input_data.insert("key", K_SOCIAL_KIT_PIXABAY_API_KEY);
  input_data.insert("q", a_keyword);
  input_data.insert("safesearch", "1");
  input_data.insert("tag_mode", "all");
  input_data.insert("page", "0");
  input_data.insert("per_page", "8");
  input_data.insert("editors_choise", "true");

  request->send_message(social_kit::url_request::kGETRequest,
                        srv_query.url("pixabay.photo.search", &input_data));
}

void pixabay_service::on_ready(on_search_ready_t a_func) {
  ctx->m_search_ready_listners.push_back(a_func);
}

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
  int m_likes;
  int m_comments;
  std::string m_user_id;
  std::string m_user_name;
  std::string m_user_image_url;
};

pixabay_service_hit_result::pixabay_service_hit_result()
    : ctx(new pixabay_service_hit_result_context) {}

pixabay_service_hit_result::pixabay_service_hit_result(
    const pixabay_service_hit_result &a_copy)
    : ctx(new pixabay_service_hit_result_context) {
  ctx->m_data = a_copy.ctx->m_data;
}

pixabay_service_hit_result::~pixabay_service_hit_result() { delete ctx; }

void pixabay_service_hit_result::set_remote_data(
    const social_kit::remote_result_data &a_data) {
  ctx->m_data = a_data;

  ctx->m_id = ctx->m_data.get("id").value();
  ctx->m_page_url = ctx->m_data.get("pageURL").value();
  ctx->m_type = ctx->m_data.get("type").value();
  ctx->m_tags  = ctx->m_data.get("tags").value();
  ctx->m_preview_url = ctx->m_data.get("previewURL").value();
  ctx->m_preview_height = std::stoi(ctx->m_data.get("previewHeight").value());
  ctx->m_preview_width = std::stoi(ctx->m_data.get("previewWidth").value());

}
