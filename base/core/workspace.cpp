#include "workspace.h"
#include <QDebug>

namespace PlexyDesk {

class WorkSpace::PrivateWorkSpace {
public:
  PrivateWorkSpace() {}
  ~PrivateWorkSpace() {}
};

WorkSpace::WorkSpace(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), d(new PrivateWorkSpace) {}

WorkSpace::~WorkSpace() { delete d; }

void WorkSpace::addSpace(const QString &label) {
  qDebug() << Q_FUNC_INFO << "Adding New Work Space -> " << label;
}
}
