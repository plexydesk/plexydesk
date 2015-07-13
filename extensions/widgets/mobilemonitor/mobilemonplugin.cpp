/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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

#include "mobilemonplugin.h"
#include <ck_extension_manager.h>
#include <desktopck_widget.h>
#include <QDeclarativeContext>
#include <QMimeData>
#include <QFileInfo>

#include "note.h"

#ifdef Q_WS_QPA
#include <bb/cascades/NavigationPane>
#endif

class MobileMonController::MobileMonControllerPrivate {
public:
  MobileMonControllerPrivate() {};
  ~MobileMonControllerPrivate() {}

  QHash<QString, Note *> mNoteData;
};

MobileMonController::MobileMonController(QObject * /*object*/)
    : mFrameParentitem(0), d(new MobileMonControllerPrivate) {
  if (connectToDataSource("bbconnengine")) {
    connect(dataSource(), SIGNAL(sourceUpdated(QVariantMap)), this,
            SLOT(onDataUpdated(QVariantMap)));
  }
}

MobileMonController::~MobileMonController() {
  if (mFrameParentitem) {
    delete mFrameParentitem;
  }

  qDeleteAll(mPinWidgets);

  delete d;
}

void MobileMonController::init() {
  /*
  if (mFrameParentitem == NULL) {
      mFrameParentitem = new PlexyNowWidget(QRectF(0.0, 0.0, 320.0, 108.0));
      mFrameParentitem->setWidgetFlag(PlexyDesk::DesktopWidget::BACKGROUND,
  true);
      mFrameParentitem->setWidgetFlag(PlexyDesk::DesktopWidget::TOP_LEVEL_WINDOW);

      mFrameParentitem->setController(this);
      mFrameParentitem->setLabelName("Ping Widget");

      connect (mFrameParentitem, SIGNAL(approvedToken(QString)), this,
  SLOT(setApprovedToken(QString)));

      sp
  }
  */
}

void MobileMonController::revokeSession(const QVariantMap &args) {
  mFrameParentitem->setContentImage(args["src"].toString());
}

void MobileMonController::handleDropEvent(
    PlexyDesk::AbstractDesktopWidget * /*widget*/, QDropEvent *event) {
  if (event->mimeData()->urls().count() >= 0) {
    const QString droppedFile =
        event->mimeData()->urls().value(0).toLocalFile();

    QFileInfo info(droppedFile);
    QPixmap droppedPixmap(droppedFile);

    if (!info.isDir() && !droppedPixmap.isNull()) {
      mFrameParentitem->setContentImage(droppedPixmap);

      if (viewport()) {
        PlexyDesk::AbstractDesktopView *view =
            qobject_cast<PlexyDesk::AbstractDesktopView *>(viewport());
        if (view) {
          view->sessionDataForController(controllerName(), "src", droppedFile);
        }
      } else {
        qDebug() << Q_FUNC_INFO << "Saving session Failed";
      }
    }
  }
}

void MobileMonController::setViewRect(const QRectF &rect) {
  if (mFrameParentitem) {
    mFrameParentitem->setPos(rect.x(), rect.y());
  } else {
    mFrameParentitem = new PlexyNowWidget(QRectF(0.0, 0.0, 320.0, 108.0));
    mFrameParentitem->setWidgetFlag(PlexyDesk::DesktopWidget::BACKGROUND, true);
    mFrameParentitem->setController(this);

    connect(mFrameParentitem, SIGNAL(approvedToken(QString)), this,
            SLOT(setApprovedToken(QString)));
  }
}

QStringList MobileMonController::actions() const {
  QStringList rv;

  rv << "Sync";
  rv << "Add Note";
  rv << "Set Pin";

  return rv;
}

void MobileMonController::requestAction(const QString &actionName,
                                        const QVariantMap &args) {
  if (actionName == "Set Pin") {
    PlexyNowWidget *pingWidget =
        new PlexyNowWidget(QRectF(0.0, 0.0, 320.0, 108.0));
    pingWidget->setController(this);
    pingWidget->setLabelName("Ping Widget");
    pingWidget->setWidgetFlag(PlexyDesk::DesktopWidget::BACKGROUND, true);
    pingWidget->setWidgetFlag(PlexyDesk::DesktopWidget::WINDOW, false);
    pingWidget->setWidgetFlag(PlexyDesk::DesktopWidget::TOP_LEVEL_WINDOW);

    if (viewport()) {
      PlexyDesk::AbstractDesktopView *view =
          qobject_cast<PlexyDesk::AbstractDesktopView *>(viewport());

      if (view) {
        view->scene()->addItem(pingWidget);
      }
      pingWidget->show();
    }

    connect(pingWidget, SIGNAL(approvedToken(QString)), this,
            SLOT(setApprovedToken(QString)));
    mPinWidgets.append(pingWidget);

    float xpos = 0.0;

    if (viewport()) {
      PlexyDesk::AbstractDesktopView *view =
          qobject_cast<PlexyDesk::AbstractDesktopView *>(viewport());
      if (view) {
        xpos = (view->width() - pingWidget->boundingRect().width()) / 2;
      }
    }
    pingWidget->setPos(xpos, 10.0);
  }

  if (actionName == "Add Note") {
    Note *note = new Note(QRect(0.0, 0.0, 240.0, 240.0));
    note->setTitle("title");
    note->setNoteContent("content");

    d->mNoteData["title"] = note;

    if (viewport()) {
      PlexyDesk::AbstractDesktopView *view =
          qobject_cast<PlexyDesk::AbstractDesktopView *>(viewport());
      if (view) {
        view->addWidgetToView(note);
      }
    }

    qDebug() << Q_FUNC_INFO << "Add Note Request";
  }
}

bool
MobileMonController::deleteWidget(PlexyDesk::AbstractDesktopWidget *widget) {
  return 0;
}

void MobileMonController::setApprovedToken(const QString &token) {
  if (dataSource()) {
    PlexyDesk::DataSource *source = dataSource();

    QVariant arg;
    QVariantMap dataMap;

    dataMap["key"] = token;

    arg.setValue(dataMap);

    source->setArguments(arg);
  }
}
