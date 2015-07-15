#include "qt_platform_image.h"
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QCryptographicHash>
#include <QCoreApplication>

#include <thread>
#include <future>

namespace cherry_kit {

class image_io::platform_image::private_platform_image {
public:
  private_platform_image() {}
  ~private_platform_image() {}

  bool check_in_cache(const std::string &a_file_name);
  QString cache_file_name(const std::string &a_file_name);

  std::function<void(io_surface *, image_io::buffer_load_status_t)>
  m_on_ready_call;
  std::string m_file_url;
  std::thread m_io_task;

  std::future<io_surface *> m_async_task_result;

  std::promise<io_surface *> m_async_notification_promis;

  std::condition_variable m_notify_condition_variable;
  std::mutex m_notify_lock_mutex;
  int m_shared_lock_value;
};

image_io::platform_image::platform_image() : priv(new private_platform_image) {}

image_io::platform_image::~platform_image() {
  //release();
  delete priv;
}

void image_io::platform_image::load_from_file(const std::string &a_file_name) {
  priv->m_file_url = a_file_name;
  priv->m_async_task_result = std::async(
      std::launch::async, &image_io::platform_image::image_decoder, this);

  QCoreApplication::processEvents();
  wait_for_signal(this);
}

void
image_io::platform_image::load_image_preview(const std::string &a_file_name) {
  priv->m_file_url = a_file_name;
  priv->m_async_task_result =
      std::async(std::launch::async,
                 &image_io::platform_image::image_preview_decoder, this);
  QCoreApplication::processEvents();
  wait_for_signal(this);
}

void image_io::platform_image::on_surface_ready(
    std::function<void(io_surface *, buffer_load_status_t)> a_callback) {
  priv->m_on_ready_call = a_callback;
}

void image_io::platform_image::release() {
  std::unique_lock<std::mutex> notify_lock(priv->m_notify_lock_mutex);
  priv->m_shared_lock_value = 1;
  notify_lock.unlock();
  priv->m_notify_condition_variable.notify_one();
}

io_surface *image_io::platform_image::image_decoder() {
  QImage ck_qt_image(priv->m_file_url.c_str());
  io_surface *ck_surface = nullptr;

  if (ck_qt_image.isNull()) {
    priv->m_async_notification_promis.set_value(ck_surface);
    release();
    return ck_surface;
  }

  ck_surface = new io_surface();
  ck_surface->width = ck_qt_image.width();
  ck_surface->height = ck_qt_image.height();

  ck_surface->buffer =
      (unsigned char *)malloc(4 * ck_qt_image.height() * ck_qt_image.width());

  for (int x = 0; x < ck_qt_image.height(); ++x) {
    QRgb *row_data = (QRgb *)ck_qt_image.scanLine(x);
    for (int y = 0; y < ck_qt_image.width(); ++y) {
      QRgb pixel_data = row_data[y];

      int red = qRed(pixel_data);
      int blue = qBlue(pixel_data);
      int green = qGreen(pixel_data);
      int alpha = qAlpha(pixel_data);

      int width = ck_surface->width;
      ck_surface->buffer[((x * width + y) * 4)] = blue;
      ck_surface->buffer[((x * width + y) * 4) + 1] = green;
      ck_surface->buffer[((x * width + y) * 4) + 2] = red;
      ck_surface->buffer[((x * width + y) * 4) + 3] = alpha;
    }
  }

  priv->m_async_notification_promis.set_value(ck_surface);
  release();
  return ck_surface;
}

io_surface *image_io::platform_image::image_preview_decoder() {
  if (!priv->m_on_ready_call) {
    qWarning() << Q_FUNC_INFO << "Error : NO callback";
    priv->m_async_notification_promis.set_value(nullptr);
    release();
    return nullptr;
  }

  bool load_from_cache = priv->check_in_cache(priv->m_file_url);

  io_surface *ck_surface = nullptr;
  QImage ck_qt_image;

  if (!load_from_cache) {
    ck_qt_image = QImage(priv->m_file_url.c_str())
                      .scaled(QSize(128, 128), Qt::KeepAspectRatioByExpanding,
                              Qt::SmoothTransformation);
  } else {
    ck_qt_image.load(priv->cache_file_name(priv->m_file_url));
  }

  if (!load_from_cache) {
    QString cache_path =
        QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/");

    QString thumbnail_path =
        QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

    QDir cache_dir(cache_path);
    QDir thumbnail_dir(thumbnail_path);

    if (!cache_dir.exists()) {
      if (!QDir::home().mkdir(cache_path)) {
        priv->m_async_notification_promis.set_value(nullptr);
        release();
        return nullptr;
      }
    }

    if (!thumbnail_dir.exists()) {
      if (!QDir::home().mkdir(thumbnail_path)) {
        qDebug() << Q_FUNC_INFO << "Failed to Create :" << thumbnail_path;
        priv->m_async_notification_promis.set_value(nullptr);
        release();
        return nullptr;
      }
    }

    QString cache_file_name = QCryptographicHash::hash(
        priv->m_file_url.c_str(), QCryptographicHash::Md5).toHex();
    if (!ck_qt_image.save(
             QDir::toNativeSeparators(thumbnail_path + "/" + cache_file_name) +
             ".png")) {
      qWarning() << Q_FUNC_INFO << "Failed to Create :" << cache_file_name;
      priv->m_async_notification_promis.set_value(nullptr);
      release();
      return nullptr;
    }
  }

  if (ck_qt_image.isNull()) {
    priv->m_async_notification_promis.set_value(nullptr);
    release();
    return nullptr;
  }

  ck_surface = new io_surface();
  ck_surface->width = ck_qt_image.width();
  ck_surface->height = ck_qt_image.height();

  ck_surface->buffer =
      (unsigned char *)malloc(4 * ck_qt_image.height() * ck_qt_image.width());

  for (int x = 0; x < ck_qt_image.height(); ++x) {
    QRgb *row_data = (QRgb *)ck_qt_image.scanLine(x);
    for (int y = 0; y < ck_qt_image.width(); ++y) {
      QRgb pixel_data = row_data[y];

      int red = qRed(pixel_data);
      int blue = qBlue(pixel_data);
      int green = qGreen(pixel_data);
      int alpha = qAlpha(pixel_data);

      int width = ck_surface->width;
      ck_surface->buffer[((x * width + y) * 4)] = blue;
      ck_surface->buffer[((x * width + y) * 4) + 1] = green;
      ck_surface->buffer[((x * width + y) * 4) + 2] = red;
      ck_surface->buffer[((x * width + y) * 4) + 3] = alpha;
    }
  }

  priv->m_async_notification_promis.set_value(ck_surface);
  release();
  return ck_surface;
}

void image_io::platform_image::emit_complete() {
  io_surface *result = priv->m_async_notification_promis.get_future().get();
  image_io::buffer_load_status_t status = image_io::kSuccess;

  if (!result)
    status = image_io::kDataError;

  priv->m_on_ready_call(result, status);
}

void image_io::platform_image::wait_for_signal(image_io::platform_image *instance) {
  std::unique_lock<std::mutex> notify_lock(priv->m_notify_lock_mutex);
  priv->m_notify_condition_variable.wait(
      notify_lock, [this] { return priv->m_shared_lock_value == 1; });
  emit_complete();
}

bool image_io::platform_image::private_platform_image::check_in_cache(
    const std::string &a_file_name) {
  QString hashed_name = QCryptographicHash::hash(
      a_file_name.c_str(), QCryptographicHash::Md5).toHex();
  QString thumbnail_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

  QString cached_file =
      QDir::toNativeSeparators(thumbnail_path + "/" + hashed_name + ".png");
  QFileInfo cache_file_info(cached_file);

  return cache_file_info.exists();
}

QString image_io::platform_image::private_platform_image::cache_file_name(
    const std::string &a_file_name) {
  QString hashed_name = QCryptographicHash::hash(
      a_file_name.c_str(), QCryptographicHash::Md5).toHex();
  QString thumbnail_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

  QString cached_file =
      QDir::toNativeSeparators(thumbnail_path + "/" + hashed_name + ".png");

  return cached_file;
}
}
