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
#ifndef P_DESKTOP_VIEWPORT_H
#define P_DESKTOP_VIEWPORT_H

#include <QObject>
#include <desktopactivity.h>
#include <style.h>
#include <plexy_core_exports.h>

#ifndef PLATFORM_GRAPHICS_SCENE
#define PLATFORM_GRAPHICS_SCENE
typedef void PlatformGraphicsScene;
#endif

namespace PlexyDesk {

class ControllerInterface;

typedef QSharedPointer<ControllerInterface> ControllerPtr;

class CORE_DECL_EXPORT DesktopViewport : public QObject {
  Q_OBJECT

public:
  typedef enum {
    kCenterOnViewport,
    kCenterOnViewportLeft,
    kCenterOnViewportRight,
    kCenterOnViewportTop,
    kCenterOnViewportBottom
  } ViewportLocation;

  virtual void setScene(PlatformGraphicsScene *scene) = 0;
  /**
      * @brief
      *
      * @param name
      * @return ControllerPtr
      */
  virtual ControllerPtr controller(const QString &name) = 0;
  /**
      * @brief
      *
      * @return QStringList
      */
  virtual QStringList currentDesktopControllers() const = 0;
  /**
      * @brief
      *
      * @param controllerName
      * @param key
      * @param value
      */
  virtual void updateSessionValue(const QString &controllerName,
                                  const QString &key, const QString &value) = 0;
  /**
      * @brief
      *
      * @param activity
      */
  virtual void addActivity(DesktopActivityPtr activity) = 0;

  virtual DesktopActivityPtr createActivity(const QString &activity,
                                            const QString &title,
                                            const QPointF &pos,
                                            const QRectF &rect,
                                            const QVariantMap &dataItem) = 0;
  virtual QObject *workspace() = 0;

  virtual QRectF geometry() const = 0;

  virtual QPointF center(const QRectF &viewGeometry,
                         const ViewportLocation &loc = kCenterOnViewport) const;

Q_SIGNALS:
  void controllerAdded(const QString &name);

public Q_SLOTS:
  /**
      * @brief
      *
      * @param widget
      */
  virtual void addWidgetToView(Widget *widget) = 0;
  /**
      * @brief
      *
      * @param
      */
  virtual void onWidgetClosed(PlexyDesk::Widget *) = 0;

protected:
  explicit DesktopViewport(QObject *parent = 0);
};
}
#endif // DESKTOP_VIEWPORT_H
