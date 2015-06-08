/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
*  Authored By  : *
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
#include "template.h"
#include <plexyconfig.h>

class TemplateControllerImpl::PrivateTemplate {
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}
};

TemplateControllerImpl::TemplateControllerImpl(QObject *object)
    : UIKit::ViewController(object), d(new PrivateTemplate) {
}

TemplateControllerImpl::~TemplateControllerImpl() { delete d; }

void TemplateControllerImpl::init() {}

void TemplateControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_session_root) {

}

void TemplateControllerImpl::submit_session_data(
    QuetzalKit::SyncObject *a_obj) {

}

void TemplateControllerImpl::set_view_rect(const QRectF &a_rect) {}

bool TemplateControllerImpl::remove_widget(UIKit::Widget *a_widget_ptr) {
  return false;
}

UIKit::ActionList TemplateControllerImpl::actions() const {
  return UIKit::ActionList();
}

void TemplateControllerImpl::request_action(const QString &a_name,
                                            const QVariantMap &a_args) {}

QString TemplateControllerImpl::icon() const {
  return QString();
}
