#ifndef VIEWBUILDER_H
#define VIEWBUILDER_H

#include <iostream>
#include <variant.h>

#include <widget.h>

namespace UIKit {
class Window;
class Label;
class Button;
class ImageButton;
class LineEdit;

typedef std::map<std::string, std::string> WidgetProperties;

class HybridLayout {
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
    kLabel,
    kLineEdit,
    kTextEdit,
    kProgressBar,
    kDial,
    kDivider,
    kModelView,
    kCalendar,
    kClock,
  } ViewIdentifier;

public:
  explicit HybridLayout(Widget *a_window);
  virtual ~HybridLayout();

  virtual void set_geometry(float a_x, float a_y, float a_width,
                            float a_height);
  virtual void set_content_margin(float a_left, float a_right, float a_top,
                           float a_bottom);

  virtual void add_column(int a_count);
  virtual void split_column(int a_column_index, int a_count);

  virtual void set_horizontal_segment_count(int a_count);
  virtual void add_horizontal_segments(int a_index, int a_count);
  virtual void set_horizontal_height(int a_row, const std::string &a_height);
  virtual void set_column_width(int a_row, int a_column, const std::string &a_width);

  virtual Widget *viewport() const;
  virtual Widget *at(int a_row, int a_column);

  virtual Widget *add_widget(int a_row, int a_column,
                             const std::string &a_widget,
                             const WidgetProperties &a_properties);
  virtual void update_property(int a_row, int a_column,
                               const WidgetProperties &a_properties);

protected:
  Widget *add_new_widget_at(int a_col, int a_row,
                            const WidgetProperties &a_props);
  Widget *add_new_button_at(int a_row, int a_col,
                            const WidgetProperties &a_props);
  Widget *add_new_label_at(int a_col, int a_row, const WidgetProperties &a_props);

  void layout();

private:
  class PrivateViewBuilder;
  PrivateViewBuilder *const d;
};
}
#endif // VIEWBUILDER_H
