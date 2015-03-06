#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QGraphicsObject>
#include <widget.h>

#include <functional>

namespace UIKit {
class ModelView : public Widget
{
  Q_OBJECT
public:
  typedef enum {
    kGridModel,
    kListModel,
    kTableModel
  } ModelType;

  ModelView(QGraphicsObject *parent = 0, ModelType aModelType = kListModel);
  virtual ~ModelView();

  virtual void insert(Widget *widget);
  virtual void remove(Widget *widget);
  virtual void clear();

  virtual void setViewGeometry(const QRectF &rect);
  virtual QRectF boundingRect() const;
  virtual void setGeometry(const QRectF &rect);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &constraint) const;

  virtual void onActivated(std::function<void (int index)> aCallback);
protected:
  bool sceneEvent(QEvent *e);
  bool event(QEvent *e);

  void insert_to_list_view(Widget *widget);
  void insert_to_grid_view(Widget *widget);
  void insert_to_table_view(Widget *widget);
private:
  class PrivateModelView;
  PrivateModelView *const d;
};

}
#endif // MODELVIEW_H
