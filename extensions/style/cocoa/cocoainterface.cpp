/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include "cocoa.h"
#include "cocoainterface.h"
#include <widget.h>
#include <plexyconfig.h>

static void destroyStyle(UIKit::Style *style) { delete style; }

QSharedPointer<UIKit::Style> CocoaInterface::style() {
  QSharedPointer<UIKit::Style> obj = QSharedPointer<UIKit::Style>(
      (UIKit::Style *)new CocoaStyle(), &destroyStyle);

  return obj;
}

// Q_EXPORT_PLUGIN2(cocoaengine, CocoaInterface)
