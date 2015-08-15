/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef DESKTOPACTIVITY_H
#define DESKTOPACTIVITY_H

#include <QCursor>

#include <ck_widget.h>
#include <ck_window.h>

#include <plexydesk_ui_exports.h>

namespace cherry_kit {

class desktop_controller_interface;
class space;
class widget;
typedef QSharedPointer<desktop_controller_interface> desktop_controller_ref;
typedef std::function<void(const std::string &, const std::string &)>
dialog_message_t;

class DECL_UI_KIT_EXPORT desktop_dialog : public QObject {
  Q_OBJECT
public:
  enum ResultType {
    kActivitySucess,
    mActivityCanceled
  };

  explicit desktop_dialog(QObject *a_parent_ptr = 0);
  virtual ~desktop_dialog();

  virtual void create_window(const QRectF &a_window_geometry,
                             const QString &a_window_title,
                             const QPointF &a_window_pos) = 0;
  virtual window *dialog_window() const = 0;

  virtual void set_activity_attribute(const QString &a_name,
                                      const QVariant &a_data);
  virtual void update_attribute(const QString &a_name, const QVariant &a_data);
  virtual bool has_attribute(const QString &a_arg);
  virtual QVariantMap attributes() const;

  virtual QString error_message() const;
  virtual QVariantMap result() const { return QVariantMap();}

  virtual void exec(const QPointF &a_pos = QCursor::pos());
  virtual void show_activity();
  virtual void hide();

  virtual void set_controller(const desktop_controller_ref &a_controller);
  virtual desktop_controller_ref controller() const;

  virtual void set_viewport(space *a_viewport_ptr);
  virtual space *viewport() const;

  virtual void purge() = 0;

  virtual void on_arguments_updated(std::function<void()> a_handler);

  virtual void
  on_action_completed(std::function<void(const QVariantMap &)> a_handler);

  virtual void
  on_discarded(std::function<void(const desktop_dialog *)> a_handler);

  virtual void on_notify(dialog_message_t callback);

  virtual void discard_activity();

protected:
  virtual void update_action();
  virtual QRectF geometry() const;
  virtual void set_geometry(const QRectF &a_geometry);
  virtual void update_content_geometry(widget *a_widget_ptr);
  virtual void notify_done();
  virtual void notify_message(const std::string &a_key,
                              const std::string &a_value) const;

Q_SIGNALS:
  void canceled();
  void resultsReady();
  void attribute_changed();

private:
  class PrivateDesktopActivity;
  PrivateDesktopActivity *const priv;
};

typedef QSharedPointer<desktop_dialog> desktop_dialog_ref;
}
#endif // DESKTOPACTIVITY_H
