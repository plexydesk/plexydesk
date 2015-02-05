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
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>
#include <desktopwidget.h>

class TemplateControllerImpl::PrivateTemplate
{
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}
  QTimer *mTemplate;
};

TemplateControllerImpl::TemplateControllerImpl(QObject *object)
  : PlexyDesk::ControllerInterface(object), d(new PrivateTemplate)
{
  startTimer(1000);
}

TemplateControllerImpl::~TemplateControllerImpl() { delete d; }

PlexyDesk::AbstractDesktopWidget *TemplateControllerImpl::defaultView()
{
  return 0;
}

void TemplateControllerImpl::revokeSession(const QVariantMap &args) {}

void TemplateControllerImpl::setViewRect(const QRectF &rect) {}

bool TemplateControllerImpl::deleteWidet(
  PlexyDesk::AbstractDesktopWidget *widget)
{
  return false;
}

QStringList TemplateControllerImpl::actions() const { return QStringList(); }

void TemplateControllerImpl::requestAction(const QString &actionName,
    const QVariantMap &args) {}

void TemplateControllerImpl::onDataUpdated(const QVariantMap &data) {}
