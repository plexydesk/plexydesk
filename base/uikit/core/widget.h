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
#ifndef PD_CORE_WIDGET_H
#define PD_CORE_WIDGET_H

#include <plexy.h>
#include <style.h>
#include <plexydesk_ui_exports.h>

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>

namespace UIKit
{
/*
 * @brief The Base Class for All types of Desktop Widgets.
 */
class ViewController;

class DECL_UI_KIT_EXPORT Widget : public QGraphicsObject,
  public QGraphicsLayoutItem
{
  Q_OBJECT

  Q_ENUMS(RenderLevel)
  Q_ENUMS(WidgetFlags)
  //Q_PROPERTY(QPointF pos READ pos WRITE setPos)

  Q_INTERFACES(QGraphicsLayoutItem)
public:
  typedef enum {
    kRenderBackground = 1ul << 0,
    kRenderDropShadow = 1ul << 1,
    kDockWindowType = 1ul << 2,
    kTopLevelWindow = 1ul << 3,
    kConvertToWindowType = 1ul << 4,
    kRenderWindowTitle = 1ul << 5
  } WidgetFlags;

  typedef enum {
    kMousePressedEvent = 1ul << 0,
    kMouseReleaseEvent = 1ul << 1,
    kFocusOutEvent = 1ul << 2,
    kFocusInEvent = 1ul << 3,
    kKeyPressEvent = 1ul << 4
  } InputEvent;

  enum RenderLevel { kRenderAtBackgroundLevel, kRenderAtForgroundLevel };

  Widget(QGraphicsObject *a_parent_ptr = 0);
  virtual ~Widget();

  void setGeometry(const QRectF &rect);
  virtual QRectF boundingRect() const;
  virtual void setMinimizedGeometry(const QRectF &rect);
  virtual QRectF minimizedGeometry() const;

  virtual void setController(ViewController *view_controller);
  virtual ViewController *controller() const;

  virtual void setLabelName(const QString &name);
  virtual QString label() const;

  virtual void setWidgetID(unsigned int aID);
  virtual unsigned widgetID() const;

  virtual StylePtr style() const;

  virtual RenderLevel layerType() const;
  virtual void setLayerType(RenderLevel level) const;

  virtual void setWindowFlag(int flags, bool enable = true);

  virtual void onInputEvent(
    std::function<void (InputEvent type, const Widget *ptr)> aCallback);

  virtual void setStyleAttribute(const QString &aKey, QVariant aData);
  virtual QVariant styleAttribute(const QString &aKey);

Q_SIGNALS:
  void clicked();
  void focusLost();

protected:
  virtual void paint_view(QPainter *painter, const QRectF &rect);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &constraint = QSizeF()) const;

  virtual void focusOutEvent(QFocusEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  virtual float scaleFactorForWidth() const;
  virtual float scaleFactorForHeight() const;
  virtual void setChildWidetVisibility(bool show);
private:
  class PrivateAbstractDesktopWidget;
  PrivateAbstractDesktopWidget *const d;
};
} //

#endif
