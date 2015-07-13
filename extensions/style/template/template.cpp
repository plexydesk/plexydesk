/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include <desktopck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <controllerinterface.h>

class TemplateStyle::PrivateTemplate {
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}
};

TemplateStyle::TemplateStyle(QObject *parent)
    : PlexyDesk::Style(parent), d(new PrivateTemplate) {}

TemplateStyle::~TemplateStyle() { delete d; }

void
TemplateStyle::paintControlElement(PlexyDesk::Style::ControlElement element,
                                   const PlexyDesk::StyleFeatures &feature,
                                   QPainter *painter) {}

void
TemplateStyle::paintControlElementText(PlexyDesk::Style::ControlElement element,
                                       const PlexyDesk::StyleFeatures &feature,
                                       const QString &text, QPainter *painter) {
}

PlexyDesk::StyleFeatures TemplateStyle::controlElementFeatures(
    PlexyDesk::Style::ControlElement element) {}
