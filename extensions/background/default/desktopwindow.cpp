#include "desktopwindow.h"

#include <QImage>
#include <QPainter>
#include <QPixmapCache>
#include <ck_image_io.h>

class desktop_window::PrivateDesktopWindow {
public:
  PrivateDesktopWindow()
      : m_background_width(0), m_background_height(0),
        m_seamless(0), m_desktop_mode(kNone) {}
  ~PrivateDesktopWindow() {
  }

  //QImage m_background_texture;
  QPixmap m_background_pixmap;
  //unsigned char *m_background_buffer;
  int m_background_width;
  int m_background_height;
  bool m_seamless;
  DesktopScalingMode m_desktop_mode;
};

void desktop_window::reset_window_background() {
  /*
  memset(priv->m_background_buffer, 0,
         (4 * priv->m_background_height * priv->m_background_width));
         */
}

void desktop_window::set_background_size(int a_width, int a_height) {
  priv->m_background_height = a_height;
  priv->m_background_width = a_width;
  priv->m_background_pixmap = QPixmap(a_width, a_height); 
  //priv->m_background_pixmap = priv->m_background_pixmap.scaled(a_width, a_height);
  update();
}

bool desktop_window::is_seamless() { return priv->m_seamless; }

void desktop_window::set_seamless(bool mode) {
  priv->m_seamless = mode;
  update();
}

void desktop_window::set_desktop_scale_mode(
    desktop_window::DesktopScalingMode a_desktop_mode) {
  priv->m_desktop_mode = a_desktop_mode;
}

desktop_window::DesktopScalingMode desktop_window::desktop_scale_mode() const {
  return priv->m_desktop_mode;
}

desktop_window::desktop_window()
    : cherry_kit::window(0), priv(new PrivateDesktopWindow) {
  set_window_type(cherry_kit::window::kFramelessWindow);

  on_window_resized([this](window *a_window, int a_width, int a_height) {
  });
}

desktop_window::~desktop_window() { delete priv; }

void desktop_window::set_background(const std::string &a_image_name) {
  cherry_kit::image_io *ck_image = new cherry_kit::image_io(0, 0);

  ck_image->on_ready([=](
      cherry_kit::image_io::buffer_load_status_t a_load_state,
      cherry_kit::image_io *a_image_service) {
    if (a_load_state == cherry_kit::image_io::kSuccess) {
      cherry_kit::io_surface *ck_image_surface_ref = a_image_service->surface();

      cherry_kit::image_io *scale_service = new cherry_kit::image_io(0, 0);
      std::unique_ptr<cherry_kit::image_io> p(std::move(scale_service));

      scale_service->on_resize([&](cherry_kit::io_surface *surface) {
        if (!surface) {
          delete ck_image;
          return;
        }

        QImage m_background_texture = QImage(
           surface->buffer, surface->width, surface->height, QImage::Format_ARGB32);
        priv->m_background_pixmap = QPixmap::fromImage(m_background_texture);
        update();

        delete ck_image;
      });

      scale_service->resize(ck_image_surface_ref, 1920, 1080, nullptr);

    } else {
      delete ck_image;
      qWarning() << Q_FUNC_INFO << "Failed loading image!";
    }

  });

  ck_image->create(a_image_name);
}

void desktop_window::set_background(const QImage &a_image_name) {
  //priv->m_background_texture = QImage();
  //priv->m_background_texture = a_image_name;
  priv->m_background_pixmap = QPixmap::fromImage(a_image_name);
  update();
}

void desktop_window::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  if (priv->m_seamless) {
    a_ctx->setBackgroundMode(Qt::TransparentMode);
    a_ctx->setCompositionMode(QPainter::CompositionMode_Source);
    a_ctx->fillRect(a_rect, Qt::transparent);
    widget::paint_view(a_ctx, a_rect);
    return;
  }

  a_ctx->setClipRect(boundingRect());
  a_ctx->save();
 /*
  a_ctx->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                              QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing, true);
*/
  a_ctx->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform, true);

  a_ctx->setBrush(QBrush(priv->m_background_pixmap));
  a_ctx->drawRect(a_rect);

  a_ctx->restore();
}
