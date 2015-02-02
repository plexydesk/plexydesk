#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QGraphicsView>

#include <space.h>
#include <plexydesk_ui_exports.h>

namespace UI {
typedef QList<Space *> SpacesList;

class DECL_UI_KIT_EXPORT WorkSpace : public QGraphicsView {
  Q_OBJECT
public:
  WorkSpace(QGraphicsScene *scene, QWidget *parent = 0);

  virtual ~WorkSpace();

  virtual void addDefaultController(const QString &name);

  virtual Space *createEmptySpace();

  virtual void removeSpace(Space *space);

  virtual void restoreSession();

  virtual void switchSpace();

  virtual void exposeSpace(uint id);

  virtual Space *exposeNextSpace();

  virtual Space *exposePreviousSpace();

  virtual void exposeSubRegion(const QRectF &rect);

  virtual QRectF workspaceGeometry() const;

  virtual QPixmap previewSpace(Space *space, int scaleFactor = 10);

  virtual SpacesList currentSpaces();

  virtual uint spaceCount() const;

  virtual Space *currentVisibleSpace() const;

  virtual void setAcceleratedRendering(bool on = true);

  virtual bool isAcceleratedRenderingOn() const;

Q_SIGNALS:
  void workspaceChange();

public Q_SLOTS:
  void addSpace();
  void revokeSpace(const QString &name, int id);

protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dropEvent(QDropEvent *event);
  virtual void wheelEvent(QWheelEvent *event);

private:
  class PrivateWorkSpace;
  PrivateWorkSpace *const d;

  void updateSpaceGeometry(Space *space, QRectF _deleted_geometry);
  void saveSpaceRemovalSessionData(QString _space_ref);
  void setWorkspaceGeometry();
};
}
#endif // WORKSPACE_H
