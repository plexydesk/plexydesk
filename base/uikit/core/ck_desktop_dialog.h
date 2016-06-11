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

#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

namespace cherry_kit {

class desktop_controller_interface;
class space;
class widget;
typedef std::shared_ptr<desktop_controller_interface> desktop_controller_ref;
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

  virtual void create_window() = 0;
  virtual window *dialog_window() const = 0;

  /* discard */
  virtual void on_arguments_updated(std::function<void()> a_handler);

  /* to keep */
  virtual void hide();
  virtual void show_activity();

  virtual void set_controller(const desktop_controller_ref &a_controller);
  virtual desktop_controller_ref controller() const;

  virtual void set_viewport(space *a_viewport_ptr);
  virtual space *viewport() const;

  virtual bool purge() = 0;
  virtual bool busy();

  virtual void
  on_action_completed(std::function<void(const QVariantMap &)> a_handler);

  virtual void
  on_discarded(std::function<void(desktop_dialog *)> a_handler);

  virtual void on_notify(dialog_message_t callback);

  virtual void discard_activity();
  void notify_exit();

protected:
  virtual void notify_done();
  virtual void notify_message(const std::string &a_key,
                              const std::string &a_value);
private:
  class PrivateDesktopActivity;
  PrivateDesktopActivity *const priv;
};

typedef std::shared_ptr<desktop_dialog> desktop_dialog_ref;
}
#endif // DESKTOPACTIVITY_H
