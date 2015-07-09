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
#include "nlp.h"
#include <plexyconfig.h>

class NLPControllerImpl::PrivateNLP {
public:
  PrivateNLP() {}
  ~PrivateNLP() {}
};

NLPControllerImpl::NLPControllerImpl(QObject *object)
    : CherryKit::ViewController(object), o_view_controller(new PrivateNLP) {}

NLPControllerImpl::~NLPControllerImpl() { delete o_view_controller; }

void NLPControllerImpl::init() {}

void NLPControllerImpl::session_data_available(
    const ck::sync_object &a_session_root) {}

void NLPControllerImpl::submit_session_data(ck::sync_object *a_obj) {}

void NLPControllerImpl::set_view_rect(const QRectF &rect) {}

bool NLPControllerImpl::remove_widget(CherryKit::Widget *widget) {
  return false;
}

CherryKit::ActionList NLPControllerImpl::actions() const {
  return CherryKit::ActionList();
}

void NLPControllerImpl::request_action(const QString &actionName,
                                       const QVariantMap &args) {}

QString NLPControllerImpl::icon() const { return QString(); }
