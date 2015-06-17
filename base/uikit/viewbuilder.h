#ifndef VIEWBUILDER_H
#define VIEWBUILDER_H

#include <iostream>
#include <variant.h>

namespace UIKit {
class Widget;
class Window;
class Label;
class Button;
class ImageButton;
class LineEdit;

typedef std::vector<std::pair<std::string, std::string>> ViewProperties;

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

  virtual void add_column(int a_count);
  virtual void add_sub_column(int a_column_index, int a_row_index, int a_count);

  virtual void add_row(int a_count);
  virtual void add_sub_row(int a_index, int a_count);

  virtual Widget *ui() const;

  virtual void add_widget(int a_col, int a_row,
                          const std::string &a_widget,
                          const ViewProperties &a_properties);
protected:
  void add_new_widget_at(int a_col, int a_row, const ViewProperties &a_props);
  void add_new_button_at(int a_col, int a_row, const ViewProperties &a_props);
  void add_new_label_at(int a_col, int a_row, const ViewProperties &a_props);
private:
  class PrivateViewBuilder;
  PrivateViewBuilder *const d;
};
}
#endif // VIEWBUILDER_H
