#include "viewbuilder.h"
#include <algorithm>

#include <widget.h>

namespace UIKit {

class Column {};
class Row {};

typedef std::pair<int, int> GridPos;

class ViewBuilder::PrivateViewBuilder {
public:
   PrivateViewBuilder(Widget *a_window)
      : m_row_count(0), m_column_count(0) {
    m_content_frame = new Widget(a_window);
    build_ui_map();
  }

  void build_ui_map();

  float get_layout_width();
  float get_layout_height();

  ~PrivateViewBuilder() {}

  int m_row_count;
  int m_column_count;

  std::map<int, int> m_column_data;
  std::map<int, int> m_row_data;

  std::map<GridPos, Widget *> m_widget_grid;
  std::map<std::string, int> m_ui_dict;

  UIKit::Widget *m_content_frame;
};

ViewBuilder::ViewBuilder(Widget *a_window)
    : d(new PrivateViewBuilder(a_window)) {}

ViewBuilder::~ViewBuilder() { delete d; }

void ViewBuilder::add_column(int a_count) { d->m_column_count = a_count; }

void ViewBuilder::add_sub_column(int a_column_index, int a_row_index,
                                 int a_count) {
}

void ViewBuilder::add_row(int a_row_count) {
  d->m_row_count = a_row_count;
}

void ViewBuilder::add_sub_row(int a_index, int a_count) {
}

Widget *ViewBuilder::ui() const
{
  return d->m_content_frame;
}

void ViewBuilder::add_new_widget_at(int a_col, int a_row,
                                    const ViewProperties &a_props)
{
  Widget *widget = new Widget(d->m_content_frame);
  GridPos pos(a_col, a_row);
  d->m_widget_grid[pos]= widget;
}

void ViewBuilder::add_new_button_at(int a_col, int a_row,
                                    const ViewProperties &a_props) {
}

void ViewBuilder::add_new_label_at(int a_col, int a_row,
                                   const ViewProperties &a_props) {
}

void ViewBuilder::add_widget(int a_col, int a_row,
                             const std::string &a_widget,
                             const ViewProperties &a_properties) {
  switch(d->m_ui_dict[a_widget]) {
    case kWidget:
      add_new_widget_at(a_col, a_row, a_properties);
    break;
    case kButton:
      add_new_widget_at(a_col, a_row, a_properties);
    default:
      return;
  }
}

void ViewBuilder::PrivateViewBuilder::build_ui_map() {
   m_ui_dict["widget"] = ViewBuilder::kWidget;
   m_ui_dict["button"] = ViewBuilder::kButton;
   m_ui_dict["image_button"] = ViewBuilder::kImageButton;
   m_ui_dict["label"] = ViewBuilder::kLabel;
   m_ui_dict["line_edit"] = ViewBuilder::kLineEdit;
   m_ui_dict["text_edit"] = ViewBuilder::kTextEdit;
   m_ui_dict["progress_bar"] = ViewBuilder::kProgressBar;
   m_ui_dict["dial"] = ViewBuilder::kDial;
   m_ui_dict["model_view"] = ViewBuilder::kModelView;
   m_ui_dict["divider"] = ViewBuilder::kDivider;
}

float ViewBuilder::PrivateViewBuilder::get_layout_width() {
  return 0;
}

float ViewBuilder::PrivateViewBuilder::get_layout_height() {
  return 0;
}
}
