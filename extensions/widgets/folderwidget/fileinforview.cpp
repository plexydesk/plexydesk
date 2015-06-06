/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexyplanet.org>
*  Authored By  : Siraj Razick <siraj@plexyplanet.org>
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

#include "fileinforview.h"
#include <button.h>
#include <lineedit.h>
#include <style.h>
#include <windowbutton.h>

// Qt
#include <QDir>
#include <QDebug>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>

class FileInforView::PrivateFileInforView {
public:
  PrivateFileInforView() {}

  ~PrivateFileInforView() {
    if (mGridLayout) {
      delete mGridLayout;
    }
  }

  QFileInfo mFileInfo;
  QIcon mIcon;
  QPropertyAnimation *mSlideAnimation;
  QGraphicsGridLayout *mGridLayout;

  // action buttons.
  QGraphicsWidget *mLayoutBase;
  UIKit::Button *mDeleteButton;
  UIKit::Button *mRenameButton;
  UIKit::WindowButton *mWindowButton;
  // UI::LineEdit *mRnameField;
  QLineEdit *mLineEdit;
  QGraphicsProxyWidget *mLineEditProxy;
  UIKit::Style *mStyle;
};

FileInforView::FileInforView(QGraphicsObject *parent)
    : UIKit::Widget(parent), d(new PrivateFileInforView) {
  this->setFlag(QGraphicsItem::ItemIsMovable, false);
  this->set_widget_flag(UIKit::Widget::kRenderDropShadow, false);

  d->mSlideAnimation = new QPropertyAnimation(this, "pos");
  d->mSlideAnimation->setDuration(500);
  d->mSlideAnimation->setEndValue(QPointF(0.0, 0.0));
  d->mSlideAnimation->setEasingCurve(QEasingCurve::InCirc);

  d->mLayoutBase = new QGraphicsWidget(this);
  d->mGridLayout = new QGraphicsGridLayout(d->mLayoutBase);

  /* Window button */
  d->mWindowButton = new UIKit::WindowButton(this);
  // d->mWindowButton->setPos(rect.width() -
  // (d->mWindowButton->boundingRect().width() + 10.0), 10.0);
  d->mWindowButton->show();
  connect(d->mWindowButton, SIGNAL(clicked()), this,
          SLOT(onCloseButtonClicked()));

  /* Delete Button */
  d->mDeleteButton = new UIKit::Button(d->mLayoutBase);
  d->mDeleteButton->setLabel(tr("Trash"));
  d->mDeleteButton->set_size(QSizeF(100, 25));
  d->mGridLayout->addItem(d->mDeleteButton, 0, 0);

  /* Rename Button */
  d->mRenameButton = new UIKit::Button(d->mLayoutBase);
  d->mRenameButton->setLabel(tr("Rename"));
  d->mRenameButton->set_size(QSizeF(100, 25));
  d->mGridLayout->addItem(d->mRenameButton, 0, 1);

  d->mLineEditProxy = new QGraphicsProxyWidget(d->mLayoutBase);
  d->mLineEdit = new QLineEdit;
  d->mLineEditProxy->setWidget(d->mLineEdit);
  d->mGridLayout->addItem(d->mLineEditProxy, 0, 2);
  d->mLineEditProxy->hide();

  d->mLayoutBase->setLayout(d->mGridLayout);
  d->mLayoutBase->setPos(136.0, 0.0);
  d->mGridLayout->invalidate();

  /* Connect to signals */
  connect(d->mRenameButton, SIGNAL(clicked()), this, SLOT(onClicked()));

  d->mStyle = 0; // new UI::NativeStyle(this);
}

void FileInforView::setFileInfo(const QFileInfo &info) { d->mFileInfo = info; }

void FileInforView::setIcon(const QIcon &icon) { d->mIcon = icon; }

void FileInforView::pop() {
  update();
  show();
  d->mSlideAnimation->setDirection(QAbstractAnimation::Forward);
  d->mSlideAnimation->start();
}

void FileInforView::push() {
  d->mSlideAnimation->setDirection(QAbstractAnimation::Backward);
  d->mSlideAnimation->start();
}

void FileInforView::setSliderPos(const QPointF &start, const QPointF &end) {
  setPos(start);
  d->mSlideAnimation->setStartValue(start);
  d->mSlideAnimation->setEndValue(end);
}

void FileInforView::onClicked() {
  if (d->mLineEditProxy->isVisible()) {
    d->mRenameButton->setLabel(tr("Rename"));
    d->mLineEditProxy->hide();

    QString fileName = QDir::fromNativeSeparators(d->mFileInfo.filePath());
    QString newFileName = QDir::fromNativeSeparators(
        d->mFileInfo.absolutePath() + "/" + d->mLineEdit->text());

    if (!QFile::rename(fileName, newFileName)) {
      qWarning() << "File Rename Failed : " << newFileName;
    }
  } else {
    d->mRenameButton->setLabel(tr("Done"));
    d->mLineEdit->setText(d->mFileInfo.fileName());
    d->mLineEditProxy->show();
  }
}

void FileInforView::onCloseButtonClicked() { push(); }

void FileInforView::paint_view(QPainter *painter, const QRectF &rect) {
  painter->fillRect(rect, QColor(236, 236, 236));

  UIKit::StyleFeatures feature;
  feature.geometry = QRectF(rect.topRight().x(), rect.topRight().y(),
                            rect.topLeft().x(), rect.topLeft().y());

  // d->mStyle->paintControlElement(UI::Style::CE_Seperator, feature,
  // painter);

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QRect iconRect(10.0, 16.0, 16.0, 16.0);
  QRectF labelRectF(0, 0, rect.width(), 24);
  QRectF sizeLabel(42.0, 8.0, 100, 32);

  d->mIcon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);

  QTextOption txtOption;
  txtOption.setAlignment(Qt::AlignCenter);
  txtOption.setWrapMode(QTextOption::WrapAnywhere);
  // painter->drawText(labelRectF, d->mFileInfo.fileName(), txtOption);

  txtOption.setAlignment(Qt::AlignVCenter);
  float fileSize = 0.0;

  if (d->mFileInfo.size() != 0) {
    fileSize = d->mFileInfo.size() / 1024;
  }

  painter->drawText(sizeLabel, QString("Size: %1 KB").arg(fileSize), txtOption);
}

void FileInforView::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  event->accept();
}
