#ifndef QT_PLATFORM_IMAGE_H
#define QT_PLATFORM_IMAGE_H

#include <ck_image_io.h>
#include <functional>

#include <QuetzalDataKitQt_export.h>

namespace cherry_kit {
class QuetzalDataKitQt_EXPORT image_io::platform_image {
public:
  platform_image();
  ~platform_image();

  void load_from_file(const std::string &a_file_name);
  void load_image_preview(const std::string &a_file_name);

  void on_surface_ready(std::function<
      void(io_surface *, image_io::buffer_load_status_t)> a_callback);

protected:
  void release();
  void emit_complete();
  io_surface *image_decoder();
  io_surface *image_preview_decoder();
  void wait_for_signal(image_io::platform_image *instance);

private:
  class private_platform_image;
  private_platform_image *const priv;
};
}
#endif // QT_PLATFORM_IMAGE_H
