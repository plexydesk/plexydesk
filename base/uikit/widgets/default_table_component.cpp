#include "default_table_component.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include <button.h>
#include <label.h>
#include <imageview.h>
#include <resource_manager.h>

class DefaultTableComponent::PrivateTableComponent {

public:
  PrivateTableComponent() {}
  ~PrivateTableComponent() {}

public:
  UIKit::Button *m_option_button;
  UIKit::Label *m_label_widget;
  UIKit::ImageView *m_image_view_widget;

  QGraphicsWidget *m_layout_base;
  QGraphicsLinearLayout *m_linear_layout;

  QRectF m_current_geometry;
  QString m_current_label_str;
  QPixmap m_icon_pixmap;

  bool m_current_item_selection;
  bool m_current_item_visibility;

  LayoutType m_current_layout_type;
};

DefaultTableComponent::DefaultTableComponent(const QRectF &a_rect,
                                             LayoutType type,
                                             QGraphicsItem *parent)
    : UIKit::TableViewItem(a_rect, parent),
      m_priv_ptr(new PrivateTableComponent) {
  m_priv_ptr->m_current_geometry = a_rect;
  m_priv_ptr->m_current_item_selection = false;
  m_priv_ptr->m_current_item_visibility = true;
  m_priv_ptr->m_current_layout_type = type;

  m_priv_ptr->m_layout_base = new QGraphicsWidget(this);
  m_priv_ptr->m_layout_base->setGeometry(a_rect);

  m_priv_ptr->m_linear_layout =
      new QGraphicsLinearLayout(m_priv_ptr->m_layout_base);

  m_priv_ptr->m_image_view_widget =
      new UIKit::ImageView(m_priv_ptr->m_layout_base);
  m_priv_ptr->m_image_view_widget->set_pixmap(
      UIKit::ResourceManager::instance()->drawable("setup-wizard.png", "hdpi"));

  m_priv_ptr->m_label_widget = new UIKit::Label(m_priv_ptr->m_layout_base);

  m_priv_ptr->m_option_button = new UIKit::Button(m_priv_ptr->m_layout_base);

  connect(m_priv_ptr->m_image_view_widget, SIGNAL(clicked()), this,
          SLOT(onClicked()));
  connect(m_priv_ptr->m_label_widget, SIGNAL(clicked()), this,
          SLOT(onClicked()));
  connect(m_priv_ptr->m_option_button, SIGNAL(clicked()), this,
          SLOT(onClicked()));

  if (type == kGridLayout) {
    m_priv_ptr->m_linear_layout->setOrientation(Qt::Vertical);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_image_view_widget);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_label_widget);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_option_button);
    m_priv_ptr->m_label_widget->set_size(QSize(64, a_rect.height()));
    m_priv_ptr->m_option_button->hide();
  } else {
    m_priv_ptr->m_label_widget->set_size(QSize(150, a_rect.height()));
    // d->m_image_view_widget->setSize(QSizeF(48.0, 48.0));
    m_priv_ptr->m_linear_layout->setOrientation(Qt::Horizontal);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_image_view_widget);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_label_widget);
    m_priv_ptr->m_linear_layout->addItem(m_priv_ptr->m_option_button);
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  setFiltersChildEvents(false);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
}

DefaultTableComponent::~DefaultTableComponent() { delete m_priv_ptr; }

QRectF DefaultTableComponent::boundingRect() const {
  QRectF rect;
  if (m_priv_ptr->m_current_layout_type == kListLayout) {
    rect = QRectF(0.0, 0.0, m_priv_ptr->m_current_geometry.width(), 64);
  } else {
    rect = m_priv_ptr->m_current_geometry;
  }
  return rect;
}

QSizeF DefaultTableComponent::sizeHint(Qt::SizeHint hint,
                                       const QSizeF &a_size) const {
  return boundingRect().size();
}

void DefaultTableComponent::set_selected() {
  m_priv_ptr->m_current_item_selection = true;
  update();
}

void DefaultTableComponent::set_label_visibility(bool a_visible) {
  m_priv_ptr->m_current_item_visibility = a_visible;

  if (a_visible) {
    m_priv_ptr->m_label_widget->show();
  } else {
    m_priv_ptr->m_label_widget->hide();
  }
}

void DefaultTableComponent::clear_selection() {
  m_priv_ptr->m_current_item_selection = false;
}

void DefaultTableComponent::set_label(const QString &a_txt) {
  m_priv_ptr->m_option_button->set_label("+");
  m_priv_ptr->m_current_label_str = a_txt;
  m_priv_ptr->m_label_widget->set_label(a_txt);
  update();
}

void DefaultTableComponent::set_icon(const QPixmap &a_pixmap) {
  m_priv_ptr->m_icon_pixmap = a_pixmap;
  m_priv_ptr->m_image_view_widget->set_pixmap(a_pixmap);
}

QPixmap DefaultTableComponent::icon() { return m_priv_ptr->m_icon_pixmap; }

void DefaultTableComponent::set_selected(bool a_selection) {
  m_priv_ptr->m_current_item_selection = a_selection;
}

QString DefaultTableComponent::label() const {
  return m_priv_ptr->m_current_label_str;
}

QString DefaultTableComponent::name() const {
  return m_priv_ptr->m_current_label_str;
}

void DefaultTableComponent::set_data(const QPixmap &a_pixmap,
                                     const QString &a_label) {
  m_priv_ptr->m_image_view_widget->set_pixmap(a_pixmap);
  m_priv_ptr->m_label_widget->set_label(a_label);
  m_priv_ptr->m_current_label_str = a_label;
}

void DefaultTableComponent::paint(QPainter *painter,
                                  const QStyleOptionGraphicsItem *option,
                                  QWidget *widget) {
  /* Painter settings */
  if (m_priv_ptr->m_current_layout_type == kListLayout ||
      m_priv_ptr->m_current_item_selection) {

    UIKit::StyleFeatures features;

    features.render_state = UIKit::StyleFeatures::kRenderElement;
    features.geometry = boundingRect();
    features.text_data = m_priv_ptr->m_current_label_str;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    if (UIKit::ResourceManager::style()) {
      UIKit::ResourceManager::style()->draw("vertical_list_item", features, painter);
    }

    /*
    QPainterPath path;
    path.addRoundedRect(boundingRect(),4.0, 4.0);
    painter->fillPath(path, QColor(254, 254, 0));
    QTextOption option;
    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    painter->drawText(boundingRect(), d->m_current_label_str, option);
    */
  }
}

void DefaultTableComponent::onClicked() { Q_EMIT clicked(this); }
