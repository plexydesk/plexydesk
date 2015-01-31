/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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

#include "desktopbaseui.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <config.h>
#include <plexyconfig.h>
#include <extensionmanager.h>
#include <abstractdesktopview.h>
#include <desktopviewplugin.h>
#include <QFileInfo>
#include <QIcon>
#include <QDir>
#include <QRegion>
//#include <qpa/qplatformnativeinterface.h>
#include <desktopactivity.h>

#if defined(Q_OS_LINUX) // && defined(Q_WS_MAC) ??
#include <qpa/qplatformnativeinterface.h>
#include <QGuiApplication>
#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>
#include <netwm.h>
#endif

using namespace PlexyDesk;

class DesktopBaseUi::DesktopBaseUiPrivate {
public:
  DesktopBaseUiPrivate()
      : mViewPlugin(NULL),
        mDesktopView(NULL),
        mDesktopWidget(NULL),
        mScene(NULL),
        mConfig(NULL) {}
  ~DesktopBaseUiPrivate() {}
  QSharedPointer<DesktopViewPlugin> mViewPlugin;
  AbstractDesktopView *mDesktopView;
  QDesktopWidget *mDesktopWidget;
  QGraphicsScene *mScene;
  PlexyDesk::Config *mConfig;
  QMap<int, PlexyDesk::AbstractDesktopView *> mViewList;
};

DesktopBaseUi::DesktopBaseUi(QWidget *parent)
    : QWidget(parent), d(new DesktopBaseUiPrivate) {

  // setup network cookiejar
  (void)Config::getCookieJar();

  setDesktopView(QLatin1String("plexydesktopview"));

  // set the icon search path: this is a global function.
  QStringList iconSearchPath = QIcon::themeSearchPaths();

  iconSearchPath << QDir::toNativeSeparators(
                        QDir::homePath() +
                        QDir::toNativeSeparators("/.plexydesk/icons/"));

  QIcon::setThemeSearchPaths(iconSearchPath);

  setup();

  connect(d->mDesktopWidget, SIGNAL(resized(int)), this,
          SLOT(screenResized(int)));
}

DesktopBaseUi::~DesktopBaseUi() {
  qDebug() << Q_FUNC_INFO;
  Q_FOREACH(QGraphicsView * view, d->mViewList.values()) {
    if (view)
      delete view;
  }

  d->mViewList.clear();
}

void DesktopBaseUi::setDesktopView(const QString &name) {
  d->mViewPlugin = ExtensionManager::instance()->desktopViewExtension(name);
  // FIX : handle changes
}

void DesktopBaseUi::setup() {
  d->mDesktopWidget = new QDesktopWidget();
  d->mConfig = PlexyDesk::Config::getInstance();

  QGraphicsScene *scene = new QGraphicsScene;
  scene->setBackgroundBrush(Qt::transparent);
  d->mScene = scene;

  if (!d->mViewPlugin) {
    return;
  }

  QObject *viewPlugin = d->mViewPlugin->view(scene);
  PlexyDesk::AbstractDesktopView *view =
      qobject_cast<PlexyDesk::AbstractDesktopView *>(viewPlugin);

  if (!view) {
    return;
  }

  view->resize(desktopRect().size());
  view->move(desktopRect().x(), desktopRect().y());

  view->setSceneRect(desktopRect());

  view->setDragMode(QGraphicsView::RubberBandDrag);

#ifdef Q_WS_QPA
  view->setDragMode(QGraphicsView::ScrollHandDrag);
#endif

#ifdef Q_WS_X11

  QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

  if (native) {
    Display *display = static_cast<Display *>(
        native->nativeResourceForWindow("display", NULL));

    NETWinInfo info(display, view->winId(), RootWindow(display, 0),
                    NET::WMDesktop);
    info.setDesktop(NETWinInfo::OnAllDesktops);
    info.setWindowType(NET::Desktop);

    // QRegion region;

    // XRectangle rec = { 0, 0, static_cast<unsigned
    // short>(0),static_cast<unsigned short>(0) };
    /// Region regi = XCreateRegion();
    /*
    Box box;
    regi->size = 0;
    regi->numRects = 0;
    regi->rects = NULL;
    regi->extents = box;
    */
    // EMPTY_REGION(regi);

    // XShapeCombineRegion(display, view->winId(), ShapeInput, 0, 0, regi,
    // ShapeSet);
  }

#endif
  // view->showLayer(QLatin1String("Widgets"));
  d->mViewList[0] = view;
  QWidget *parentWidget = qobject_cast<QWidget *>(parent());
  if (parentWidget) {
    this->resize(view->size());
    view->setParent(this);

#ifdef Q_OS_MAC
// TODO: until we write our own NSView we do this for mac (issue : 169)
/*- (void)drawRect:(NSRect)rect {
    [[NSColor clearColor] set];
    NSRectFill(rect);
  }
*/
#endif
  }
  view->show();
#ifdef PLEXYNAME
  view->setWindowTitle(QString(PLEXYNAME));
#endif

  view->layout(desktopRect());
}

void DesktopBaseUi::screenResized(int screen) {
  if (!screen)
    return;

  QGraphicsView *view = d->mViewList[screen];
  QRect desktopScreenRect = d->mDesktopWidget->screenGeometry(screen);
#ifdef Q_WS_WIN
  // A 1px hack to make the widget fullscreen and not covering the toolbar on
  // Win
  desktopScreenRect.setHeight(desktopScreenRect.height() - 1);
#endif

  view->resize(desktopScreenRect.size());
}

QRect DesktopBaseUi::desktopRect() const {
  if (d->mDesktopWidget->screenCount() == 1) {
    return d->mDesktopWidget->screenGeometry();
  }

  int total_width = 0;
  int total_height = 0;

  for (int i = 0; i < d->mDesktopWidget->screenCount(); i++) {
    total_width += d->mDesktopWidget->screenGeometry(i).width();
    total_height += d->mDesktopWidget->screenGeometry(i).height();
  }

  return QRect(0, 0, total_width, total_height);
}

QList<AbstractDesktopView *> DesktopBaseUi::views() const {
  return d->mViewList.values();
}
