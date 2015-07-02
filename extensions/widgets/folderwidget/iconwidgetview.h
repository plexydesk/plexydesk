#ifndef ICONWIDGETVIEW_H
#define ICONWIDGETVIEW_H

#include <scrollwidget.h>
#include "folderitem.h"

class IconWidgetView : public CherryKit::Widget {
  Q_OBJECT
public:
  explicit IconWidgetView(Widget *a_parent_ptr = 0);
  virtual ~IconWidgetView();

  void setDirectoryPath(const QString &path);

public
Q_SLOTS:
  void onClicked(FolderItem *item);
  void infoViewClicked();

private:
  // virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  class PrivateIconWidgetView;
  PrivateIconWidgetView *const d;
};

#endif // ICONWIDGETVIEW_H
