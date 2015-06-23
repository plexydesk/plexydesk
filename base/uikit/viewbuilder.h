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

typedef std::map<std::string, std::string> ViewProperties;

class ViewBuilder {
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
    kModelView
  } ViewIdentifier;

public:
  explicit ViewBuilder(Widget *a_window);
  virtual ~ViewBuilder();

  virtual void set_geometry(float a_x, float a_y, float a_width,
                            float a_height);
  virtual void set_margine(float a_left, float a_right, float a_top,
                           float a_bottom);

  virtual void add_column(int a_count);
  virtual void split_column(int a_column_index, int a_count);

  virtual void add_rows(int a_count);
  virtual void split_row(int a_index, int a_count);
  virtual void set_row_height(int a_row, const std::string &a_height);

  virtual Widget *ui() const;
  virtual Widget *at(int a_row, int a_column);

  virtual Widget *add_widget(int a_row, int a_column,
                             const std::string &a_widget,
                             const ViewProperties &a_properties);
  virtual void update_property(int a_row, int a_column,
                               const ViewProperties &a_properties);

protected:
  Widget *add_new_widget_at(int a_col, int a_row,
                            const ViewProperties &a_props);
  Widget *add_new_button_at(int a_row, int a_col,
                            const ViewProperties &a_props);
  Widget *add_new_label_at(int a_col, int a_row, const ViewProperties &a_props);

  void layout();

private:
  class PrivateViewBuilder;
  PrivateViewBuilder *const d;
};
}
#endif // VIEWBUILDER_H
