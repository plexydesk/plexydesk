#include "qt_platform_image.h"
#include <QDebug>
#include <QImage>
#include <thread>

namespace cherry_kit {

class image_io::platform_image::private_platform_image {
public:
  private_platform_image() {}
  ~private_platform_image() {}

  std::function<void(io_surface *, image_io::buffer_load_status_t)>
  m_on_ready_call;
  std::string m_file_url;
};

image_io::platform_image::platform_image() : o(new private_platform_image) {}

image_io::platform_image::~platform_image() { delete o; }

void image_io::platform_image::load_from_file(const std::string &a_file_name) {
  // todo: check if the path is valid first ;
  o->m_file_url = a_file_name;
  std::thread task([this]() {
    if (!o->m_on_ready_call) {
      qWarning() << Q_FUNC_INFO << "Error : NO callback";
      return;
    }

    QImage ck_qt_image(o->m_file_url.c_str());
    io_surface *ck_surface = nullptr;

    if (ck_qt_image.isNull()) {
      o->m_on_ready_call(ck_surface, image_io::kDataError);
      return;
    }

    ck_surface = new io_surface();
    ck_surface->width = ck_qt_image.width();
    ck_surface->height = ck_qt_image.height();
    const unsigned char *bits = ck_qt_image.constBits();
    ck_surface->buffer =
        (unsigned char *)malloc((ck_qt_image.width() * ck_qt_image.height()));
    memcpy(ck_surface->buffer, bits,
           (ck_qt_image.width() * ck_qt_image.height()));

    o->m_on_ready_call(ck_surface, image_io::kSuccess);
  });

  task.detach();
}

void image_io::platform_image::on_surface_ready(
    std::function<void(io_surface *, buffer_load_status_t)> a_callback) {
  o->m_on_ready_call = a_callback;
}
}
