/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "ck_image_io.h"

#include <config.h>

#ifdef USE_QT
#include "qt5/qt_platform_image.h"
#include <QDebug>
#include <QImage>
#else
#error "toolkit not set"
#endif

#include <future>
#include <thread>

namespace cherry_kit {

class image_io::private_io_image_impl {
public:
  private_io_image_impl() : m_surface(0) {}
  ~private_io_image_impl() {
      if (m_surface)
        delete m_surface;
  }

  io_surface *m_surface;
  std::function<void(buffer_load_status_t, image_io *)> m_call_on_ready;

  std::string m_url;

  std::future<io_surface *> m_async_op;
  std::future<std::string> m_async_sync_op;
};

image_io::image_io(int a_width, int a_height)
    : io_ctx(new platform_image), priv(new private_io_image_impl) {}

image_io::~image_io() {
  qDebug() << Q_FUNC_INFO;
  delete io_ctx;
  delete priv;
}

void image_io::create(int a_width, int a_height) {}

void image_io::create(image_data_ref a_buffer, const size_t size) {
  io_ctx->on_surface_ready(
      [this](io_surface *a_surface, buffer_load_status_t a_status) {

        if (a_status != kSuccess) {
          if (priv->m_call_on_ready)
            priv->m_call_on_ready(a_status, this);
          return;
        }

        if (priv->m_surface) {
          delete priv->m_surface;
          priv->m_surface = nullptr;
        }

        priv->m_surface = a_surface;

        if (priv->m_call_on_ready)
          priv->m_call_on_ready(kSuccess, this);
      });

  io_ctx->load_from_buffer(a_buffer, size);
}

void image_io::create(const std::string &a_file_name, bool a_preview) {
  if (a_preview) {
    preview_image(a_file_name);
    return;
  }

  priv->m_url = a_file_name;
  io_ctx->on_surface_ready(
      [this](io_surface *a_surface, buffer_load_status_t a_status) {

        if (a_status != kSuccess) {
          if (priv->m_call_on_ready)
            priv->m_call_on_ready(a_status, this);
          return;
        }

        if (priv->m_surface) {
          delete priv->m_surface;
          priv->m_surface = nullptr;
        }

        priv->m_surface = a_surface;

        if (priv->m_call_on_ready)
          priv->m_call_on_ready(kSuccess, this);
      });
  io_ctx->load_from_file(a_file_name);
}

std::string image_io::url() const { return priv->m_url; }

void image_io::preview_image(const std::string &a_file_name) {
  priv->m_url = a_file_name;
  io_ctx->on_surface_ready(
      [this](io_surface *a_surface, buffer_load_status_t a_status) {
        if (a_status != kSuccess) {
          if (priv->m_call_on_ready)
            priv->m_call_on_ready(a_status, this);
          return;
        }

        if (priv->m_surface) {
          delete priv->m_surface;
          priv->m_surface = nullptr;
        }

        priv->m_surface = a_surface;

        if (priv->m_call_on_ready)
          priv->m_call_on_ready(kSuccess, this);
      });

  io_ctx->load_image_preview(a_file_name);
}

io_surface *image_io::surface() const { return priv->m_surface; }

io_surface *image_io::add_task(image_io::image_operation_t a_method,
                               const image_io::scale_options &arg) {
  return nullptr;
}

void image_io::set_filter(const std::string &a_filter_name, int a_flag) {}

void image_io::on_ready(
    std::function<void(buffer_load_status_t, image_io *)> a_callback) {
  priv->m_call_on_ready = a_callback;
}

void image_io::on_image_saved(on_save_callback_t a_callback) {
  io_ctx->on_save_ready(a_callback);
}

void image_io::save(const io_surface *a_surface, const std::string &a_prefix) {
  priv->m_async_sync_op = std::async(std::launch::async, [=]() {
    return io_ctx->save(a_surface, a_prefix);
  });

  io_ctx->notify_save(priv->m_async_sync_op.get());
}

void image_io::on_resize(on_resize_callback_t a_callback) {
	io_ctx->on_resize(a_callback);
}

void image_io::resize(io_surface *a_surface, int a_width, int a_height,
                      on_resize_callback_t a_callback) {
  priv->m_async_op = std::async(std::launch::async, [=]() {
      return io_ctx->resize(a_surface, a_width, a_height, a_callback);
  });

  std::cout << "task status : " << std::endl;
  io_surface *rv = priv->m_async_op.get();
  io_ctx->notify_resize(rv);
  delete rv;
}

io_surface::io_surface() : width(0), height(0), buffer(nullptr) {}

/*
io_surface::io_surface(const io_surface &copy)
    : width(copy.width), height(copy.height) {
  std::memcpy(buffer, copy.buffer, 4 * copy.width * copy.height);
}
*/

io_surface::~io_surface() {
  if (buffer)
    free(buffer);
}

image_data_ref io_surface::copy() {
  size_t buffer_size = 4 * width * height;
  image_data_ref buffer_copy = (image_data_ref)malloc(buffer_size);
  memcpy(buffer_copy, buffer, buffer_size);

  return buffer_copy;
}

io_surface *io_surface::dup()
{
    io_surface *rv = new io_surface();

    rv->width = width;
    rv->height = height;
    rv->buffer = copy();
    return rv;
}
}

/*\class image_io
 * \define class for manipulating alpha image buffers. This class allows you to
 * create, resize, and set filters on the buffer. The filters are dynamically
 * loadable during runtime with plugins.
 * */
