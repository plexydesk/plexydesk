#include "chooseritem.h"

#include <QPainter>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <imagebutton.h>

#include <button.h>
#include <imageview.h>
#include <label.h>
#include <themepackloader.h>

class GridIcon::PrivateChooserItem
{

public:
  PrivateChooserItem() {}
  ~PrivateChooserItem() {}

  // UI::Button *mOptButton;
  QGraphicsWidget *mLayoutBase;
  QGraphicsLinearLayout *mLayout;
  QGraphicsGridLayout *mGridLayout;
  QRectF mBoundingRect;

  QString mLabel;
  // QPixmap mIconImage;
  bool mIsSelected;

  UIKit::Label *mLabelView;
  UIKit::ImageView *mImageView;

  ItemLayout mType;

  QVariantMap m_item_property_map;
};

GridIcon::GridIcon(const QRectF &rect, ItemLayout type, QGraphicsItem *parent)
  : UIKit::TableViewItem(rect, parent), d(new PrivateChooserItem)
{
  d->mBoundingRect = rect;
  d->mIsSelected = false;
  d->mType = type;
  // this->setCacheMode(DeviceCoordinateCache);

  d->mLayoutBase = new QGraphicsWidget(this);
  d->mLayout = new QGraphicsLinearLayout(this);

  d->mImageView = new UIKit::ImageView(d->mLayoutBase);
  d->mImageView->set_pixmap(
    UIKit::Theme::instance()->drawable("setup-wizard.png", "hdpi"));
  d->mLabelView = new UIKit::Label(d->mLayoutBase);
  connect(d->mImageView, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(d->mLabelView, SIGNAL(clicked()), this, SLOT(onClicked()));

  if (type == Grid) {
    d->mLayout->setOrientation(Qt::Vertical);
    d->mImageView->setMinimumSize(QSizeF(72, rect.height() - 32));
    d->mLayout->addItem(d->mImageView);
    d->mLayout->addItem(d->mLabelView);
    d->mLabelView->set_size(QSize(64, 24));
    d->mImageView->setGeometry(
      QRectF(QPointF(), QSizeF(72, rect.height() - 32)));
  } else {
    // d->mLabelView->setSize(QSize(150, rect.height()));
    d->mLayout->setOrientation(Qt::Horizontal);
    d->mLayout->addItem(d->mImageView);
    d->mLayout->addItem(d->mLabelView);
    // d->mLayout->addItem(d->mOptButton);
  }

  d->mLayoutBase->setLayout(d->mLayout);
  d->mLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setFiltersChildEvents(false);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
}

GridIcon::~GridIcon()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QRectF GridIcon::boundingRect() const
{
  QRectF rect;
  if (d->mType == List) {
    rect = QRectF(0.0, 0.0, d->mBoundingRect.width(), 64);
  } else {
    rect = QRectF(0.0, 0.0, 72, 96);
  }
  return rect;
}

QSizeF GridIcon::sizeHint(Qt::SizeHint hint, const QSizeF &size) const
{
  return boundingRect().size();
}

void GridIcon::set_selected()
{
  d->mIsSelected = true;
  update();
}

void GridIcon::clear_selection() { d->mIsSelected = false; }

void GridIcon::setLabel(const QString &txt)
{
  // d->mOptButton->setLabel("+");
  d->mLabel = txt;
  d->mLabelView->set_label(txt);
  update();
}

void GridIcon::setIcon(const QPixmap &pixmap)
{
  d->mImageView->set_pixmap(pixmap);
}

void GridIcon::setDefault(bool selection) { d->mIsSelected = selection; }

QString GridIcon::label() const { return d->mLabel; }

void GridIcon::setItemProperties(const QPixmap &pixmap, const QString &label,
                                 const QVariantMap &prop)
{
  d->mImageView->set_pixmap(pixmap);
  d->mLabelView->set_label(label);
  d->mLabel = label;

  d->m_item_property_map["label"] = label;
  d->m_item_property_map["icon_pixmap"] = pixmap;

  qDebug() << Q_FUNC_INFO << prop;
  foreach(const QString & key, prop.keys()) {
    qDebug() << Q_FUNC_INFO << prop[key];
    d->m_item_property_map[key] = prop[key].toString();
  }

  qDebug() << Q_FUNC_INFO << "END Trace:" << d->m_item_property_map;
}

void GridIcon::setItemProperty(const QString &key, const QVariant &value)
{
  d->m_item_property_map[key] = value;
}

QVariantMap GridIcon::itemProperties() const { return d->m_item_property_map; }

void GridIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
  /* Painter settings */
  if (d->mType == List || d->mIsSelected) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    QPainterPath path;
    path.addRoundedRect(boundingRect(), 4.0, 4.0);
    painter->fillPath(path, QColor(254, 254, 254));
  }

  // painter->drawRect(option->exposedRect);
}

void GridIcon::onClicked()
{
  qDebug() << Q_FUNC_INFO << "Item Clicked";
  Q_EMIT clicked(this);
}
