#include "toolbar.h"

#include <imagebutton.h>
#include <resource_manager.h>
#include <QDesktopWidget>
#include <QGraphicsWidget>
#include <tableview.h>
#include <QGraphicsLinearLayout>

#include <viewbuilder.h>

namespace UIKit {
class ToolBar::PrivateToolBar {
public:
  PrivateToolBar() : m_item_count(1) {}
  ~PrivateToolBar() {}

  QRectF contentGeometry() const;
  QRectF frameGeometry() const;

  uint m_item_count;
  QSize m_icon_size;
  QString m_icon_resolution;

  std::vector<std::function<void(const QString &)> > m_action_handler_list;
  ViewBuilder *m_layout;
};

ToolBar::ToolBar(Widget *parent) : Widget(parent), d(new PrivateToolBar) {
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  d->m_layout = new ViewBuilder(this);
  d->m_layout->set_margine(0, 0, 0, 0);
  d->m_layout->set_geometry(0, 0, 32, 32);
  d->m_layout->set_row_count(1);
  d->m_layout->split_row(0, 2);
  d->m_layout->set_row_height(0, "100%");
}

ToolBar::~ToolBar() { delete d; }

void ToolBar::add_action(const QString &a_lable, const QString &a_icon,
                         bool a_togle_action) {

  prepareGeometryChange();
  d->m_layout->split_row(0, d->m_item_count);
  d->m_layout->set_geometry(0, 0, ((32 * (d->m_item_count))), 32);

  UIKit::ViewProperties accept_button_prop;
  accept_button_prop["label"] = a_lable.toStdString();
  accept_button_prop["icon"] = a_icon.toStdString();

  d->m_layout->add_widget(0, (d->m_item_count - 1), "image_button",
                          accept_button_prop);

  d->m_item_count = d->m_item_count + 1;

  setGeometry(d->m_layout->ui()->geometry());
  update();
}

void ToolBar::insert_widget(Widget *a_widget_ptr) {
}

void ToolBar::set_orientation(Qt::Orientation a_orientation) {
}

void ToolBar::set_icon_resolution(const QString &a_res) {
  d->m_icon_resolution = a_res;
}

void ToolBar::set_icon_size(const QSize &a_size) {
  d->m_icon_size = a_size * ResourceManager::style()->scale_factor();
}

StylePtr ToolBar::style() const { return ResourceManager::style(); }

/*
void ToolBar::setGeometry(const QRectF &rect)
{
    qDebug() << Q_FUNC_INFO << rect;

   // d->mLayout->setGeometry(QRectF(0, 0, rect.width(), rect.height() + 400));
    //d->mLayoutBase->setPos(QPointF());
    Widget::setGeometry(rect);
}
*/

QRectF ToolBar::contents_geometry() const {
  return d->m_layout->ui()->geometry();
}

QRectF ToolBar::frame_geometry() const { return d->m_layout->ui()->geometry(); }

QSizeF ToolBar::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return d->m_layout->ui()->geometry().size();
}

void
ToolBar::on_item_activated(std::function<void(const QString &)> a_handler) {
  d->m_action_handler_list.push_back(a_handler);
}

void ToolBar::paint_view(QPainter *painter, const QRectF &exposeRect) {
}

void ToolBar::tool_button_press_handler(const Widget *a_widget_ptr) {
  if (a_widget_ptr) {
    const ImageButton *button = qobject_cast<const ImageButton *>(a_widget_ptr);
    if (button) {

      std::for_each(std::begin(d->m_action_handler_list),
                    std::end(d->m_action_handler_list),
                    [&](std::function<void(const QString &)> a_func) {
        if (a_func)
          a_func(button->label());
      });
    }
  }
}

QRectF ToolBar::PrivateToolBar::contentGeometry() const {
  return m_layout->ui()->geometry();
}

QRectF ToolBar::PrivateToolBar::frameGeometry() const {
  return m_layout->ui()->geometry();
}
}
