#ifndef DESKTOPWINDOW_H
#define DESKTOPWINDOW_H

#include <window.h>

class QPainter;

class DesktopWindow : public CherryKit::Window {
public:
  DesktopWindow();
  virtual ~DesktopWindow();

  void set_background(const QString &a_image_name);
  void set_background(const QImage &a_image_name);
  void reset_window_background();

  bool is_seamless();
  void set_seamless(bool mode = true);

protected:
  void paint_view(QPainter *a_ctx, const QRectF &a_rect);

private:
  class PrivateDesktopWindow;
  PrivateDesktopWindow *o_window;
};

#endif // DESKTOPWINDOW_H
