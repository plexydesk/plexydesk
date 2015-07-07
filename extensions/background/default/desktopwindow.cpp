#include "desktopwindow.h"

#include <QImage>
#include <QPainter>

class DesktopWindow::PrivateDesktopWindow {
public:
  PrivateDesktopWindow()
      : m_background_buffer(0), m_background_width(0), m_background_height(0),
        m_seamless(0) {}
  ~PrivateDesktopWindow() {
    if (m_background_buffer)
      free(m_background_buffer);
  }

  QImage m_background_texture;
  unsigned char *m_background_buffer;
  int m_background_width;
  int m_background_height;
  bool m_seamless;
};

void DesktopWindow::reset_window_background() {
  memset(o_window->m_background_buffer, 0,
         (4 * o_window->m_background_height * o_window->m_background_width));
}

bool DesktopWindow::is_seamless() { return o_window->m_seamless; }

void DesktopWindow::set_seamless(bool mode) { o_window->m_seamless = mode; }

DesktopWindow::DesktopWindow()
    : CherryKit::Window(0), o_window(new PrivateDesktopWindow) {
  set_window_type(CherryKit::Window::kFramelessWindow);

  on_window_resized([this](Window *a_window, int a_width, int a_height) {
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

void DesktopWindow::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  if (o_window->m_seamless) {
    a_ctx->setBackgroundMode(Qt::TransparentMode);
    a_ctx->setCompositionMode(QPainter::CompositionMode_Source);
    a_ctx->fillRect(a_rect, Qt::transparent);

    Widget::paint_view(a_ctx, a_rect);

    return;
  }

  a_ctx->drawImage(a_rect, o_window->m_background_texture);
}
