#include "resource_manager.h"
#include "viewbuilder.h"
#include <algorithm>
#include <iostream>

#include <button.h>
#include <imagebutton.h>
#include <label.h>
#include <texteditor.h>
#include <calendarwidget.h>
#include <clockwidget.h>

#include <dialwidget.h>
#include <widget.h>

namespace CherryKit {

class Column {};
class Row {};

typedef std::pair<int, int> GridPos;

class HybridLayout::PrivateViewBuilder {
  typedef enum {
    kPercentageValue = 1,
    kAutoValue
  } Value;

public:
  PrivateViewBuilder(Widget *a_window) : m_row_count(1), m_column_count(1) {
    m_content_frame = new Widget(a_window);
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

  Widget *add_new_label_at(int a_row, int a_col,
                           const WidgetProperties &a_props);
  Widget *add_new_image_button_at(int a_row, int a_col,
                                  const WidgetProperties &a_props);
  void update_image_button_properties(int a_row, int a_col,
                                      const WidgetProperties &a_props);

  Widget *add_new_text_edit_at(int a_row, int a_col,
                               const WidgetProperties &a_props);
  Widget *add_new_calendar_at(int a_row, int a_col,
                              const WidgetProperties &a_props);
  Widget *add_new_clock_at(int a_row, int a_col,
                           const WidgetProperties &a_props);
  Widget *add_new_dial_at(int a_row, int a_col,
                          const WidgetProperties &a_props);

  void layout();

  // members
  int m_row_count;
  int m_column_count;

  std::map<int, int> m_column_data;
  std::map<int, int> m_row_data;
  std::map<int, std::string> m_row_height_data;
  std::map<GridPos, std::string> m_column_width_data;

  std::map<GridPos, Widget *> m_widget_grid;
  std::map<GridPos, int> m_ui_type_dict;
  std::map<std::string, int> m_ui_dict;

  CherryKit::Widget *m_content_frame;
  QRectF m_grid_geometry;

  float m_left_margine;
  float m_right_margine;
  float m_top_margine;
  float m_bottom_margine;
};

HybridLayout::HybridLayout(Widget *a_window)
    : o_view_builder(new PrivateViewBuilder(a_window)) {}

HybridLayout::~HybridLayout() { delete o_view_builder; }

void HybridLayout::set_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  o_view_builder->m_grid_geometry = QRectF(a_x, a_y, a_width, a_height);
  o_view_builder->m_content_frame->setGeometry(o_view_builder->m_grid_geometry);
  set_content_margin(
      o_view_builder->m_left_margine, o_view_builder->m_right_margine,
      o_view_builder->m_top_margine, o_view_builder->m_bottom_margine);
  layout();
}

void HybridLayout::set_content_margin(float a_left, float a_right, float a_top,
                                      float a_bottom) {
  o_view_builder->m_left_margine = a_left;
  o_view_builder->m_right_margine = a_right;
  o_view_builder->m_top_margine = a_top;
  o_view_builder->m_bottom_margine = a_bottom;

  o_view_builder->m_grid_geometry.setWidth(
      o_view_builder->m_grid_geometry.width() - (a_left + a_right));
  o_view_builder->m_grid_geometry.setHeight(
      o_view_builder->m_grid_geometry.height() - (a_top + a_bottom));

  o_view_builder->m_content_frame->setPos(a_left, a_top);

  layout();
}

void HybridLayout::PrivateViewBuilder::layout() {
  // layout items.
  for (int row = 0; row < m_row_count; row++) {
    int column_count = m_row_data[row];
    for (int i = 0; i < column_count; i++) {
      GridPos pos = std::make_pair(row, i);
      if (!m_widget_grid[pos])
        continue;

      Widget *widget = m_widget_grid[pos];
      widget->setGeometry(QRectF(0, 0, calculate_cell_width(row, i),
                                 calculate_cell_height(row, i)));
      widget->setPos(get_x(row, i), get_y(row, 0));
    }
  }
}

void HybridLayout::add_column(int a_count) {
  o_view_builder->m_column_count = a_count;
}

void HybridLayout::add_rows(int a_row_count) {
  o_view_builder->m_row_count = a_row_count;
  for (int i = 0; i < a_row_count; i++) {
    // if (!(d->m_row_data[i] > 1))
    //  continue;
    add_segments(i, 1);
  }
}

void HybridLayout::add_segments(int a_index, int a_count) {
  o_view_builder->m_row_data[a_index] = a_count;

  for (int i = 0; i < a_count; i++) {
    set_segment_width(a_index, i, "auto");
  }
}

void HybridLayout::set_row_height(int a_row,
                                         const std::string &a_height) {
  o_view_builder->m_row_height_data[a_row] = a_height;
}

void HybridLayout::set_segment_width(int a_row, int a_column,
                                    const std::string &a_width) {
  GridPos pos = std::make_pair(a_row, a_column);
  o_view_builder->m_column_width_data[pos] = a_width;
}

Widget *HybridLayout::viewport() const {
  return o_view_builder->m_content_frame;
}

Widget *HybridLayout::at(int a_row, int a_column) {
  GridPos pos = std::make_pair(a_row, a_column);
  return o_view_builder->m_widget_grid[pos];
}

Widget *HybridLayout::add_new_widget_at(int a_col, int a_row,
                                        const WidgetProperties &a_props) {
  Widget *widget = new Widget(o_view_builder->m_content_frame);
  GridPos pos(a_col, a_row);
  o_view_builder->m_widget_grid[pos] = widget;
  o_view_builder->m_ui_type_dict[pos] = kWidget;

  return widget;
}

Widget *HybridLayout::add_new_button_at(int a_row, int a_col,
                                        const WidgetProperties &a_props) {
  Button *btn = new Button(o_view_builder->m_content_frame);
  GridPos pos(a_row, a_col);

  o_view_builder->m_widget_grid[pos] = btn;
  o_view_builder->m_ui_type_dict[pos] = kButton;

  // set view properties.
  btn->set_label(QString::fromStdString(a_props.at("label")));
  btn->set_size(QSizeF(o_view_builder->calculate_cell_width(a_row, a_col),
                       o_view_builder->calculate_cell_height(a_row, a_col)));

  layout();

  return btn;
}

Widget *HybridLayout::add_new_label_at(int a_col, int a_row,
                                       const WidgetProperties &a_props) {
  return 0;
}

void HybridLayout::layout() { o_view_builder->layout(); }

Widget *HybridLayout::add_widget(int a_row, int a_column,
                                 const std::string &a_widget,
                                 const WidgetProperties &a_properties) {
  Widget *rv = 0;

  switch (o_view_builder->m_ui_dict[a_widget]) {
  case kWidget:
    rv = add_new_widget_at(a_row, a_column, a_properties);
    break;
  case kButton:
    rv = add_new_button_at(a_row, a_column, a_properties);
    break;
  case kLabel:
    rv = o_view_builder->add_new_label_at(a_row, a_column, a_properties);
    break;
  case kTextEdit:
    rv = o_view_builder->add_new_text_edit_at(a_row, a_column, a_properties);
    break;
  case kImageButton:
    rv = o_view_builder->add_new_image_button_at(a_row, a_column, a_properties);
    break;
  case kCalendar:
    rv = o_view_builder->add_new_calendar_at(a_row, a_column, a_properties);
    break;
  case kClock:
    rv = o_view_builder->add_new_clock_at(a_row, a_column, a_properties);
    break;
  case kDialView:
    rv = o_view_builder->add_new_dial_at(a_row, a_column, a_properties);
    break;
  default:
    rv = 0;
  }

  return rv;
}

void HybridLayout::update_property(int a_row, int a_column,
                                   const WidgetProperties &a_properties) {
  GridPos pos = std::make_pair(a_row, a_column);

  if (o_view_builder->m_ui_type_dict.find(pos) ==
      o_view_builder->m_ui_type_dict.end()) {
    qWarning() << Q_FUNC_INFO << "Error: No widget at index : ";
    return;
  }

  switch (o_view_builder->m_ui_type_dict[pos]) {
  case kImageButton:
    o_view_builder->update_image_button_properties(a_row, a_column,
                                                   a_properties);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Widget type";
  }
}

void HybridLayout::PrivateViewBuilder::build_ui_map() {
  m_ui_dict["widget"] = HybridLayout::kWidget;
  m_ui_dict["button"] = HybridLayout::kButton;
  m_ui_dict["image_button"] = HybridLayout::kImageButton;
  m_ui_dict["label"] = HybridLayout::kLabel;
  m_ui_dict["line_edit"] = HybridLayout::kLineEdit;
  m_ui_dict["text_edit"] = HybridLayout::kTextEdit;
  m_ui_dict["progress_bar"] = HybridLayout::kProgressBar;
  m_ui_dict["model_view"] = HybridLayout::kModelView;
  m_ui_dict["divider"] = HybridLayout::kDivider;
  m_ui_dict["calendar"] = HybridLayout::kCalendar;
  m_ui_dict["clock"] = HybridLayout::kClock;
  m_ui_dict["dial"] = HybridLayout::kDialView;
}

float HybridLayout::PrivateViewBuilder::get_layout_width() { return 0; }

float HybridLayout::PrivateViewBuilder::get_layout_height() { return 0; }

float
HybridLayout::PrivateViewBuilder::calculate_cell_width(int a_row,
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
      Widget *widget = m_widget_grid.at(_pos);
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
HybridLayout::PrivateViewBuilder::calculate_cell_height(int a_row,
                                                        int a_column) const {
  if (get_value_type(m_row_height_data.at(a_row)) == kAutoValue) {
    float max_height = 16;

    for (int i = 0; i < m_row_data.at(a_row); i++) {
      GridPos pos = std::make_pair(a_row, i);

      if (m_widget_grid.count(pos) == 0) {
        continue;
      }

      Widget *widget = m_widget_grid.at(pos);

      if (!widget)
        continue;

      max_height = (max_height > widget->boundingRect().height())
                       ? max_height
                       : widget->boundingRect().height();
    }
    return max_height;
  }

  float rv = 0;
  float auto_height = 2;

  for (int i = 0; i < m_row_count; i++) {
    if (get_value_type(m_row_height_data.at(i)) == kAutoValue) {
      auto_height += calculate_cell_height(i, 0);
    }
  }

  rv = ((m_grid_geometry.height() - auto_height) / 100) *
       get_percentage(m_row_height_data.at(a_row));

  return rv;
}

float HybridLayout::PrivateViewBuilder::get_x(int a_row, int a_column) {
  float cell_width = (m_left_margine);

  for (int i = 0; i < a_column; i++) {
    cell_width += calculate_cell_width(a_row, i);
  }
  return cell_width;
}

float HybridLayout::PrivateViewBuilder::get_y(int a_row, int a_column) {
  float cell_height = m_top_margine;

  for (int i = 0; i < a_row; i++) {
    float _height = calculate_cell_height(i, 0);
    cell_height += _height;
  }

  return cell_height;
}

HybridLayout::PrivateViewBuilder::Value
HybridLayout::PrivateViewBuilder::get_value_type(const std::string &a_value)
    const {

  if (a_value.empty())
    return kAutoValue;

  if (a_value.compare("auto") == 0)
    return kAutoValue;

  if (a_value.at(a_value.size() - 1) == '%')
    return kPercentageValue;

  return kAutoValue;
}

float HybridLayout::PrivateViewBuilder::get_percentage(
    const std::string &a_value) const {
  if (get_value_type(a_value) == kAutoValue)
    return 0;

  std::string value_digits = a_value.substr(0, a_value.size() - 1);
  return (float)std::stoi(value_digits);
}

Widget *HybridLayout::PrivateViewBuilder::add_new_label_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  Label *label = new Label(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = label;
  m_ui_type_dict[pos] = kLabel;

  label->set_label(QString::fromStdString(a_props.at("label")));
  label->set_size(QSizeF(calculate_cell_width(a_row, a_col),
                         calculate_cell_height(a_row, a_col)));
  label->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  layout();

  return label;
}

Widget *HybridLayout::PrivateViewBuilder::add_new_image_button_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  ImageButton *image_button = new ImageButton(m_content_frame);
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
  QPixmap pixmap = ResourceManager::instance()->drawable(icon, "hdpi");
  image_button->set_pixmap(pixmap);

  image_button->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                   calculate_cell_height(a_row, a_col)));
  image_button->setMinimumSize(QSize(calculate_cell_width(a_row, a_col),
                                     calculate_cell_height(a_row, a_col)));
  layout();

  return image_button;
}

void HybridLayout::PrivateViewBuilder::update_image_button_properties(
    int a_row, int a_col, const WidgetProperties &a_props) {
  Widget *widget = m_widget_grid[std::make_pair(a_row, a_col)];

  if (!widget) {
    qWarning() << Q_FUNC_INFO << "Invalid Widget at : ";
    return;
  }

  ImageButton *image_button = dynamic_cast<ImageButton *>(widget);

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

  QPixmap pixmap = ResourceManager::instance()->drawable(icon, "hdpi");

  image_button->set_lable(text);
  image_button->set_pixmap(pixmap);
}

Widget *HybridLayout::PrivateViewBuilder::add_new_text_edit_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  TextEditor *editor = new TextEditor(m_content_frame);
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

Widget *HybridLayout::PrivateViewBuilder::add_new_calendar_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  CalendarView *calendar = new CalendarView(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = calendar;
  m_ui_type_dict[pos] = kCalendar;

  calendar->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                               calculate_cell_height(a_row, a_col)));
  layout();

  return calendar;
}

Widget *HybridLayout::PrivateViewBuilder::add_new_clock_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  ClockWidget *clock = new ClockWidget(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = clock;
  m_ui_type_dict[pos] = kClock;

  clock->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  layout();

  return clock;
}

Widget *HybridLayout::PrivateViewBuilder::add_new_dial_at(
    int a_row, int a_col, const WidgetProperties &a_props) {
  DialWidget *dial_widget = new DialWidget(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = dial_widget;
  m_ui_type_dict[pos] = kClock;

  dial_widget->setGeometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  layout();

  return dial_widget;
}
}
