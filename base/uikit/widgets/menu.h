#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <desktopactivity.h>
#include <desktopwidget.h>

namespace UI {
class Space;

/**
 * @brief Desktop Menu Class to quick popups.
 *
 * This class coverts a given window class into a
 * popup class which will hide when the focus is lost.
 */
class Menu : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Constructs a Menu Instance.
   *
   * @param parent the parent object of the menu
   */
  explicit Menu(QObject *parent = 0);
  /**
   * @brief
   */
  virtual ~Menu();
  /**
   * @brief Sets the main Window of the menu
   *
   * This defines the Window which should popup
   * on request.
   *
   * @param widget a Valid Window class to.
   */
  virtual void setContentWidget(QSharedPointer<UIWidget> widget);
  /**
   * @brief Invoke the menu on a given location
   *
   * Makes the menu popup on a given location on a given
   * screen.
   *
   * @param pos the location in scene coordinates
   */
  virtual void exec(const QPointF &pos);

Q_SIGNALS:
  /**
   * @brief Signal emited when menu gets deactivated
   */
  void deactivated();
  /**
   * @brief Signal emited when menu gets activated
   */
  void activated();

protected Q_SLOTS:
  virtual void onFocusOutEvent();
  virtual void hide();
  virtual void show();

private:
  class PrivateMenu;
  PrivateMenu *const d;
};
}
#endif // MENU_H
