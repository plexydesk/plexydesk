#ifndef CK_PLATFORM_CONTEXT_H
#define CK_PLATFORM_CONTEXT_H

#include <QImage>

#ifdef __APPLE__
namespace cherry_kit {
class ck_color {
public:
  ck_color(float, float, float, float) {}

private:
  float m_red;
  float m_green;
  float m_blue;
  float m_alpha;
};

class rectf {
public:
  rectf() {}

};

class graphics_context {
public:
  graphics_context(QImage *a_surface_ref = 0);
  virtual ~graphics_context();

  bool is_valid() const;

  void draw_round_rect(float x, float y, float width, float height,
                       float x_radious, float y_radious);

private:
  class private_platform_context;
  private_platform_context *const ctx;
};
}
#endif

#endif // CK_PLATFORM_CONTEXT_H
