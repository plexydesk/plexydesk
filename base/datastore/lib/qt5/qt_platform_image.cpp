#include "qt_platform_image.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QImage>

#include <future>
#include <thread>

namespace cherry_kit {

class image_io::platform_image::private_platform_image {
public:
  private_platform_image() : m_current_surface(0){}
  ~private_platform_image() {}

  bool check_in_cache(const std::string &a_file_name);
  QString cache_file_name(const std::string &a_file_name);

  std::function<void(io_surface *, image_io::buffer_load_status_t)>
      m_on_ready_call;
  std::string m_file_url;
  std::thread m_io_task;

  io_surface *m_current_surface;
  std::future<io_surface *> m_async_task_result;
  std::future<void> m_async_data_image_url;

  std::condition_variable m_notify_condition_variable;
  std::mutex m_notify_lock_mutex;

  std::vector<on_save_callback_t> m_notify_save_list;

  int m_shared_lock_value;
};

image_io::platform_image::platform_image() : priv(new private_platform_image) {}

image_io::platform_image::~platform_image() { delete priv; }

void image_io::platform_image::load_from_file(const std::string &a_file_name) {
  priv->m_file_url = a_file_name;
  priv->m_async_task_result = std::async(
      std::launch::async, &image_io::platform_image::image_decoder, this);

  QCoreApplication::processEvents();

  io_surface *result = priv->m_async_task_result.get();
	priv->m_current_surface = result;

  image_io::buffer_load_status_t status = image_io::kSuccess;

  if (!result)
    status = image_io::kDataError;

  priv->m_on_ready_call(result, status);
}

void image_io::platform_image::load_image_preview(
    const std::string &a_file_name) {
  priv->m_file_url = a_file_name;
  priv->m_async_task_result =
      std::async(std::launch::async,
                 &image_io::platform_image::image_preview_decoder, this);
  QCoreApplication::processEvents();
  io_surface *result = priv->m_async_task_result.get();
  image_io::buffer_load_status_t status = image_io::kSuccess;

  if (!result)
    status = image_io::kDataError;

  priv->m_on_ready_call(result, status);
}

void image_io::platform_image::load_from_buffer(const unsigned char *a_buffer,
                                                const size_t size) {  
  io_surface *result = new io_surface();
  QImage _qimage = QImage::fromData(a_buffer, (int)size);
  image_io::buffer_load_status_t status = image_io::kSuccess;

  if (_qimage.isNull()) {
    status = image_io::kDataError;
    delete result;
    priv->m_on_ready_call(0, status);
    return;
  }

  // result->buffer = (unsigned char *) malloc(size);
  // memcpy(result->buffer, a_buffer, size);

  result->buffer = (unsigned char *)malloc(
      4 * _qimage.height() * _qimage.width() * sizeof(unsigned char));

  memcpy(result->buffer, _qimage.constBits(),
         4 * _qimage.height() * _qimage.width() * sizeof(unsigned char));

  result->height = _qimage.height();
  result->width = _qimage.width();

  status = image_io::kSuccess;
  priv->m_on_ready_call(result, status);
}

void image_io::platform_image::on_surface_ready(
    std::function<void(io_surface *, buffer_load_status_t)> a_callback) {
  priv->m_on_ready_call = a_callback;
}

void image_io::platform_image::on_save_ready(on_save_callback_t a_callback) {
	priv->m_notify_save_list.push_back(a_callback);
}

void image_io::platform_image::save_completed(const std::string &a_file_name) {
	std::for_each(priv->m_notify_save_list.begin(),
			priv->m_notify_save_list.end(),[&] (on_save_callback_t a_func) {
			 if (a_func)
			   a_func(a_file_name);
			});
}

std::string image_io::platform_image::save_image(const io_surface *a_surface,
		const std::string &a_prefix) {
	std::string rv;

	if (!a_surface || a_surface->buffer == 0) {
		std::cout << "Null Image Data -> " <<  __FUNCTION__ << std::endl;
	    save_completed(rv);
		return rv;
	}

	std::string target_file;

	QDir file_system;
	QFile target_device;
	QString target_dir;
	QCryptographicHash crypto(QCryptographicHash::Md5);
	QImage qimage = QImage(a_surface->buffer,
			a_surface->width, a_surface->height, QImage::Format_ARGB32); 

	crypto.addData((const char *)a_surface->buffer,
			(a_surface->width * a_surface->height * 4)); 
	
	std::string data_hash(crypto.result().toHex());

	target_file = QDir::homePath().toStdString() + "/.socialkit/cache/" 
		+ "/" + a_prefix + "/";
	target_dir = QString::fromStdString(target_file); 

    file_system = QDir(target_dir);

	if (!file_system.exists()) {
		file_system.mkpath(target_dir);
	}
	
	target_file += data_hash + ".png";

	std::cout << __FUNCTION__ << "target : " << target_file << std::endl;

	target_device.setFileName(QDir::toNativeSeparators(
			QString::fromStdString(target_file)));

	if (target_device.open(QIODevice::WriteOnly)) {
	  qimage.save(&target_device, "PNG", 100);
	}

	save_completed(target_file);
	
	return rv;
}

void image_io::platform_image::save(const io_surface *a_surface,
		const std::string &a_prefix) {
	std::cout << "Request -> " <<  __FUNCTION__ << std::endl;
	priv->m_async_data_image_url = std::async(std::launch::async, [=](){
		save_image(a_surface, a_prefix);
	});
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
    qDebug() << Q_FUNC_INFO << "Error Loading image";
    return ck_surface;
  }

  ck_surface = new io_surface();
  ck_surface->width = ck_qt_image.width();
  ck_surface->height = ck_qt_image.height();

  ck_surface->buffer = (unsigned char *)malloc(
      4 * ck_qt_image.height() * ck_qt_image.width() * sizeof(unsigned char));

  memcpy(ck_surface->buffer, ck_qt_image.constBits(),
         4 * ck_qt_image.height() * ck_qt_image.width() *
             sizeof(unsigned char));

  /*
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
  */

  return ck_surface;
}

io_surface *image_io::platform_image::image_preview_decoder() {
  if (!priv->m_on_ready_call) {
    qWarning() << Q_FUNC_INFO << "Error : NO callback";
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
        return nullptr;
      }
    }

    if (!thumbnail_dir.exists()) {
      if (!QDir::home().mkdir(thumbnail_path)) {
        qDebug() << Q_FUNC_INFO << "Failed to Create :" << thumbnail_path;
        return nullptr;
      }
    }

    QString cache_file_name = QCryptographicHash::hash(priv->m_file_url.c_str(),
                                                       QCryptographicHash::Md5)
                                  .toHex();
    if (!ck_qt_image.save(
            QDir::toNativeSeparators(thumbnail_path + "/" + cache_file_name) +
            ".png")) {
      qWarning() << Q_FUNC_INFO << "Failed to Create :" << cache_file_name;
      return nullptr;
    }
  }

  if (ck_qt_image.isNull()) {
    return nullptr;
  }

  ck_surface = new io_surface();
  ck_surface->width = ck_qt_image.width();
  ck_surface->height = ck_qt_image.height();

  ck_surface->buffer = (unsigned char *)malloc(
      4 * ck_qt_image.height() * ck_qt_image.width() * sizeof(unsigned char));

  memcpy(ck_surface->buffer, ck_qt_image.constBits(),
         4 * ck_qt_image.height() * ck_qt_image.width() *
             sizeof(unsigned char));

  /*
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
  */

  return ck_surface;
}

void image_io::platform_image::emit_complete() {
  io_surface *result = priv->m_async_task_result.get();
  image_io::buffer_load_status_t status = image_io::kSuccess;

  if (!result)
    status = image_io::kDataError;

  priv->m_on_ready_call(result, status);
}

void image_io::platform_image::wait_for_signal(
    image_io::platform_image *instance) {
  std::unique_lock<std::mutex> notify_lock(priv->m_notify_lock_mutex);
  priv->m_notify_condition_variable.wait(
      notify_lock, [this] { return priv->m_shared_lock_value == 1; });
  emit_complete();
}

bool image_io::platform_image::private_platform_image::check_in_cache(
    const std::string &a_file_name) {
  QString hashed_name =
      QCryptographicHash::hash(a_file_name.c_str(), QCryptographicHash::Md5)
          .toHex();
  QString thumbnail_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

  QString cached_file =
      QDir::toNativeSeparators(thumbnail_path + "/" + hashed_name + ".png");
  QFileInfo cache_file_info(cached_file);

  return cache_file_info.exists();
}

QString image_io::platform_image::private_platform_image::cache_file_name(
    const std::string &a_file_name) {
  QString hashed_name =
      QCryptographicHash::hash(a_file_name.c_str(), QCryptographicHash::Md5)
          .toHex();
  QString thumbnail_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.cherry_io/cache/");

  QString cached_file =
      QDir::toNativeSeparators(thumbnail_path + "/" + hashed_name + ".png");

  return cached_file;
}
}
