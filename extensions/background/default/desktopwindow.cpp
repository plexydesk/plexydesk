#include "desktopwindow.h"

#include <QImage>
#include <QPainter>

class DesktopWindow::PrivateDesktopWindow {
public:
  PrivateDesktopWindow()
      : m_background_buffer(0), m_background_width(0), m_background_height(0) {}
  ~PrivateDesktopWindow() {
    if (m_background_buffer)
      free(m_background_buffer);
  }

  QImage m_background_texture;
  unsigned char *m_background_buffer;
  int m_background_width;
  int m_background_height;
};

void DesktopWindow::reset_window_background() {
  memset(p_window->m_background_buffer, 0,
         (4 * p_window->m_background_height * p_window->m_background_width));
}

DesktopWindow::DesktopWindow()
    : UIKit::Window(0), p_window(new PrivateDesktopWindow) {
  set_window_type(UIKit::Window::kFramelessWindow);

  on_window_resized([this](Window *a_window, int a_width, int a_height) {
    if (p_window->m_background_buffer) {
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

DesktopWindow::~DesktopWindow() { delete p_window; }

void DesktopWindow::set_background(const QString &a_image_name) {
    /*
    QImage background(a_image_name);
    p_window->m_background_width = background.width();
    p_window->m_background_height = background.height();

    p_window->m_background_buffer = background.bits();
    */

    p_window->m_background_texture.load(a_image_name);

    update();
}

void DesktopWindow::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  qDebug() << Q_FUNC_INFO << a_rect;

  /*
  if (!p_window->m_background_buffer) {
    UIKit::Window::paint_view(a_ctx, a_rect);
    return;
  }

  a_ctx->drawImage(a_rect, QImage(p_window->m_background_buffer,
                                  p_window->m_background_width,
                                  p_window->m_background_height,
                                  QImage::Format_ARGB32_Premultiplied));

  UIKit::Window::paint_view(a_ctx, a_rect);
  */

  a_ctx->drawImage(a_rect, p_window->m_background_texture);
}
