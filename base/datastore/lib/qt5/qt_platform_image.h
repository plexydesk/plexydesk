#ifndef QT_PLATFORM_IMAGE_H
#define QT_PLATFORM_IMAGE_H

#include <ck_image_io.h>
#include <functional>

namespace cherry_kit {
class image_io::platform_image {
public:
  platform_image();
  ~platform_image();

  void load_from_file(const std::string &a_file_name);
  void on_surface_ready(std::function<
      void(io_surface *, image_io::buffer_load_status_t)> a_callback);

private:
  class private_platform_image;
  private_platform_image *const o;
};
}
#endif // QT_PLATFORM_IMAGE_H
