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
#ifndef CHERRY_IMAGE_IO_H
#define CHERRY_IMAGE_IO_H

#include <string>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <functional>

#include <QuetzalDataKitQt_export.h>

namespace cherry_kit {
typedef unsigned char *image_data_ref;
typedef std::function<void(const std::string &)> on_save_callback_t;

class QuetzalDataKitQt_EXPORT io_surface {
public:
  io_surface();
  // io_surface(const io_surface &copy);
  virtual ~io_surface();

  virtual image_data_ref copy();

  int width;
  int height;
  image_data_ref buffer;
};

class QuetzalDataKitQt_EXPORT image_io {
public:
  typedef enum {
    kImageScaleToWidth,
    kImageScaleToHeight,
    kImageCrop,
    kImageStreach
  } image_operation_t;

  typedef enum {
    kSuccess,
    kInvalidURL,
    kOutOfMemory,
    kDataError,
    kUnknownFormat
  } buffer_load_status_t;

  typedef struct {
    int x;
    int y;
    int width;
    int height;
  } scale_options;

  image_io(int a_width, int a_height);
  virtual ~image_io();

  virtual void create(int a_width, int a_height);
  virtual void create(image_data_ref a_buffer, const size_t size);
  virtual void create(const std::string &a_file_name, bool a_preview = false);

  virtual std::string url() const;

  virtual void preview_image(const std::string &a_file_name);

  virtual io_surface *surface() const;

  virtual io_surface *add_task(image_operation_t a_method,
			const scale_options &arg);

  virtual void set_filter(const std::string &a_filter_name, int a_flag);

  virtual void
  on_ready(std::function<void(buffer_load_status_t, image_io *)> a_callback);
  
  virtual void on_image_saved(on_save_callback_t a_callback);
  virtual void save(const io_surface *a_surface, 
			const std::string &a_prefix);

private:
  class platform_image;
  class private_io_image_impl;

  private_io_image_impl *const priv;
  platform_image *const io_ctx;
};
}

#endif // CHERRY_IMAGE_IO_H
