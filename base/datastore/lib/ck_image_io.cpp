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
#include <QImage>
#include "qt5/qt_platform_image.h"
#include <QDebug>
#else
#error "toolkit not set"
#endif

namespace cherry_kit {

class image_io::private_io_image_impl {
public:
  private_io_image_impl() : m_surface(0) {}
  ~private_io_image_impl() { delete m_surface; }

  io_surface *m_surface;
  std::function<void(buffer_load_status_t, image_io *)> m_call_on_ready;
  std::string m_url;
};

image_io::image_io(int a_width, int a_height)
    : o_surface_proxy(new platform_image), priv(new private_io_image_impl) {}

image_io::~image_io() {
  qDebug() << Q_FUNC_INFO;
  delete priv;
  delete o_surface_proxy;
}

void image_io::create(int a_width, int a_height) {}

void image_io::create(image_data_ref a_buffer, int a_width, int a_height) {}

void image_io::create(const std::string &a_file_name) {
  priv->m_url = a_file_name;
  o_surface_proxy->on_surface_ready([this](io_surface *a_surface,
                                           buffer_load_status_t a_status) {

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
  o_surface_proxy->load_from_file(a_file_name);
}

std::string image_io::url() const { return priv->m_url; }

void image_io::preview_image(const std::string &a_file_name) {
  priv->m_url = a_file_name;
  o_surface_proxy->on_surface_ready([this](io_surface *a_surface,
                                           buffer_load_status_t a_status) {
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

  o_surface_proxy->load_image_preview(a_file_name);
}

io_surface *image_io::surface() const { return priv->m_surface; }

io_surface *image_io::add_task(image_io::image_operation_t a_method,
                              const image_io::scale_options &arg) { return nullptr;}

void image_io::set_filter(const std::string &a_filter_name, int a_flag) {}

void image_io::on_ready(
    std::function<void(buffer_load_status_t, image_io *)> a_callback) {
  priv->m_call_on_ready = a_callback;
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
}

/*\class image_io
 * \define class for manipulating alpha image buffers. This class allows you to
 * create, resize, and set filters on the buffer. The filters are dynamically
 * loadable during runtime with plugins.
 * */
