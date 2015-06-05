#ifndef DESKTOPACTIVITY_H
#define DESKTOPACTIVITY_H

#include <QCursor>
#include <QGraphicsObject>

#include <widget.h>
#include <window.h>

#include <plexydesk_ui_exports.h>

namespace UIKit {

class ViewController;
class Space;
class Widget;
typedef QSharedPointer<ViewController> ViewControllerPtr;

class DECL_UI_KIT_EXPORT DesktopActivity : public QObject {
  Q_OBJECT
public:
  enum ResultType {
    kActivitySucess,
    mActivityCanceled
  };

  explicit DesktopActivity(QGraphicsObject *a_parent_ptr = 0);
  virtual ~DesktopActivity();

  virtual void create_window(const QRectF &a_window_geometry,
                             const QString &a_window_title,
                             const QPointF &a_window_pos) = 0;
  virtual Window *window() const = 0;

  virtual void set_activity_attribute(const QString &a_name,
                                      const QVariant &a_data);
  virtual void update_attribute(const QString &a_name, const QVariant &a_data);
  virtual bool has_attribute(const QString &a_arg);
  virtual QVariantMap attributes() const;

  virtual QString error_message() const;
  virtual QVariantMap result() const = 0;

  virtual void exec(const QPointF &a_pos = QCursor::pos());
  virtual void show_activity();
  virtual void hide();

  virtual void set_controller(const ViewControllerPtr &a_controller);
  virtual ViewControllerPtr controller() const;

  virtual void set_viewport(Space *a_viewport_ptr);
  virtual Space *viewport() const;

  virtual void cleanup() = 0;

  virtual void on_arguments_updated(std::function<void()> a_handler);

  virtual void on_action_completed(
      std::function<void(const QVariantMap &)> a_handler);

  virtual void on_discarded(
      std::function<void(const DesktopActivity *)> a_handler);

  virtual void discard_activity();
protected:
  virtual void update_action();
  virtual QRectF geometry() const;
  virtual void set_geometry(const QRectF &a_geometry);
  virtual void update_content_geometry(Widget *a_widget_ptr);
  virtual void notify_done();
Q_SIGNALS:
  void finished();
  void canceled();
  void discarded();
  void resultsReady();
  void attribute_changed();

private:
  class PrivateDesktopActivity;
  PrivateDesktopActivity *const d;
};

typedef QSharedPointer<DesktopActivity> DesktopActivityPtr;
}
#endif // DESKTOPACTIVITY_H
