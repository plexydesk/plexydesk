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
#ifndef UI_STYLE_H
#define UI_STYLE_H

#include <plexydesk_ui_exports.h>
#include <stylefeatures.h>

#include <QPainter>
#include <QObject>

namespace UIKit
{
class Widget;

class DECL_UI_KIT_EXPORT Style
{
public:
  Style();

  virtual ~Style();

  virtual QVariant attribute(const QString &a_type, const QString &a_name) const;

  virtual QVariantMap attribute_map(const QString &a_type) const = 0;

  virtual void draw(const QString &a_type,
                    const StyleFeatures &a_options,
                    QPainter *a_painter_ptr,
                    const Widget *a_widget_ptr = 0) = 0;
};
}

typedef QSharedPointer<UIKit::Style> StylePtr;
#endif // STYLE_H
