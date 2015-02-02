#include "datecellfactory.h"
#include "datecell.h"
#include <themepackloader.h>
#include <QHash>

class DateCellFactory::PrivateDateCellFactory {
public:
  PrivateDateCellFactory() {}
  ~PrivateDateCellFactory() { mData.clear(); }

  QList<TableViewItem *> mListItems;
  QMap<QString, QPixmap> mData;
  QSize mCellSize;
  bool mLablelVisibility;
  bool mHeaderMode;
};

DateCellFactory::DateCellFactory(QGraphicsObject *parent)
    : UI::TableModel(parent), d(new PrivateDateCellFactory) {
  /*
  for (int i = 0 ; i < 10; i++) {
      addDataItem(QString("%1").arg(i),
  UI::ThemepackLoader::instance()->drawable("setup-wizard.png", "hdpi"),
  false);
  }
  */

  d->mHeaderMode = false;
  setCellSize(QSize(42, 42));
}

DateCellFactory::~DateCellFactory() { delete d; }

QList<TableViewItem *> DateCellFactory::componentList() {
  QList<TableViewItem *> list;
  Q_FOREACH(TableViewItem * copyItem, d->mListItems) {
    DateCell *item = new DateCell(
        QRectF(0.0, 0.0, d->mCellSize.width(), d->mCellSize.height()),
        DateCell::Grid, 0);
    DateCell *cell = qobject_cast<DateCell *>(copyItem);

    item->setHeaderMode(d->mHeaderMode);
    item->setLabelVisibility(d->mLablelVisibility);
    item->addDataItem(cell->icon(), cell->label());
    list.append(item);
  }

  return list;
}

float DateCellFactory::margin() const { return 0.0; }

float DateCellFactory::padding() const { return 0.0; }

float DateCellFactory::leftMargin() const { return 0.0; }

float DateCellFactory::rightMargin() const { return 0.0; }

bool DateCellFactory::init() {
  Q_EMIT updated();

  return true;
}

TableModel::TableRenderMode DateCellFactory::renderType() const {
  return DateCellFactory::kRenderAsGridView;
}

void DateCellFactory::addDataItem(const QString &label, const QPixmap pixmap,
                                  bool selected) {
  DateCell *item = new DateCell(
      QRectF(0.0, 0.0, d->mCellSize.width(), d->mCellSize.height()),
      DateCell::Grid, 0);

  item->setLabelVisibility(d->mLablelVisibility);
  item->addDataItem(pixmap, label);
  item->setHeaderMode(d->mHeaderMode);
  d->mData[label] = pixmap;

  d->mListItems.append(item);
  Q_EMIT add(item);
}

void DateCellFactory::setLabelVisibility(bool visibility) {
  d->mLablelVisibility = visibility;
}

void DateCellFactory::setHeaderMode(bool mode) { d->mHeaderMode = mode; }

void DateCellFactory::setCellSize(const QSize &size) { d->mCellSize = size; }

UI::TableViewItem *DateCellFactory::itemAt(int i) {
  qDebug() << Q_FUNC_INFO << d->mListItems.count();

  return d->mListItems.at(i);
}
