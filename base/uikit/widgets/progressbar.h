#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>

#include <style.h>
#include <plexydesk_ui_exports.h>
#include <desktopwidget.h>

namespace UI
{

class DECL_UI_KIT_EXPORT ProgressBar : public UIWidget
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  explicit ProgressBar(QGraphicsObject *parent = 0);

  virtual ~ProgressBar();

  virtual void setLabel(const QString &txt);

  virtual QString label() const;

  virtual void setSize(const QSizeF &size);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

  virtual int maxRange();

  virtual int minRange();

public Q_SLOTS:
  void setRange(int min, int max);
  void setValue(int value);
  void onValueChanged(const QVariant &value);

Q_SIGNALS:
  void clicked();
  void contentBoundingRectChaned();

protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private:
  class PrivateProgressBar;
  PrivateProgressBar *const d;
};
}

#endif // PROGRESS_BAR_H
