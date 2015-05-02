/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : *
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
#include "inputdialogactivity.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsEffect>
#include <QGraphicsBlurEffect>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QGraphicsAnchorLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QLineEdit>

#include <view_controller.h>
#include <style.h>
#include <themepackloader.h>
#include <button.h>
#include <texteditor.h>

class InputDialogActivityData::PrivateInputDialogActivity {
public:
  PrivateInputDialogActivity() {}
  ~PrivateInputDialogActivity() {}

  UIKit::Window *mFrame;
  QGraphicsBlurEffect *mBackgroundEffect;

  /*Widgets*/
  UIKit::Button *mOkButton;
  UIKit::Button *mCancelButton;

  QGraphicsWidget *mLayoutBase;
  QGraphicsWidget *mHLayoutBase;

  QGraphicsLinearLayout *mVLayout;
  QGraphicsLinearLayout *mHLayout;

  QLineEdit *mLineEdit;
  QGraphicsProxyWidget *mLineEditProxy;
  UIKit::TextEditor *mEditor;

  QRectF mBoundingRect;
  QString mCurrentText;
};

InputDialogActivityData::InputDialogActivityData(QGraphicsObject *object)
    : UIKit::DesktopActivity(object), d(new PrivateInputDialogActivity) {
  d->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
}

InputDialogActivityData::~InputDialogActivityData() { delete d; }

void InputDialogActivityData::create_window(const QRectF &window_geometry,
                                            const QString &window_title,
                                            const QPointF &window_pos) {
  if (has_attribute("geometry")) {
    d->mBoundingRect = attributes()["geometry"].toRect();
  } else {
    d->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
  }

  // todo: invoke UI
  d->mFrame = new UIKit::Window();
  d->mFrame->setGeometry(geometry());
  d->mFrame->setVisible(true);
  d->mFrame->set_widget_name("Message Dialog");

  d->mFrame->set_widget_flag(UIKit::Widget::kRenderBackground);
  d->mFrame->set_widget_flag(UIKit::Widget::kConvertToWindowType);
  d->mFrame->set_widget_flag(UIKit::Widget::kRenderDropShadow);

  d->mBackgroundEffect = new QGraphicsBlurEffect(this);
  d->mBackgroundEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  d->mBackgroundEffect->setBlurRadius(0.0);
  d->mBackgroundEffect->setEnabled(true);

  d->mLayoutBase = new QGraphicsWidget(d->mFrame);
  d->mLayoutBase->setPos(0.0, 72.0);
  d->mVLayout = new QGraphicsLinearLayout(d->mLayoutBase);
  d->mVLayout->setOrientation(Qt::Vertical);

  QRectF _layoutRect(0.0, 0.0, geometry().width(), geometry().height() - 64.0);
  d->mVLayout->setGeometry(_layoutRect);
  d->mVLayout->setMaximumHeight(_layoutRect.height());

  d->mHLayoutBase = new QGraphicsWidget(d->mLayoutBase);

  d->mOkButton = new UIKit::Button(d->mHLayoutBase);
  d->mCancelButton = new UIKit::Button(d->mHLayoutBase);

  QRectF _editorRect(0.0, 0.0, geometry().width(),
                     geometry().height() -
                         (96 + d->mOkButton->boundingRect().height()));
  d->mEditor = new UIKit::TextEditor(d->mLayoutBase);
  d->mVLayout->addItem(d->mEditor);

  d->mHLayout = new QGraphicsLinearLayout(d->mHLayoutBase);
  d->mHLayout->setOrientation(Qt::Horizontal);

  d->mOkButton->setLabel(tr("Post"));
  d->mCancelButton->setLabel(tr("Cancel"));

  d->mHLayout->setContentsMargins(25.0, 0.0, 25.0, 25.0);
  d->mVLayout->setContentsMargins(10.0, 10.0, 10.0, 10.0);
  d->mVLayout->addItem(d->mHLayoutBase);
  d->mHLayout->addStretch(1);
  d->mHLayout->addItem(d->mCancelButton);
  d->mHLayout->addItem(d->mOkButton);

  d->mEditor->setFocus();

  connect(d->mCancelButton, SIGNAL(clicked()), this, SIGNAL(canceled()));
  connect(d->mOkButton, SIGNAL(clicked()), this, SLOT(onOkButtonPressed()));

  this->exec();
  show_activity();

  d->mFrame->on_window_discarded([this](UIKit::Window *aWindow) {
    discard_activity();
  });
}

QString InputDialogActivityData::error_message() const { return QString(); }

QVariantMap InputDialogActivityData::activityResult() const {
  QVariantMap rv;
  rv["text"] = QVariant(d->mCurrentText);
  return rv;
}

QRectF InputDialogActivityData::geometry() const { return d->mBoundingRect; }

QVariantMap InputDialogActivityData::result() const { return QVariantMap(); }

UIKit::Window *InputDialogActivityData::window() const { return d->mFrame; }

void InputDialogActivityData::cleanup() {
  if (d->mFrame) {
    delete d->mFrame;
  }

  d->mFrame = 0;
}

void InputDialogActivityData::onWidgetClosed(UIKit::Widget *widget) {
  connect(this, SIGNAL(discarded()), this, SLOT(onMotionAnimFinished()));
  discard_activity();
}

void InputDialogActivityData::onMotionAnimFinished() { Q_EMIT finished(); }

void InputDialogActivityData::onOkButtonPressed() {
  connect(this, SIGNAL(discarded()), this, SLOT(onMotionAnimFinished()));
  d->mCurrentText = d->mEditor->text();
  Q_EMIT finished();
  discard_activity();
}

void InputDialogActivityData::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *widget) {
  /*
  if (d->mStyle) {
      UI::StyleFeatures feature;
      feature.exposeRect = option->exposedRect;
      feature.state = UI::StyleFeatures::SF_FrontView;
      d->mStyle->paintControlElement(UI::Style::CE_Frame, feature,
  painter);
  }
  */
}
