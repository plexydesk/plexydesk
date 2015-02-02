#ifndef DESKTOPACTIVITY_H
#define DESKTOPACTIVITY_H

#include <QCursor>
#include <QGraphicsObject>

#include <widget.h>

#include <plexydesk_ui_exports.h>

namespace PlexyDesk {

class ViewController;
class DesktopViewport;
typedef QSharedPointer<ViewController> ControllerPtr;

class DECL_UI_KIT_EXPORT DesktopActivity : public QObject {
  Q_OBJECT
public:
  enum ResultType { kActivitySucess, mActivityCanceled };

  explicit DesktopActivity(QGraphicsObject *parent = 0);

  virtual ~DesktopActivity();

  virtual void createWindow(const QRectF &window_geometry,
                            const QString &window_title,
                            const QPointF &window_pos) = 0;

  virtual Widget *window() const = 0;

  virtual void setActivityAttribute(const QString &name, const QVariant &data);

  virtual void updateAttribute(const QString &name, const QVariant &data);

  virtual bool hasAttribute(const QString &arg);

  virtual QVariantMap attributes() const;

  virtual QString getErrorMessage() const;

  virtual QVariantMap result() const = 0;

  virtual void exec(const QPointF &pos = QCursor::pos());

  virtual void showActivity();

  virtual void hide();

  virtual void setController(const ControllerPtr &controller);

  virtual ControllerPtr controller() const;

  virtual void setViewport(DesktopViewport *viewport);

  virtual DesktopViewport *viewport() const;

  // virtual void cleanup() = 0;
  virtual void cleanup() {};

protected:
  virtual void updateAction();
  virtual void discardActivity();
  virtual QRectF geometry() const;
  virtual void setGeometry(const QRectF &geometry);
  virtual void updateContentGeometry(Widget *widget);
  void setResult(ResultType type, const QVariantMap &data);

Q_SIGNALS:
  void finished();
  void canceled();
  void discarded();
  void resultsReady();
  void attributeChanged();

private:
  class PrivateDesktopActivity;
  PrivateDesktopActivity *const d;
};

typedef QSharedPointer<DesktopActivity> DesktopActivityPtr;
}
#endif // DESKTOPACTIVITY_H
