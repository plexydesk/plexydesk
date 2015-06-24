#include "toolbar.h"

#include <imagebutton.h>
#include <resource_manager.h>
#include <QDesktopWidget>
#include <QGraphicsWidget>
#include <tableview.h>
#include <QGraphicsLinearLayout>

namespace UIKit {
class ToolBar::PrivateToolBar {
public:
  PrivateToolBar() {}
  ~PrivateToolBar() {}

  QRectF contentGeometry() const;
  QRectF frameGeometry() const;

  uint mButtonCount;
  QGraphicsWidget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;
  QSize m_icon_size;
  QString m_icon_resolution;

  std::vector<std::function<void (const QString &)>> m_action_handler_list;
};

ToolBar::ToolBar(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateToolBar) {
  d->mButtonCount = 0;
  d->m_icon_resolution = "mdpi";
  d->m_icon_size = QSize(24, 24);

  d->mLayoutBase = new QGraphicsWidget(this);
  d->mLayout = new QGraphicsLinearLayout();
  d->mLayoutBase->setLayout(d->mLayout);
  d->mLayout->setContentsMargins(5.0, 5.0, 5.0, 5.0);
  d->mLayout->setSpacing(5.0);
  d->mLayout->setOrientation(Qt::Horizontal);

  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable, false);
}

ToolBar::~ToolBar() { delete d; }

void ToolBar::add_action(const QString &a_lable, const QString &a_icon,
                         bool a_togle_action) {
  ImageButton *button = new ImageButton(d->mLayoutBase);

  float _button_size = d->m_icon_size.width();

  if (d->mButtonCount != 0) {
    d->mLayout->addStretch();
  }

  button->set_lable(a_lable);
  button->set_pixmap(UIKit::ResourceManager::instance()->drawable(a_icon + ".png",
                                                        d->m_icon_resolution));
  button->setGeometry(QRectF(QPointF(), d->m_icon_size));
  button->setMinimumSize(d->m_icon_size);

  button->on_input_event([this](Widget::InputEvent aEventType,
                                const Widget *aWidget) {
    if (aEventType == kMouseReleaseEvent) {
      tool_button_press_handler(aWidget);
    }
  });

  d->mLayout->addItem(button);
  d->mButtonCount = d->mButtonCount + 1;

  d->mLayout->invalidate();
  d->mLayout->updateGeometry();
  d->mLayout->activate();
}

void ToolBar::insert_widget(Widget *a_widget_ptr) {
  if (d->mLayout->count() != 0) {
    //d->mLayout->addStretch();
  }

  d->mLayout->addItem(a_widget_ptr);
  d->mLayout->invalidate();
  d->mLayout->updateGeometry();
  d->mLayout->activate();
  update();
}

void ToolBar::set_orientation(Qt::Orientation a_orientation) {
  d->mLayout->setOrientation(a_orientation);
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

QRectF ToolBar::contents_geometry() const { return d->contentGeometry(); }

QRectF ToolBar::frame_geometry() const { return d->frameGeometry(); }

QSizeF ToolBar::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
  return d->frameGeometry().size();
}

void ToolBar::on_item_activated(std::function<void (const QString &)> a_handler)
{
  d->m_action_handler_list.push_back(a_handler);
}

void ToolBar::paint_view(QPainter *painter, const QRectF &exposeRect) {
  /*
  QPen pen;
  painter->save();
  painter->fillRect(frame_geometry(), QColor("#000000"));
  painter->restore();
  */
}

void ToolBar::tool_button_press_handler(const Widget *a_widget_ptr) {
  if (a_widget_ptr) {
    const ImageButton *button = qobject_cast<const ImageButton *>(a_widget_ptr);
    if (button) {

      std::for_each(std::begin(d->m_action_handler_list),
                    std::end(d->m_action_handler_list),
                    [&](std::function<void (const QString &)> a_func) {
        if (a_func)
          a_func(button->label());
      });
    }
  }
}

QRectF ToolBar::PrivateToolBar::contentGeometry() const {
  return mLayout->contentsRect();
}

QRectF ToolBar::PrivateToolBar::frameGeometry() const {
  return mLayout->geometry();
}
}
