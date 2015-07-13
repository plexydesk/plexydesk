#include "toolbar.h"

#include <imagebutton.h>
#include <resource_manager.h>
#include <QDesktopWidget>
#include <QGraphicsWidget>
#include <tableview.h>
#include <QGraphicsLinearLayout>

#include <viewbuilder.h>

namespace cherry_kit {
class ToolBar::PrivateToolBar {
public:
  PrivateToolBar() : m_item_count(1) {}
  ~PrivateToolBar() {}

  uint m_item_count;
  QSize m_icon_size;
  QString m_icon_resolution;

  std::vector<std::function<void(const QString &)> > m_action_handler_list;
  HybridLayout *m_layout;
};

ToolBar::ToolBar(Widget *parent)
    : Widget(parent), o_tool_bar(new PrivateToolBar) {
  o_tool_bar->m_layout = new HybridLayout(this);
  o_tool_bar->m_layout->set_content_margin(10, 10, 10, 10);
  o_tool_bar->m_layout->set_horizontal_segment_count(1);
  o_tool_bar->m_layout->set_horizontal_height(0, "100%");
}

ToolBar::~ToolBar() { delete o_tool_bar; }

void ToolBar::add_action(const QString &a_lable, const QString &a_icon,
                         bool a_togle_action) {

  o_tool_bar->m_layout->add_horizontal_segments(0, o_tool_bar->m_item_count);

  cherry_kit::WidgetProperties accept_button_prop;
  accept_button_prop["label"] = "";
  accept_button_prop["icon"] = a_icon.toStdString();

  o_tool_bar->m_layout->add_widget(0, (o_tool_bar->m_item_count - 1),
                                   "image_button", accept_button_prop);
  o_tool_bar->m_item_count += 1;

  update();
}

void ToolBar::insert_widget(Widget *a_widget_ptr) {}

void ToolBar::set_orientation(Qt::Orientation a_orientation) {}

void ToolBar::set_icon_resolution(const QString &a_res) {
  o_tool_bar->m_icon_resolution = a_res;
}

void ToolBar::set_icon_size(const QSize &a_size) {
  o_tool_bar->m_icon_size = a_size * ResourceManager::style()->scale_factor();
}

StylePtr ToolBar::style() const { return ResourceManager::style(); }

void ToolBar::setGeometry(const QRectF &a_rect) {
  o_tool_bar->m_layout->set_geometry(0, 0, a_rect.width(), a_rect.height());
  Widget::setGeometry(a_rect);
}

QRectF ToolBar::contents_geometry() const { return geometry(); }

QRectF ToolBar::frame_geometry() const { return geometry(); }

QSizeF ToolBar::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return geometry().size();
}

void
ToolBar::on_item_activated(std::function<void(const QString &)> a_handler) {
  o_tool_bar->m_action_handler_list.push_back(a_handler);
}

void ToolBar::paint_view(QPainter *painter, const QRectF &exposeRect) {
  painter->fillRect(exposeRect, QColor("#f0f0f0"));
}

void ToolBar::tool_button_press_handler(const Widget *a_widget_ptr) {
  if (a_widget_ptr) {
    const ImageButton *button = qobject_cast<const ImageButton *>(a_widget_ptr);
    if (button) {

      std::for_each(std::begin(o_tool_bar->m_action_handler_list),
                    std::end(o_tool_bar->m_action_handler_list),
                    [&](std::function<void(const QString &)> a_func) {
        if (a_func)
          a_func(button->label());
      });
    }
  }
}
}
