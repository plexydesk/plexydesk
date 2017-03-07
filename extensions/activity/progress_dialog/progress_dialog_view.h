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
#ifndef PROGRESSDIALOG_ACTIVITY_H
#define PROGRESSDIALOG_ACTIVITY_H

#include <ck_data_source.h>
#include <ck_window.h>
#include <ck_desktop_dialog.h>

class progress_dialog_view : public cherry_kit::desktop_dialog {
  Q_OBJECT

public:
  progress_dialog_view(QGraphicsObject *object = 0);

  virtual ~progress_dialog_view();

  void create_window();

  cherry_kit::window *dialog_window() const;
  bool purge();

private:
  class PrivateProgressDialog;
  PrivateProgressDialog *const priv;
};

#endif
