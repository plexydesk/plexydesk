#ifndef CLASSICBACKGROUNDRENDER_H
#define CLASSICBACKGROUNDRENDER_H

#include <plexy.h>

#include <QImage>

#include <widget.h>

class ClassicBackgroundRender : public UI::Widget {
  Q_OBJECT
public:
  typedef enum {
    None,
    Streach,
    Tile,
    Frame,
    CenterFocus,
    FitWidth,
    FitHeight
  } ScalingMode;

  explicit ClassicBackgroundRender(const QRectF &rect,
                                   QGraphicsObject *parent = 0,
                                   const QImage &background_image = QImage());

  void setBackgroundImage(const QString &path);

  void setBackgroundImage(const QImage &img);

  void setBackgroundImage(const QUrl &url);

  void setBackgroundGeometry(const QRectF &rect);

  void setBackgroundMode(ScalingMode mode);

  StylePtr style() const;

  bool isSeamlessModeSet() const;

  void setSeamLessMode(bool value);

protected:
  void paintView(QPainter *painter, const QRectF &rect);
  void paintMinimizedView(QPainter *painter, const QRectF &rect);
  void drawBackroundFrame(QPainter *painter, const QRectF &rect);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
  QImage mBackgroundImage;
  QImage mBackgroundImageWidthScaled;
  QImage mBackgroundImageHeightScaled;
  QRectF mGeometry;
  ScalingMode mScalingMode;
  bool mSeamLessMode;
};

#endif // CLASSICBACKGROUNDRENDER_H
