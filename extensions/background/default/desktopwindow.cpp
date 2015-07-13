#include "desktopwindow.h"

#include <QImage>
#include <QPainter>

class DesktopWindow::PrivateDesktopWindow {
public:
  PrivateDesktopWindow()
      : m_background_buffer(0), m_background_width(0), m_background_height(0),
        m_seamless(0), m_desktop_mode(kNone) {}
  ~PrivateDesktopWindow() {
    if (m_background_buffer)
      free(m_background_buffer);
  }

  QImage m_background_texture;
  unsigned char *m_background_buffer;
  int m_background_width;
  int m_background_height;
  bool m_seamless;
  DesktopScalingMode m_desktop_mode;
};

void DesktopWindow::reset_window_background() {
  memset(o_window->m_background_buffer, 0,
         (4 * o_window->m_background_height * o_window->m_background_width));
}

bool DesktopWindow::is_seamless() { return o_window->m_seamless; }

void DesktopWindow::set_seamless(bool mode) {
  o_window->m_seamless = mode;
  update();
}

void DesktopWindow::set_desktop_scale_mode(
    DesktopWindow::DesktopScalingMode a_desktop_mode) {
  o_window->m_desktop_mode = a_desktop_mode;
}

DesktopWindow::DesktopScalingMode DesktopWindow::desktop_scale_mode() const {
  return o_window->m_desktop_mode;
}

DesktopWindow::DesktopWindow()
    : cherry_kit::window(0), o_window(new PrivateDesktopWindow) {
  set_window_type(cherry_kit::window::kFramelessWindow);

  on_window_resized([this](window *a_window, int a_width, int a_height) {
    if (o_window->m_background_buffer) {
      /*
    //reset_window_background();
    p_window->m_background_width = a_width;
    p_window->m_background_height = a_height;
    p_window->m_background_buffer = (unsigned char *)realloc(
        p_window->m_background_buffer, (a_width, a_height * 4));
    //reset_window_background();
    */
    }
  });
}

DesktopWindow::~DesktopWindow() { delete o_window; }

void DesktopWindow::set_background(const QString &a_image_name) {
  /*
  QImage background(a_image_name);
  p_window->m_background_width = background.width();
  p_window->m_background_height = background.height();

  p_window->m_background_buffer = background.bits();
  */

  if (!o_window->m_background_texture.load(a_image_name))
    qDebug() << Q_FUNC_INFO << "Failed to load image";

  update();
}

void DesktopWindow::set_background(const QImage &a_image_name) {
  o_window->m_background_texture = a_image_name;
  update();
}

void DesktopWindow::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  if (o_window->m_seamless) {
    a_ctx->setBackgroundMode(Qt::TransparentMode);
    a_ctx->setCompositionMode(QPainter::CompositionMode_Source);
    a_ctx->fillRect(a_rect, Qt::transparent);

    widget::paint_view(a_ctx, a_rect);

    return;
  }

  a_ctx->save();
  a_ctx->setRenderHints(QPainter::SmoothPixmapTransform
                        | QPainter::HighQualityAntialiasing
                        | QPainter::Antialiasing);
  a_ctx->drawImage(a_rect, o_window->m_background_texture);
  a_ctx->restore();
}
