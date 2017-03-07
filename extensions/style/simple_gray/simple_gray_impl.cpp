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
#include "simple_gray.h"
#include "simple_gray_impl.h"
#include <ck_widget.h>
#include <ck_config.h>

static void destroyStyle(cherry_kit::style *style) { delete style; }

cherry_kit::style_ref SimpleGrayInterface::style() {
  m_instence_ref = cherry_kit::style_ref(new SimpleGrayStyle());
  return m_instence_ref;
}

#ifdef __QT4_TOOLKIT__
Q_EXPORT_PLUGIN2(simplegray, SimpleGrayInterface)
#endif
