#include "default_table_component.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include <button.h>
#include <label.h>
#include <imageview.h>
#include <themepackloader.h>

class DefaultTableComponent::PrivateTableComponent
{

public:
  PrivateTableComponent() {}

  ~PrivateTableComponent() {}

public:
  UI::Button *mOptButton;
  UI::Label *m_label_widget;
  UI::ImageView *m_image_view_widget;

  QGraphicsWidget *m_layout_base;
  QGraphicsLinearLayout *m_linear_layout;

  QRectF m_current_geometry;
  QString m_current_label_str;
  QPixmap m_icon_pixmap;

  bool m_current_item_selection;
  bool m_current_item_visibility;

  LayoutType m_current_layout_type;
};

DefaultTableComponent::DefaultTableComponent(const QRectF &rect,
    LayoutType type,
    QGraphicsItem *parent)
  : UI::TableViewItem(rect, parent), d(new PrivateTableComponent)
{
  d->m_current_geometry = rect;
  d->m_current_item_selection = false;
  d->m_current_item_visibility = true;
  d->m_current_layout_type = type;

  d->m_layout_base = new QGraphicsWidget(this);
  d->m_layout_base->setGeometry(rect);

  d->m_linear_layout = new QGraphicsLinearLayout(d->m_layout_base);

  d->m_image_view_widget = new UI::ImageView(d->m_layout_base);
  d->m_image_view_widget->setPixmap(
    UI::Theme::instance()->drawable("setup-wizard.png", "hdpi"));

  d->m_label_widget = new UI::Label(d->m_layout_base);

  d->mOptButton = new UI::Button(d->m_layout_base);

  connect(d->m_image_view_widget, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(d->m_label_widget, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(d->mOptButton, SIGNAL(clicked()), this, SLOT(onClicked()));

  if (type == kGridLayout) {
    d->m_linear_layout->setOrientation(Qt::Vertical);
    d->m_linear_layout->addItem(d->m_image_view_widget);
    d->m_linear_layout->addItem(d->m_label_widget);
    d->m_linear_layout->addItem(d->mOptButton);
    d->m_label_widget->setSize(QSize(64, rect.height()));
    d->mOptButton->hide();
  } else {
    d->m_label_widget->setSize(QSize(150, rect.height()));
    // d->m_image_view_widget->setSize(QSizeF(48.0, 48.0));
    d->m_linear_layout->setOrientation(Qt::Horizontal);
    d->m_linear_layout->addItem(d->m_image_view_widget);
    d->m_linear_layout->addItem(d->m_label_widget);
    d->m_linear_layout->addItem(d->mOptButton);
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  setFiltersChildEvents(false);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
}

DefaultTableComponent::~DefaultTableComponent() { delete d; }

QRectF DefaultTableComponent::boundingRect() const
{
  QRectF rect;
  if (d->m_current_layout_type == kListLayout) {
    rect = QRectF(0.0, 0.0, d->m_current_geometry.width(), 64);
  } else {
    rect = d->m_current_geometry;
  }
  return rect;
}

QSizeF DefaultTableComponent::sizeHint(Qt::SizeHint hint,
                                       const QSizeF &size) const
{
  return boundingRect().size();
}

void DefaultTableComponent::setSelected()
{
  d->m_current_item_selection = true;
  update();
}

void DefaultTableComponent::setLabelVisibility(bool visible)
{
  d->m_current_item_visibility = visible;

  if (visible) {
    d->m_label_widget->show();
  } else {
    d->m_label_widget->hide();
  }
}

void DefaultTableComponent::clearSelection()
{
  d->m_current_item_selection = false;
}

void DefaultTableComponent::setLabel(const QString &txt)
{
  d->mOptButton->setLabel("+");
  d->m_current_label_str = txt;
  d->m_label_widget->setLabel(txt);
  update();
}

void DefaultTableComponent::setIcon(const QPixmap &pixmap)
{
  d->m_icon_pixmap = pixmap;
  d->m_image_view_widget->setPixmap(pixmap);
}

QPixmap DefaultTableComponent::icon() { return d->m_icon_pixmap; }

void DefaultTableComponent::setSelected(bool selection)
{
  d->m_current_item_selection = selection;
}

QString DefaultTableComponent::label() const { return d->m_current_label_str; }

QString DefaultTableComponent::name() { return d->m_current_label_str; }

void DefaultTableComponent::setData(const QPixmap &pixmap,
                                    const QString &label)
{
  d->m_image_view_widget->setPixmap(pixmap);
  d->m_label_widget->setLabel(label);
  d->m_current_label_str = label;
}

void DefaultTableComponent::paint(QPainter *painter,
                                  const QStyleOptionGraphicsItem *option,
                                  QWidget *widget)
{
  /* Painter settings */
  if (d->m_current_layout_type == kListLayout || d->m_current_item_selection) {

    UI::StyleFeatures features;

    features.render_state = UI::StyleFeatures::kRenderElement;
    features.geometry = boundingRect();
    features.text_data = d->m_current_label_str;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    if (UI::Theme::style()) {
      UI::Theme::style()->draw("vertical_list_item", features, painter);
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
