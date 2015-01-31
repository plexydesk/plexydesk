#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QGraphicsView>

namespace PlexyDesk {

class WorkSpace : public QGraphicsView {
  Q_OBJECT
public:
  explicit WorkSpace(QGraphicsScene *scene, QWidget *parent = 0);
  virtual ~WorkSpace();

  virtual void addSpace(const QString &label);

private:
  class PrivateWorkSpace;
  PrivateWorkSpace *const d;
};
}
#endif // WORKSPACE_H
