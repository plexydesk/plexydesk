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
  void load_from_buffer(const unsigned char *a_buffer, const size_t size);

  void on_surface_ready(std::function<
      void(io_surface *, image_io::buffer_load_status_t)> a_callback);

  void on_save_ready(on_save_callback_t a_callback);
  void save(const io_surface *a_surface, const std::string &a_prefix = "data");

  void resize(const io_surface *a_surface, int a_width, int a_height,
              on_resize_callback_t a_callback);

protected:
  void release();
  void emit_complete();
  io_surface *image_decoder();
  io_surface *image_preview_decoder();
  void wait_for_signal(image_io::platform_image *instance);

  std::string save_image(const io_surface *a_surface,
                         const std::string &a_prefix);
  void save_completed(const std::string &a_file_name);

  io_surface *resize_image(const io_surface *a_surface, int a_width,
                           int a_height);
private:
  class private_platform_image;
  private_platform_image *const priv;
};
}
#endif // QT_PLATFORM_IMAGE_H
