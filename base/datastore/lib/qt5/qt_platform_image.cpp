#include "qt_platform_image.h"
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QCryptographicHash>
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

  std::future<void> m_async_notification_result;
};

image_io::platform_image::platform_image() : o(new private_platform_image) {}

image_io::platform_image::~platform_image() {
    qDebug() << Q_FUNC_INFO;
    delete o;
}

static void qtz_set_color_value_at_pos(unsigned char **surface, int width, int x,
                                int y, int red, int green, int blue,
                                int alpha) {


  if (!(*surface))
    return;

  (*surface)[((y * width + x) * 4)] = red;
  (*surface)[((y * width + x) * 4) + 1] = green;
  (*surface)[((y * width + x) * 4) + 2] = blue;
  (*surface)[((y * width + x) * 4) + 3] = alpha;
}

void image_io::platform_image::load_from_file(const std::string &a_file_name) {
  // todo: check if the path is valid first ;
  o->m_file_url = a_file_name;

  o->m_async_notification_result = std::async(std::launch::async, [this]() {
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

    ck_surface->buffer =
        (unsigned char *)malloc(4 * ck_qt_image.height() * ck_qt_image.width());

    for (int x = 0; x < ck_qt_image.height(); ++x) {
        QRgb *row_data = (QRgb*)ck_qt_image.scanLine(x);
        for(int y = 0; y < ck_qt_image.width(); ++y) {
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

    o->m_on_ready_call(ck_surface, image_io::kSuccess);
  });
}

void
image_io::platform_image::load_image_preview(const std::string &a_file_name) {
  o->m_file_url = a_file_name;

  o->m_async_notification_result = std::async(std::launch::async, [this]() {

    if (!o->m_on_ready_call) {
      qWarning() << Q_FUNC_INFO << "Error : NO callback";
      return;
    }

    bool load_from_cache = o->check_in_cache(o->m_file_url);

    io_surface *ck_surface = nullptr;
    QImage ck_qt_image;

    if (!load_from_cache) {
        qDebug() << Q_FUNC_INFO << "Not Found in Cache"
                 << o->m_file_url.c_str();
      ck_qt_image = QImage(o->m_file_url.c_str())
                        .scaled(QSize(128, 128), Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);
    }
     else {
        qDebug() << Q_FUNC_INFO << "Load from Cache";
      ck_qt_image.load(o->cache_file_name(o->m_file_url));
    }

    if (!load_from_cache) {
      QString cache_path =
          QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/");

      QString thumbnail_path =
          QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

      QDir cache_dir(cache_path);
      QDir thumbnail_dir(thumbnail_path);

      qDebug() << Q_FUNC_INFO << "start saving preview";
      if (!cache_dir.exists()) {
        if (!QDir::home().mkdir(cache_path)) {
          qDebug() << Q_FUNC_INFO << "Failed to Create :" << cache_path;
          o->m_on_ready_call(ck_surface, image_io::kDataError);
          return;
        }
      }

      if (!thumbnail_dir.exists()) {
        if (!QDir::home().mkdir(thumbnail_path)) {
          qDebug() << Q_FUNC_INFO << "Failed to Create :" << thumbnail_path;
          o->m_on_ready_call(ck_surface, image_io::kDataError);
          return;
        }
      }

      QString cache_file_name = QCryptographicHash::hash(
          o->m_file_url.c_str(), QCryptographicHash::Md5).toHex();
      if (!ck_qt_image.save(QDir::toNativeSeparators(thumbnail_path + "/" +
                                                     cache_file_name) +
                            ".png")) {
        qDebug() << Q_FUNC_INFO << "Failed to Create :" << cache_file_name;
        o->m_on_ready_call(ck_surface, image_io::kDataError);
        return;
      }
    }

    if (ck_qt_image.isNull()) {
      o->m_on_ready_call(ck_surface, image_io::kDataError);
      return;
    }

    ck_surface = new io_surface();
    ck_surface->width = ck_qt_image.width();
    ck_surface->height = ck_qt_image.height();

    ck_surface->buffer =
        (unsigned char *)malloc(4 * ck_qt_image.height() * ck_qt_image.width());


    for (int x = 0; x < ck_qt_image.height(); ++x) {
        QRgb *row_data = (QRgb*)ck_qt_image.scanLine(x);
        for(int y = 0; y < ck_qt_image.width(); ++y) {
           // qDebug() << Q_FUNC_INFO << "->" << sizeof (row_data);
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

    o->m_on_ready_call(ck_surface, image_io::kSuccess);
  });
}

void image_io::platform_image::on_surface_ready(
    std::function<void(io_surface *, buffer_load_status_t)> a_callback) {
  o->m_on_ready_call = a_callback;
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
