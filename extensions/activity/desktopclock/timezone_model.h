#ifndef TIME_ZONE_MODEL_H
#define TIME_ZONE_MODEL_H

#include <tableviewcellinterface.h>

class TimeZoneModel : public UIKit::TableModel
{
  Q_OBJECT
public:
  TimeZoneModel(QGraphicsObject *a_parent_ptr = 0);

  virtual ~TimeZoneModel();

  float margin() const;
  float padding() const;

  virtual float left_margin() const;
  virtual float right_margin() const;

  virtual bool init();

  virtual TableRenderMode render_type() const;

  void insertItem(const QString &label, const QPixmap pixmap,
                  bool selected = false);

  void setLabelVisibility(bool visibility);
  void setCellSize(const QSize &size);
  void clear();
private:
  class PrivateTableDelegate;
  PrivateTableDelegate *const d;
};

#endif // TIME_ZONE_MODEL_H
