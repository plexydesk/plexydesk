#include "ck_resource_manager.h"
#include "ck_fixed_layout.h"
#include <algorithm>
#include <iostream>

#include <ck_button.h>
#include <ck_icon_button.h>
#include <ck_label.h>
#include <ck_text_editor.h>
#include <ck_line_edit.h>
#include <ck_calendar_view.h>
#include <ck_clock_view.h>

#include <ck_dial_view.h>
#include <ck_image_view.h>
#include <ck_widget.h>
#include <ck_item_view.h>

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
  PrivateViewBuilder(widget *a_window)
      : m_row_count(1), m_column_count(1), m_horizontal_padding(1.0f),
        m_verticle_padding(2.0f) {
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
                           const widget_properties_t &a_props,
                           widget_handler_callback_t a_callback);
  widget *add_new_image_button_at(int a_row, int a_col,
                                  const widget_properties_t &a_props,
                                  widget_handler_callback_t a_callback);
  widget *add_new_image_view_at(int a_row, int a_col,
                                const widget_properties_t &a_props,
                                widget_handler_callback_t a_callback);

  void update_image_button_properties(int a_row, int a_col,
                                      const widget_properties_t &a_props);

  widget *add_new_text_edit_at(int a_row, int a_col,
                               const widget_properties_t &a_props,
                               widget_handler_callback_t a_callback);
  widget *add_new_line_edit_at(int a_row, int a_col,
                               const widget_properties_t &a_props,
                               widget_handler_callback_t a_callback);
  widget *add_new_calendar_at(int a_row, int a_col,
                              const widget_properties_t &a_props,
                              widget_handler_callback_t a_callback);
  widget *add_new_clock_at(int a_row, int a_col,
                           const widget_properties_t &a_props,
                           widget_handler_callback_t a_callback);
  widget *add_new_dial_at(int a_row, int a_col,
                          const widget_properties_t &a_props,
                          widget_handler_callback_t a_callback);

  widget *add_new_model_view_at(int a_row, int a_col,
                                const widget_properties_t &a_props,
                                widget_handler_callback_t a_callback);

  widget *add_new_grid_model_view_at(int a_row, int a_col,
                                     const widget_properties_t &a_props,
                                     widget_handler_callback_t a_callback);

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
  float m_horizontal_padding;
  float m_verticle_padding;
};

fixed_layout::fixed_layout(widget *a_window)
    : priv(new PrivateViewBuilder(a_window)) {}

fixed_layout::~fixed_layout() { delete priv; }

void fixed_layout::set_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  priv->m_grid_geometry = QRectF(a_x, a_y, a_width, a_height);
  priv->m_content_frame->set_geometry(priv->m_grid_geometry);
  set_content_margin(priv->m_left_margine, priv->m_right_margine,
                     priv->m_top_margine, priv->m_bottom_margine);
  layout();
}

void fixed_layout::set_content_margin(float a_left, float a_right, float a_top,
                                      float a_bottom) {
  priv->m_left_margine = a_left;
  priv->m_right_margine = a_right;
  priv->m_top_margine = a_top;
  priv->m_bottom_margine = a_bottom;

  priv->m_grid_geometry.setWidth(priv->m_grid_geometry.width() -
                                 ((a_left + a_right) / 2));
  priv->m_grid_geometry.setHeight(priv->m_grid_geometry.height() -
                                  ((a_top + a_bottom) / 2));

  priv->m_content_frame->setPos(a_left, a_top);

  layout();
}

void fixed_layout::set_verticle_spacing(float a_value) {
  priv->m_verticle_padding = a_value;
}

void fixed_layout::set_horizontal_spacing(float a_value) {
  priv->m_horizontal_padding = a_value;
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
      widget->set_geometry(QRectF(0, 0, calculate_cell_width(row, i),
                                  calculate_cell_height(row, i)));
      widget->setPos(get_x(row, i), get_y(row, 0));
    }
  }
}

void fixed_layout::add_column(int a_count) { priv->m_column_count = a_count; }

void fixed_layout::add_rows(int a_row_count) {
  priv->m_row_count = a_row_count;
  for (int i = 0; i < a_row_count; i++) {
    // if (!(d->m_row_data[i] > 1))
    //  continue;
    add_segments(i, 1);
  }
}

void fixed_layout::add_segments(int a_index, int a_count) {
  priv->m_row_data[a_index] = a_count;

  for (int i = 0; i < a_count; i++) {
    set_segment_width(a_index, i, "auto");
  }
}

void fixed_layout::set_row_height(int a_row, const std::string &a_height) {
  priv->m_row_height_data[a_row] = a_height;
}

void fixed_layout::set_segment_width(int a_row, int a_column,
                                     const std::string &a_width) {
  GridPos pos = std::make_pair(a_row, a_column);
  priv->m_column_width_data[pos] = a_width;
}

widget *fixed_layout::viewport() const { return priv->m_content_frame; }

widget *fixed_layout::at(int a_row, int a_column) {
  GridPos pos = std::make_pair(a_row, a_column);
  return priv->m_widget_grid[pos];
}

widget *fixed_layout::add_new_widget_at(int a_col, int a_row,
                                        const widget_properties_t &a_props,
                                        widget_handler_callback_t a_callback) {
  cherry_kit::widget *ck_widget = new widget(priv->m_content_frame);
  GridPos pos(a_col, a_row);
  priv->m_widget_grid[pos] = ck_widget;
  priv->m_ui_type_dict[pos] = kWidget;

  return ck_widget;
}

widget *fixed_layout::add_new_button_at(int a_row, int a_col,
                                        const widget_properties_t &a_props,
                                        widget_handler_callback_t a_callback) {
  button *btn = new button(priv->m_content_frame);
  GridPos pos(a_row, a_col);

  priv->m_widget_grid[pos] = btn;
  priv->m_ui_type_dict[pos] = kButton;

  // set view properties.
  btn->set_label(QString::fromStdString(a_props.at("label")));
  btn->set_size(QSizeF(priv->calculate_cell_width(a_row, a_col),
                       priv->calculate_cell_height(a_row, a_col)));

  layout();

  btn->on_click([=]() {
    if (a_callback)
      a_callback();
  });

  return btn;
}

widget *fixed_layout::add_new_label_at(int a_col, int a_row,
                                       const widget_properties_t &a_props,
                                       widget_handler_callback_t a_callback) {
  return 0;
}

void fixed_layout::layout() { priv->layout(); }

widget *fixed_layout::add_widget(int a_row, int a_column,
                                 const std::string &a_widget,
                                 const widget_properties_t &a_properties,
                                 widget_handler_callback_t a_callback) {
  widget *rv = 0;

  switch (priv->m_ui_dict[a_widget]) {
  case kWidget:
    rv = add_new_widget_at(a_row, a_column, a_properties, a_callback);
    break;
  case kButton:
    rv = add_new_button_at(a_row, a_column, a_properties, a_callback);
    break;
  case kLabel:
    rv = priv->add_new_label_at(a_row, a_column, a_properties, a_callback);
    break;
  case kTextEdit:
    rv = priv->add_new_text_edit_at(a_row, a_column, a_properties, a_callback);
    break;
  case kImageButton:
    rv = priv->add_new_image_button_at(a_row, a_column, a_properties,
                                       a_callback);
    break;
  case kImageView:
    rv = priv->add_new_image_view_at(a_row, a_column, a_properties, a_callback);
    break;
  case kCalendar:
    rv = priv->add_new_calendar_at(a_row, a_column, a_properties, a_callback);
    break;
  case kClock:
    rv = priv->add_new_clock_at(a_row, a_column, a_properties, a_callback);
    break;
  case kDialView:
    rv = priv->add_new_dial_at(a_row, a_column, a_properties, a_callback);
    break;
  case kModelView:
    rv = priv->add_new_model_view_at(a_row, a_column, a_properties, a_callback);
    break;
  case kGridModelView:
    rv = priv->add_new_grid_model_view_at(a_row, a_column, a_properties,
                                          a_callback);
    break;
  case kLineEdit:
    rv = priv->add_new_line_edit_at(a_row, a_column, a_properties, a_callback);
    break;
  default:
    rv = 0;
  }

  return rv;
}

void fixed_layout::update_property(int a_row, int a_column,
                                   const widget_properties_t &a_properties) {
  GridPos pos = std::make_pair(a_row, a_column);

  if (priv->m_ui_type_dict.find(pos) == priv->m_ui_type_dict.end()) {
    qWarning() << Q_FUNC_INFO << "Error: No widget at index : ";
    return;
  }

  switch (priv->m_ui_type_dict[pos]) {
  case kImageButton:
    priv->update_image_button_properties(a_row, a_column, a_properties);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Widget type";
  }
}

void fixed_layout::PrivateViewBuilder::build_ui_map() {
  m_ui_dict["widget"] = fixed_layout::kWidget;
  m_ui_dict["button"] = fixed_layout::kButton;
  m_ui_dict["image_button"] = fixed_layout::kImageButton;
  m_ui_dict["image_view"] = fixed_layout::kImageView;
  m_ui_dict["label"] = fixed_layout::kLabel;
  m_ui_dict["line_edit"] = fixed_layout::kLineEdit;
  m_ui_dict["text_edit"] = fixed_layout::kTextEdit;
  m_ui_dict["progress_bar"] = fixed_layout::kProgressBar;
  m_ui_dict["model_view"] = fixed_layout::kModelView;
  m_ui_dict["grid_model_view"] = fixed_layout::kGridModelView;
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

      max_height = (max_height > widget->contents_geometry().height())
                       ? max_height
                       : widget->contents_geometry().height();
    }
    return max_height - m_verticle_padding;
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

  return rv - m_verticle_padding;
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
    float _height = (calculate_cell_height(i, 0) + m_verticle_padding);
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
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  cherry_kit::label *ck_label = new label(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = ck_label;
  m_ui_type_dict[pos] = kLabel;

  ck_label->set_text(QString::fromStdString(a_props.at("label")));
  ck_label->set_size(QSizeF(calculate_cell_width(a_row, a_col),
                            calculate_cell_height(a_row, a_col)));
  ck_label->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                calculate_cell_height(a_row, a_col)));
  layout();

  return ck_label;
}

widget *fixed_layout::PrivateViewBuilder::add_new_image_button_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
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

  image_button->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                    calculate_cell_height(a_row, a_col)));
  /*
  image_button->setMinimumSize(QSize(calculate_cell_width(a_row, a_col),
                                     calculate_cell_height(a_row, a_col)));
                                     */
  layout();

  return image_button;
}

widget *fixed_layout::PrivateViewBuilder::add_new_image_view_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  image_view *img_view = new image_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = img_view;
  m_ui_type_dict[pos] = kImageButton;

  img_view->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                calculate_cell_height(a_row, a_col)));
  /*
  img_view->setMinimumSize(QSize(calculate_cell_width(a_row, a_col),
                                 calculate_cell_height(a_row, a_col)));
                                 */
  layout();

  return img_view;
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

  QPixmap pixmap = resource_manager::instance()->drawable(icon, "mdpi");

  image_button->set_lable(text);
  image_button->set_pixmap(pixmap);
}

widget *fixed_layout::PrivateViewBuilder::add_new_text_edit_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  cherry_kit::text_editor *editor =
      new cherry_kit::text_editor(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = editor;
  m_ui_type_dict[pos] = kTextEdit;

  QString text;

  if (a_props.find("text") != a_props.end()) {
    text = QString::fromStdString(a_props.at("text"));
  }

  editor->set_text(text);
  editor->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                              calculate_cell_height(a_row, a_col)));
  layout();

  return editor;
}

widget *fixed_layout::PrivateViewBuilder::add_new_line_edit_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  cherry_kit::line_edit *editor = new cherry_kit::line_edit(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = editor;
  m_ui_type_dict[pos] = kLineEdit;

  QString text;

  if (a_props.find("text") != a_props.end()) {
    text = QString::fromStdString(a_props.at("text"));
  }

  editor->set_text(text);
  editor->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                              calculate_cell_height(a_row, a_col)));
  /*
  editor->setMinimumSize(QSize(calculate_cell_width(a_row, a_col),
                               calculate_cell_height(a_row, a_col)));
                               */
  layout();

  return editor;
}

widget *fixed_layout::PrivateViewBuilder::add_new_calendar_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  calendar_view *calendar = new calendar_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = calendar;
  m_ui_type_dict[pos] = kCalendar;

  calendar->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                calculate_cell_height(a_row, a_col)));
  layout();

  return calendar;
}

widget *fixed_layout::PrivateViewBuilder::add_new_clock_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  clock_view *clock = new clock_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = clock;
  m_ui_type_dict[pos] = kClock;

  clock->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                             calculate_cell_height(a_row, a_col)));
  layout();

  return clock;
}

widget *fixed_layout::PrivateViewBuilder::add_new_dial_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  dial_view *dial_widget = new dial_view(m_content_frame);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = dial_widget;
  m_ui_type_dict[pos] = kClock;

  dial_widget->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                   calculate_cell_height(a_row, a_col)));
  layout();

  return dial_widget;
}

widget *fixed_layout::PrivateViewBuilder::add_new_grid_model_view_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  cherry_kit::item_view *item_view = new cherry_kit::item_view(
      m_content_frame, cherry_kit::item_view::kGridModel);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = item_view; //_widget;
  m_ui_type_dict[pos] = kGridModelView;

  item_view->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                 calculate_cell_height(a_row, a_col)));

  item_view->set_view_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                      calculate_cell_height(a_row, a_col)));

  layout();
  return item_view;
}

widget *fixed_layout::PrivateViewBuilder::add_new_model_view_at(
    int a_row, int a_col, const widget_properties_t &a_props,
    widget_handler_callback_t a_callback) {
  cherry_kit::item_view *item_view = new cherry_kit::item_view(
      m_content_frame, cherry_kit::item_view::kListModel);
  GridPos pos(a_row, a_col);

  m_widget_grid[pos] = item_view; //_widget;
  m_ui_type_dict[pos] = kModelView;

  item_view->set_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                 calculate_cell_height(a_row, a_col)));

  item_view->set_view_geometry(QRectF(0, 0, calculate_cell_width(a_row, a_col),
                                      calculate_cell_height(a_row, a_col)));

  layout();
  return item_view;
}
}
