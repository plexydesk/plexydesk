#include "chooseractiondelegate.h"

#include "chooseritem.h"
#include <themepackloader.h>
#include <QHash>

class ChooserActionDelegate::PrivateChooserActionDelegate {
public:
  PrivateChooserActionDelegate() {}
  ~PrivateChooserActionDelegate() { mData.clear(); }

  QList<TableViewItem *> mListItems;
  QMap<QString, QPixmap> mData;
  QVariantMap m_item_properties;
};

ChooserActionDelegate::ChooserActionDelegate(QGraphicsObject *parent)
    : UI::TableModel(parent), d(new PrivateChooserActionDelegate) {}

ChooserActionDelegate::~ChooserActionDelegate() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

float ChooserActionDelegate::margin() const { return 5.0; }

float ChooserActionDelegate::padding() const { return 10.0; }

float ChooserActionDelegate::leftMargin() const { return 5.0; }

float ChooserActionDelegate::rightMargin() const { return 5.0; }

bool ChooserActionDelegate::init() {
  Q_EMIT updated();

  return true;
}

TableModel::TableRenderMode ChooserActionDelegate::renderType() const {
  return ChooserActionDelegate::kRenderAsGridView;
}

void ChooserActionDelegate::removeAll() {
  d->mData.clear();

  Q_EMIT updated();
}

void ChooserActionDelegate::addDataItem(const QString &label,
                                        const QPixmap pixmap, bool selected,
                                        const QVariantMap &properties) {
  d->mData[label] = pixmap;
  d->m_item_properties[label] = properties;

  GridIcon *_item = new GridIcon(QRectF(0.0, 0.0, 96, 96), GridIcon::Grid, 0);
  _item->setItemProperties(pixmap, label, properties);

  Q_EMIT add(_item);
}
