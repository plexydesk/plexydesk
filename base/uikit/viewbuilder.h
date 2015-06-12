#ifndef VIEWBUILDER_H
#define VIEWBUILDER_H

#include <iostream>

namespace UIKit {
class Widget;
class Window;
class Label;
class Button;
class ImageButton;
class LineEdit;

class ViewBuilder {
  typedef enum {
    kAlignNone = 0,
    kAlignLeft = 1,
    kAlignRight = 2,
    kAlignCenter = 3,
  } Alignment;

public:
  explicit ViewBuilder(Widget *a_window);
  virtual ~ViewBuilder();

  virtual void add_column(int a_count);
  virtual void add_sub_column(int a_column_index, int a_row_index, int a_count);

  virtual void add_row(int a_count);
  virtual void add_sub_row(int a_index, int a_count);

  virtual int add_button(int a_parent_id, const std::string &a_label) const;
  virtual Widget *button(int a_id);

  virtual Widget *ui() const;

private:
  class PrivateViewBuilder;
  PrivateViewBuilder *const d;
};
}
#endif // VIEWBUILDER_H
