#ifndef ICONWIDGETVIEW_H
#define ICONWIDGETVIEW_H

#include <scrollwidget.h>
#include "folderitem.h"

class IconWidgetView : public UI::UIWidget
{
  Q_OBJECT
public:
  explicit IconWidgetView(QGraphicsObject *parent = 0);
  virtual ~IconWidgetView();

  void setDirectoryPath(const QString &path);

public Q_SLOTS:
  void onClicked(FolderItem *item);
  void infoViewClicked();

private:
  // virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  class PrivateIconWidgetView;
  PrivateIconWidgetView *const d;
};

#endif // ICONWIDGETVIEW_H
