#include "timezone_model.h"
#include "default_table_component.h"
#include <themepackloader.h>
#include <QHash>
#include <QTimeZone>

class TimeZoneModel::PrivateTableDelegate {
public:
  PrivateTableDelegate() {}
  ~PrivateTableDelegate() { m_data_map.clear(); }

  QList<UI::TableViewItem *> m_table_view_item_list;
  QMap<QString, QPixmap> m_data_map;
  QSize m_current_item_size;
  bool m_current_item_label_visibility;
};

TimeZoneModel::TimeZoneModel(QGraphicsObject *parent)
    : UI::TableModel(parent), d(new PrivateTableDelegate) {
  d->m_current_item_label_visibility = false;
}

TimeZoneModel::~TimeZoneModel() { delete d; }

float TimeZoneModel::margin() const { return 0.0; }

float TimeZoneModel::padding() const { return 0.0; }

float TimeZoneModel::leftMargin() const { return 0.0; }

float TimeZoneModel::rightMargin() const { return 0.0; }

bool TimeZoneModel::init() {
  /*
  Q_FOREACH(const QByteArray &ids, QTimeZone::availableTimeZoneIds()) {
      QTimeZone zone(ids);
      qDebug() << Q_FUNC_INFO << zone.displayName(QTimeZone::StandardTime) <<
  ":" <<
                  zone.comment() << ":" <<
                  QLocale::countryToString(zone.country());
      insertItem(QLocale::countryToString(zone.country()),
                 UI::Theme::instance()->drawable("clock.png", "mdpi"),
                 false);
  }
  */

  return true;
}

UI::TableModel::TableRenderMode TimeZoneModel::renderType() const {
  return UI::TableModel::kRenderAsListView;
}

void TimeZoneModel::insertItem(const QString &label, const QPixmap pixmap,
                               bool selected) {
  DefaultTableComponent *item = new DefaultTableComponent(
      QRectF(0.0, 0.0, 240, 32), DefaultTableComponent::kListLayout, 0);

  // item->setLabelVisibility(d->m_current_item_label_visibility);
  item->setData(pixmap, label);

  Q_EMIT add(item);
}

void TimeZoneModel::setLabelVisibility(bool visibility) {
  d->m_current_item_label_visibility = visibility;
}

void TimeZoneModel::setCellSize(const QSize &size) {
  d->m_current_item_size = size;
}

void TimeZoneModel::clear() { Q_EMIT cleared(); }
