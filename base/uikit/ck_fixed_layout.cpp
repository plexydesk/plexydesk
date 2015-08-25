#include "ck_resource_manager.h"
#include "ck_fixed_layout.h"
#include <algorithm>
#include <iostream>

#include <ck_button.h>
#include <ck_icon_button.h>
#include <ck_label.h>
#include <ck_text_editor.h>
#include <ck_calendar_view.h>
#include <ck_clock_view.h>

#include <ck_dial_view.h>
#include <ck_widget.h>

namespace cherry_kit {

class Column {};
class Row {};

typedef std::pair<int, int> GridPos;

class fixed_layout::PrivateViewBuilder {
  typedef enum {
    kPercentageValue = 1,
    kAutoValue
  } Value;

public:
  PrivateViewBuilder(widget *a_window) : m_row_count(1), m_column_count(1) {
    m_content_frame = new widget(a_window);
    build_ui_map();
  }

  void build_ui_map();

  float get_layout_width();
  float get_layout_height();

  ~PrivateViewBuilder() {}

  float calculate_cell_width(int a_row, int a_column) const;
  float calculate_cell_height(int a_row, int a_column) const;

  float get_x(int a_row, int a_column);
  float get_y(int a_row, int a_column);

  Value get_value_type(const std::string &a_value) const;
  float get_percentage(const std::string &a_value) const;

  widget *add_new_label_at(int a_row, int a_col,
                           const widget_properties_t &a_props);
  widget *add_new_image_button_at(int a_row, int a_col,
                                  const widget_properties_t &a_props);
  void update_image_button_properties(int a_row, int a_col,
                                      const widget_properties_t &a_props);

  widget *add_new_text_edit_at(int a_row, int a_col,
                               const widget_properties_t &a_props);
  widget *add_new_calendar_at(int a_row, int a_col,
                              const widget_properties_t &a_props);
  widget *add_new_clock_at(int a_row, int a_col,
                           const widget_properties_t &a_props);
  widget *add_new_dial_at(int a_row, int a_col,
                          const widget_properties_t &a_props);

  void layout();

  // members
  int m_row_count;
  int m_column_count;

  std::map<int, int> m_column_data;
  std::map<int, int> m_row_data;
  std::map<int, std::string> m_row_height_data;
  std::map<GridPos, std::string> m_column_width_data;

  std::map<GridPos, widget *> m_widget_grid;
  std::map<GridPos, int> m_ui_type_dict;
  std::map<std::string, int> m_ui_dict;

  cherry_kit::widget *m_content_frame;
  QRectF m_grid_geometry;

  float m_left_margine;
  float m_right_margine;
  float m_top_margine;
  float m_bottom_margine;
};

fixed_layout::fixed_layout(widget *a_window)
    : o_fixed_layout(new PrivateViewBuilder(a_window)) {}

fixed_layout::~fixed_layout() { delete o_fixed_layout; }

void fixed_layout::set_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  o_fixed_layout->m_grid_geometry = QRectF(a_x, a_y, a_width, a_height);
  o_fixed_layout->m_content_frame->setGeometry(o_fixed_layout->m_grid_geometry);
  set_content_margin(
      o_fixed_layout->m_left_margine, o_fixed_layout->m_right_margine,
      o_fixed_layout->m_top_margine, o_fixed_layout->m_bottom_margine);
  layout();
}

void fixed_layout::set_content_margin(float a_left, float a_right, float a_top,
                                      float a_bottom) {
  o_fixed_layout->m_left_margine = a_left;
  o_fixed_layout->m_right_margine = a_right;
  o_fixed_layout->m_top_margine = a_top;
  o_fixed_layout->m_bottom_margine = a_bottom;

  o_fixed_layout->m_grid_geometry.setWidth(
      o_fixed_layout->m_grid_geometry.width() - (a_left + a_right));
  o_fixed_layout->m_grid_geometry.setHeight(
      o_fixed_layout->m_grid_geometry.height() - (a_top + a_bottom));

  o_fixed_layout->m_content_frame->setPos(a_left, a_top);

  layout();
}

void fixed_layout::PrivateViewBuilder::layout() {
  // layout items.
  for (int row = 0; row < m_row_count; row++) {
    int column_count = m_row_data[row];
    for (int i = 0; i < column_count; i++) {
      GridPos pos = std::make_pair(row, i);
      if (!m_widget_grid[pos])
        continue;

      widget *widget = m_widget_grid[pos];
      widget->setGeometry(QRectF(0, 0, calculate_cell_width(row, i),
                                 calculate_cell_height(row, i)));
      widget->setPos(get_x(row, i), get_y(row, 0));
    }
  }
}

void fixed_layout::add_column(int a_count) {
  o_fixed_layout->m_column_count = a_count;
}

void fixed_layout::add_rows(int a_row_count) {
  o_fixed_layout->m_row_count = a_row_count;
  for (int i = 0; i < a_row_count; i++) {
    // if (!(d->m_row_data[i] > 1))
    //  continue;
    add_segments(i, 1);
  }
}

void fixed_layout::add_segments(int a_index, int a_count) {
  o_fixed_layout->m_row_data[a_index] = a_count;

  for (int i = 0; i < a_count; i++) {
    set_segment_width(a_index, i, "auto");
  }
}

void fixed_layout::set_row_height(int a_row, const std::string &a_height) {
  o_fixed_layout->m_row_height_data[a_row] = a_height;
}

void fixed_layout::set_segment_width(int a_row, int a_column,
                                     const std::string &a_width) {
  GridPos pos = std::make_pair(a_row, a_column);
  o_fixed_layout->m_column_width_data[pos] = a_width;
}

widget *fixed_layout::viewport() const {
  return o_fixed_layout->m_content_frame;
}

widget *fixed_layout::at(int a_row, int a_column) {
  GridPos pos = std::make_pair(a_row, a_column);
  return o_fixed_layout->m_widget_grid[pos];
}

widget *fixed_layout::add_new_widget_at(int a_col, int a_row,
                                        const widget_properties_t &a_props) {
  cherry_kit::widget *ck_widget = new widget(o_fixed_layout->m_content_frame);
  GridPos pos(a_col, a_row);
  o_fixed_layout->m_widget_grid[pos] = ck_widget;
  o_fixed_layout->m_ui_type_dict[pos] = kWidget;

  return ck_widget;
}

widget *fixed_layout::add_new_button_at(int a_row, int a_col,
                                        const widget_properties_t &a_props) {
  button *btn = new button(o_fixed_layout->m_content_frame);
  GridPos pos(a_row, a_col);

  o_fixed_layout->m_widget_grid[pos] = btn;
  o_fixed_layout->m_ui_type_dict[pos] = kButton;

  // set view properties.
  btn->set_label(QString::fromStdString(a_props.at("label")));
  btn->set_size(QSizeF(o_fixed_layout->calculate_cell_width(a_row, a_col),
                       o_fixed_layout->calculate_cell_height(a_row, a_col)));

  layout();

  return btn;
}

widget *fixed_layout::add_new_label_at(int a_col, int a_row,
                                       const widget_properties_t &a_props) {
  return 0;
}

void fixed_layout::layout() { o_fixed_layout->layout(); }

widget *fixed_layout::add_widget(int a_row, int a_column,
                                 const std::string &a_widget,
                                 const widget_properties_t &a_properties) {
  widget *rv = 0;

  switch (o_fixed_layout->m_ui_dict[a_widget]) {
  case kWidget:
    rv = add_new_widget_at(a_row, a_column, a_properties);
    break;
  case kButton:
    rv = add_new_button_at(a_row, a_column, a_properties);
    break;
  case kLabel:
    rv = o_fixed_layout->add_new_label_at(a_row, a_column, a_properties);
    break;
  case kTextEdit:
    rv = o_fixed_layout->add_new_text_edit_at(a_row, a_column, a_properties);
    break;
  case kImageButton:
    rv = o_fixed_layout->add_new_image_button_at(a_row, a_column, a_properties);
    break;
  case kCalendar:
    rv = o_fixed_layout->add_new_calendar_at(a_row, a_column, a_properties);
    break;
  case kClock:
    rv = o_fixed_layout->add_new_clock_at(a_row, a_column, a_properties);
    break;
  case kDialView:
    rv = o_fixed_layout->add_new_dial_at(a_row, a_column, a_properties);
    break;
  default:
    rv = 0;
  }

  return rv;
}

void fixed_layout::update_property(int a_row, int a_column,
                                   const widget_properties_t &a_properties) {
  GridPos pos = std::make_pair(a_row, a_column);

  if (o_fixed_layout->m_ui_type_dict.find(pos) ==
      o_fixed_layout->m_ui_type_dict.end()) {
    qWarning() << Q_FUNC_INFO << "Error: No widget at index : ";
    return;
  }

  switch (o_fixed_layout->m_ui_type_dict[pos]) {
  case kImageButton:
    o_fixed_layout->update_image_button_properties(a_row, a_column,
                                                   a_properties);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Widget type";
  }
}

void fixed_layout::PrivateViewBuilder::build_ui_map() {
  m_ui_dict["widget"] = fixed_layout::kWidget;
  m_ui_dict["button"] = fixed_layout::kButton;
  m_ui_dict["image_button"] = fixed_layout::kImageButton;
  m_ui_dict["label"] = fixed_layout::kLabel;
  m_ui_dict["line_edit"] = fixed_layout::kLineEdit;
  m_ui_dict["text_edit"] = fixed_layout::kTextEdit;
  m_ui_dict["progress_bar"] = fixed_layout::kProgressBar;
  m_ui_dict["model_view"] = fixed_layout::kModelView;
  m_ui_dict["divider"] = fixed_layout::kDivider;
  m_ui_dict["calendar"] = fixed_layout::kCalendar;
  m_ui_dict["clock"] = fixed_layout::kClock;
  m_ui_dict["dial"] = fixed_layout::kDialView;
}

float fixed_layout::PrivateViewBuilder::get_layout_width() { return 0; }

float fixed_layout::PrivateViewBuilder::get_layout_height() { return 0; }

float
fixed_layout::PrivateViewBuilder::calculate_cell_width(int a_row,
                                                       int a_column) const {

  int column_count = m_row_data.at(a_row);
  GridPos pos = std::make_pair(a_row, a_column);

  if (!column_count)
    return m_grid_geometry.width();

  if (get_value_type(m_column_width_data.at(pos)) == kAutoValue) {

    return (m_grid_geometry.width() / column_count);
  }

  int auto_width = (m_left_margine + m_right_margine);

  for (int i = 0; i < column_count; i++) {
    GridPos _pos = std::make_pair(a_row, i);
    if (get_value_type(m_column_width_data.at(_pos)) == kAutoValue) {
      widget *widget = m_widget_grid.at(_pos);
      if (!widget)
        continue;

      auto_width += widget->geometry().width();
    }
  }

  int width_value = get_percentage(m_column_width_data.at(pos));
  float rv = ((m_grid_geometry.width() - auto_width) / 100) * width_value;

  return rv;
}

float
fixed_layout::PrivateViewBuilder::calculate_cell_height(int a_row,
                                                        int a_column) const {
  if (get_value_type(m_row_height_data.at(a_row)) == kAutoValue) {
    float max_height = 32;

    for (int i = 0; i < m_row_data.at(a_row); i++) {
      GridPos pos = std::make_pair(a_row, i);

      if (m_widget_grid.count(pos) == 0) {
        continue;
      }

      widget *widget = m_widget_grid.at(pos);

      if (!widget)
        continue;

      max_height = (max_height > widget->boundingRect().height())
                       ? max_height
                       : widget->boundingRect().height();
    }
    return max_height;
  }

  float rv = 0;
  float auto_height = 4;

  for (int i = 0; i < m_row_count; i++) {
    if (get_value_type(m_row_height_data.at(i)) == kAutoValue) {
      auto_height += calculate_cell_height(i, 0);
    }
  }

  rv = ((m_grid_geometry.height() - auto_height) / 100) *
       get_percentage(m_row_height_data.at(a_row));

  return rv;
}

float fixed_layout::PrivateViewBuilder::get_x(int a_row, int a_column) {
  float cell_width = (m_left_margine);

  for (int i = 0; i < a_column; i++) {
    cell_width += calculate_cell_width(a_row, i);
  }
  return cell_width;
}

float fixed_layout::PrivateViewBuilder::get_y(int a_row, int a_column) {
  float cell_height = m_top_margine;

  for (int i = 0; i < a_row; i++) {
    float _height = calculate_cell_height(i, 0);
    cell_height += _height;
  }

  return cell_height;
}

fixed_layout::PrivateViewBuilder::Value
fixed_layout::PrivateViewBuilder::get_value_type(const std::string &a_value)
    const {

  if (a_value.empty())
    return kAutoValue;

  if (a_value.compare("auto") == 0)
    return kAutoValue;

  if (a_value.at(a_value.size() - 1) == '%')
    return kPercentageValue;

  return kAutoValue;
}

float fixed_layout::PrivateViewBuilder::get_percentage(
    const std::string &a_value) const {
  if (get_value_type(a_value) == kAutoValue)
    return 0;

  std::string value_digits = a_value.substr(0, a_value.size() - 1);
  return (float)std::stoi(value_digits);
}

widget *fixed_layout::PrivateViewBuilder::add_new_label_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  cherry_kit::label *ck_label = new label(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = ck_label;
  m_ui_type_dict[pos] = kLabel;

  ck_label->set_text(QString::fromStdString(a_props.at("label")));
  ck_label->set_size(QSizeF(calculate_cell_width(a_row, a_col),
                         calculate_cell_height(a_row, a_col)));
  ck_label->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  layout();

  return ck_label;
}

widget *fixed_layout::PrivateViewBuilder::add_new_image_button_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  icon_button *image_button = new icon_button(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = image_button;
  m_ui_type_dict[pos] = kImageButton;

  QString text;
  QString icon;

  if (a_props.find("label") != a_props.end()) {
    text = QString::fromStdString(a_props.at("label"));
  }

  if (a_props.find("icon") != a_props.end()) {
    icon = QString::fromStdString(a_props.at("icon"));
  }

  image_button->set_lable(text);
  QPixmap pixmap = resource_manager::instance()->drawable(icon, "mdpi");
  image_button->set_pixmap(pixmap);

  image_button->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                   calculate_cell_height(a_row, a_col)));
  image_button->setMinimumSize(QSize(calculate_cell_width(a_row, a_col),
                                     calculate_cell_height(a_row, a_col)));
  layout();

  return image_button;
}

void fixed_layout::PrivateViewBuilder::update_image_button_properties(
    int a_row, int a_col, const widget_properties_t &a_props) {
  widget *widget = m_widget_grid[std::make_pair(a_row, a_col)];

  if (!widget) {
    qWarning() << Q_FUNC_INFO << "Invalid Widget at : ";
    return;
  }

  icon_button *image_button = dynamic_cast<icon_button *>(widget);

  if (!image_button) {
    qWarning() << Q_FUNC_INFO << "Invalid Widget Casting : ";
    return;
  }

  QString text;
  QString icon;

  if (a_props.find("label") != a_props.end()) {
    text = QString::fromStdString(a_props.at("label"));
  }

  if (a_props.find("icon") != a_props.end()) {
    icon = QString::fromStdString(a_props.at("icon"));
  }

  QPixmap pixmap = resource_manager::instance()->drawable(icon, "hdpi");

  image_button->set_lable(text);
  image_button->set_pixmap(pixmap);
}

widget *fixed_layout::PrivateViewBuilder::add_new_text_edit_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  text_editor *editor = new text_editor(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = editor;
  m_ui_type_dict[pos] = kTextEdit;

  QString text;

  if (a_props.find("text") != a_props.end()) {
    text = QString::fromStdString(a_props.at("text"));
  }

  editor->set_text(text);
  editor->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                             calculate_cell_height(a_row, a_col)));
  layout();

  return editor;
}

widget *fixed_layout::PrivateViewBuilder::add_new_calendar_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  calendar_view *calendar = new calendar_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = calendar;
  m_ui_type_dict[pos] = kCalendar;

  calendar->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                               calculate_cell_height(a_row, a_col)));
  layout();

  return calendar;
}

widget *fixed_layout::PrivateViewBuilder::add_new_clock_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  clock_view *clock = new clock_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = clock;
  m_ui_type_dict[pos] = kClock;

  clock->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  layout();

  return clock;
}

widget *fixed_layout::PrivateViewBuilder::add_new_dial_at(
    int a_row, int a_col, const widget_properties_t &a_props) {
  dial_view *dial_widget = new dial_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = dial_widget;
  m_ui_type_dict[pos] = kClock;

  dial_widget->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                  calculate_cell_height(a_row, a_col)));
  layout();

  return dial_widget;
}
}
