#ifndef CLASSICBACKGROUNDRENDER_H
#define CLASSICBACKGROUNDRENDER_H

#include <QImage>

#include <ck_widget.h>
#include <ck_window.h>

class ClassicBackgroundRender : public cherry_kit::window {
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

  explicit ClassicBackgroundRender(const QRectF &rect, widget *a_parent_ptr = 0,
                                   const QImage &background_image = QImage());

  void setBackgroundImage(const QString &path);

  void setBackgroundImage(const QImage &img);

  void setBackgroundImage(const QUrl &url);

  void setBackgroundGeometry(const QRectF &rect);

  void setBackgroundMode(ScalingMode mode);

  cherry_kit::style_ref style() const;

  bool isSeamlessModeSet() const;

  void setSeamLessMode(bool value);

protected:
  void paint_view(QPainter *painter, const QRectF &rect);
  void paintMinimizedView(QPainter *painter, const QRectF &rect);
  void drawBackroundFrame(QPainter *painter, const QRectF &rect);

private:
  QImage mBackgroundImage;
  QImage mBackgroundImageWidthScaled;
  QImage mBackgroundImageHeightScaled;
  QRectF mGeometry;
  ScalingMode mScalingMode;
  bool mSeamLessMode;
};

#endif // CLASSICBACKGROUNDRENDER_H
