#include "viewbuilder.h"
#include <algorithm>

namespace UIKit {

class Column {};
class Row {};

typedef std::pair<int, int> GridPos;

class ViewBuilder::PrivateViewBuilder {
public:
  PrivateViewBuilder(Widget *a_window)
      : m_view_parent(a_window), m_row_count(0), m_column_count(0) {
    m_content_frame = new Widget(m_view_parent);
  }

  ~PrivateViewBuilder() {
    if (m_content_frame)
      delete m_content_frame;
  }

  Widget *m_view_parent;
  Widget *m_content_frame;

  int m_row_count;
  int m_column_count;

  std::map<int, int> m_column_data;
  std::map<int, int> m_row_data;

  std::map<GridPos, int> m_grid_data;
};

ViewBuilder::ViewBuilder(Widget *a_window)
    : d(new PrivateViewBuilder(a_window)) {}

ViewBuilder::~ViewBuilder() { delete d; }

void ViewBuilder::add_column(int a_count) { d->m_column_count = a_count; }

void ViewBuilder::add_sub_column(int a_column_index, int a_row_index,
                                 int a_count) {
  if ((a_column_index > d->m_column_count) || (a_column_index < 0))
    return;

  if ((a_row_index > d->m_row_count) || a_row_index < 0)
      return;

  GridPos key_index = std::make_pair(a_column_index, a_row_index);
}

void ViewBuilder::add_row(int a_row_count) {}

void ViewBuilder::add_sub_row(int a_index, int a_count) {}

int ViewBuilder::add_button(int a_parent_id, const std::string &a_label) const {
}

Widget *ViewBuilder::button(int a_id) {}

Widget *ViewBuilder::ui() const { return d->m_content_frame; }
}
