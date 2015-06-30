#ifndef DESKTOPWINDOW_H
#define DESKTOPWINDOW_H

#include <window.h>

class QPainter;

class DesktopWindow : public UIKit::Window {
public:
  DesktopWindow();
  virtual ~DesktopWindow();

  void set_background(const QString &a_image_name);

  void reset_window_background();
protected:
  void paint_view(QPainter *a_ctx, const QRectF &a_rect);

private:
  class PrivateDesktopWindow;
  PrivateDesktopWindow *p_window;
};

#endif // DESKTOPWINDOW_H
