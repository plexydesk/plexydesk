#ifndef DESKTOPWINDOW_H
#define DESKTOPWINDOW_H

#include <ck_window.h>

class QPainter;

class desktop_window : public cherry_kit::window {
public:
  typedef enum {
    kNone,
    kStreachBackground,
    kTileBackground,
    kFrameBackground,
    kCenterFocusBackground,
    kFitWidthBackground,
    kFitHeightBackground
  } DesktopScalingMode;

  desktop_window();
  virtual ~desktop_window();

  void set_background(const std::string &a_image_name);
  void set_background(const QImage &a_image_name);
  void reset_window_background();

  bool is_seamless();
  void set_seamless(bool mode = true);

  void set_desktop_scale_mode(DesktopScalingMode a_desktop_mode = kNone);
  DesktopScalingMode desktop_scale_mode() const;

protected:
  void paint_view(QPainter *a_ctx, const QRectF &a_rect);

private:
  class PrivateDesktopWindow;
  PrivateDesktopWindow *o_window;
};

#endif // DESKTOPWINDOW_H
