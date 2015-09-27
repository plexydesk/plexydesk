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
#ifndef VIEWBUILDER_H
#define VIEWBUILDER_H

#include <iostream>
#include <ck_widget.h>

namespace cherry_kit {
class window;
class label;
class button;
class icon_button;

typedef std::map<std::string, std::string> widget_properties_t;
typedef std::function<void ()> widget_handler_callback_t;

class DECL_UI_KIT_EXPORT fixed_layout {
  typedef enum {
    kAlignNone = 0,
    kAlignLeft = 1,
    kAlignRight = 2,
    kAlignCenter = 3,
  } Alignment;

  typedef enum {
    kWidget = 0,
    kButton,
    kImageButton,
    kImageView,
    kLabel,
    kLineEdit,
    kTextEdit,
    kProgressBar,
    kDivider,
    kModelView,
    kCalendar,
    kClock,
    kDialView,
  } ViewIdentifier;

public:
  explicit fixed_layout(widget *a_window);
  virtual ~fixed_layout();

  virtual void set_geometry(float a_x, float a_y, float a_width,
                            float a_height);
  virtual void set_content_margin(float a_left, float a_right, float a_top,
                                  float a_bottom);
  virtual void set_verticle_spacing(float a_value);
  virtual void set_horizontal_spacing(float a_value);

  virtual void add_column(int a_count);

  virtual void add_rows(int a_count);

  virtual void set_row_height(int a_row, const std::string &a_height);

  virtual void add_segments(int a_index, int a_count);
  virtual void set_segment_width(int a_row, int a_column,
                                 const std::string &a_width);

  virtual widget *viewport() const;
  virtual widget *at(int a_row, int a_column);

  virtual widget *add_widget(int a_row, int a_column,
                             const std::string &a_widget,
                             const widget_properties_t &a_properties,
                             widget_handler_callback_t a_callback = 0);
  virtual void update_property(int a_row, int a_column,
                               const widget_properties_t &a_properties);
protected:
  widget *add_new_widget_at(int a_col, int a_row,
                            const widget_properties_t &a_props,
                            widget_handler_callback_t a_callback);
  widget *add_new_button_at(int a_row, int a_col,
                            const widget_properties_t &a_props,
                            widget_handler_callback_t a_callback);
  widget *add_new_label_at(int a_col, int a_row,
                           const widget_properties_t &a_props,
                           widget_handler_callback_t a_callback);

  void layout();

private:
  class PrivateViewBuilder;
  PrivateViewBuilder *const priv;
};
}
#endif // VIEWBUILDER_H
