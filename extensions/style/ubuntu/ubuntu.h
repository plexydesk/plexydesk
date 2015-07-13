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
#ifndef UBUNTU_ACTIVITY_H
#define UBUNTU_ACTIVITY_H

#include <QtCore>

#include <abstractplugininterface.h>
#include <datasource.h>
#include <QtNetwork>
#include <style.h>

using namespace PlexyDesk;

class ubuntuStyle : public PlexyDesk::Style {
  Q_OBJECT

public:
  ubuntuStyle(QObject *object = 0);

  virtual ~ubuntuStyle();

  virtual void paintControlElement(ControlElement element,
                                   const PlexyDesk::StyleFeatures &feature,
                                   QPainter *painter);
  virtual void paintControlElementText(ControlElement element,
                                       const PlexyDesk::StyleFeatures &feature,
                                       const QString &text, QPainter *painter);
  virtual PlexyDesk::StyleFeatures
  controlElementFeatures(ControlElement element = ControlElement());

private:
  void drawPushButton(const StyleFeatures &features, QPainter *painter);
  void drawFrame(const StyleFeatures &features, QPainter *painter);
  void drawPushButtonText(const StyleFeatures &features, const QString &text,
                          QPainter *painter);
  void drawLineEdit(const StyleFeatures &features, QPainter *painter);
  void drawLineEditText(const StyleFeatures &features, const QString &text,
                        QPainter *painter);
  void drawLabelEditText(const StyleFeatures &features, const QString &text,
                         QPainter *painter);
  void drawSeperatorLine(const StyleFeatures &features, QPainter *painter);

  class Privateubuntu;
  Privateubuntu *const d;
};

#endif
