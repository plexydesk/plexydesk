#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <desktopwidget.h>
#include <widget.h>
#include <plexydesk_ui_exports.h>

namespace PlexyDesk {
class DECL_UI_KIT_EXPORT ToolBar : public PlexyDesk::Widget {
  Q_OBJECT

public:
  ToolBar(QGraphicsObject *parent = 0);

  virtual ~ToolBar();

  virtual void addAction(const QString &lable, const QString &icon,
                         bool togleAction = false);

  virtual void insertWidget(UIWidget *widget);

  virtual void setOrientation(Qt::Orientation orientation);

  virtual void setIconResolution(const QString &res);

  virtual void setIconSize(const QSize &size);

  virtual StylePtr style() const;

  // virtual void setGeometry(const QRectF &rect);

  virtual QRectF contentGeometry() const;

  virtual QRectF frameGeometry() const;

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

Q_SIGNALS:
  void action(const QString &actionName);

protected:
  virtual void paintView(QPainter *painter, const QRectF &exposeRect);

private Q_SLOTS:
  void onButtonPressed();

private:
  class PrivateToolBar;
  PrivateToolBar *const d;
};
}

#endif // TOOLBAR_H
