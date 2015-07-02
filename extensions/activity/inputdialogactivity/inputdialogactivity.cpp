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
#include <resource_manager.h>
#include <button.h>
#include <texteditor.h>

class InputDialogActivityData::PrivateInputDialogActivity {
public:
  PrivateInputDialogActivity() {}
  ~PrivateInputDialogActivity() {}

  CherryKit::Window *mFrame;
  QGraphicsBlurEffect *mBackgroundEffect;

  /*Widgets*/
  CherryKit::Button *mOkButton;
  CherryKit::Button *mCancelButton;

  CherryKit::Widget *mLayoutBase;
  CherryKit::Widget *mHLayoutBase;

  QGraphicsLinearLayout *mVLayout;
  QGraphicsLinearLayout *mHLayout;

  QLineEdit *mLineEdit;
  QGraphicsProxyWidget *mLineEditProxy;
  CherryKit::TextEditor *mEditor;

  QRectF mBoundingRect;
  QString mCurrentText;
};

InputDialogActivityData::InputDialogActivityData(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateInputDialogActivity) {
  o_desktop_activity->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
}

InputDialogActivityData::~InputDialogActivityData() { delete o_desktop_activity; }

void InputDialogActivityData::create_window(const QRectF &window_geometry,
                                            const QString &window_title,
                                            const QPointF &window_pos) {
  if (has_attribute("geometry")) {
    o_desktop_activity->mBoundingRect = attributes()["geometry"].toRect();
  } else {
    o_desktop_activity->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
  }

  // todo: invoke UI
  o_desktop_activity->mFrame = new CherryKit::Window();
  o_desktop_activity->mFrame->setGeometry(geometry());
  o_desktop_activity->mFrame->setVisible(true);
  o_desktop_activity->mFrame->set_widget_name("Message Dialog");

  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderBackground);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kConvertToWindowType);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderDropShadow);

  o_desktop_activity->mBackgroundEffect = new QGraphicsBlurEffect(this);
  o_desktop_activity->mBackgroundEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  o_desktop_activity->mBackgroundEffect->setBlurRadius(0.0);
  o_desktop_activity->mBackgroundEffect->setEnabled(true);

  o_desktop_activity->mLayoutBase = new CherryKit::Widget(o_desktop_activity->mFrame);
  o_desktop_activity->mLayoutBase->setPos(0.0, 72.0);
  o_desktop_activity->mVLayout = new QGraphicsLinearLayout(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mVLayout->setOrientation(Qt::Vertical);

  QRectF _layoutRect(0.0, 0.0, geometry().width(), geometry().height() - 64.0);
  o_desktop_activity->mVLayout->setGeometry(_layoutRect);
  o_desktop_activity->mVLayout->setMaximumHeight(_layoutRect.height());

  o_desktop_activity->mHLayoutBase = new CherryKit::Widget(o_desktop_activity->mLayoutBase);

  o_desktop_activity->mOkButton = new CherryKit::Button(o_desktop_activity->mHLayoutBase);
  o_desktop_activity->mCancelButton = new CherryKit::Button(o_desktop_activity->mHLayoutBase);

  QRectF _editorRect(0.0, 0.0, geometry().width(),
                     geometry().height() -
                         (96 + o_desktop_activity->mOkButton->boundingRect().height()));
  o_desktop_activity->mEditor = new CherryKit::TextEditor(o_desktop_activity->mLayoutBase);
  o_desktop_activity->mVLayout->addItem(o_desktop_activity->mEditor);

  o_desktop_activity->mHLayout = new QGraphicsLinearLayout(o_desktop_activity->mHLayoutBase);
  o_desktop_activity->mHLayout->setOrientation(Qt::Horizontal);

  o_desktop_activity->mOkButton->set_label(tr("Post"));
  o_desktop_activity->mCancelButton->set_label(tr("Cancel"));

  o_desktop_activity->mHLayout->setContentsMargins(25.0, 0.0, 25.0, 25.0);
  o_desktop_activity->mVLayout->setContentsMargins(10.0, 10.0, 10.0, 10.0);
  o_desktop_activity->mVLayout->addItem(o_desktop_activity->mHLayoutBase);
  o_desktop_activity->mHLayout->addStretch(1);
  o_desktop_activity->mHLayout->addItem(o_desktop_activity->mCancelButton);
  o_desktop_activity->mHLayout->addItem(o_desktop_activity->mOkButton);

  o_desktop_activity->mEditor->setFocus();

  connect(o_desktop_activity->mCancelButton, SIGNAL(clicked()), this, SIGNAL(canceled()));
  connect(o_desktop_activity->mOkButton, SIGNAL(clicked()), this, SLOT(onOkButtonPressed()));

  this->exec();
  show_activity();
}

QString InputDialogActivityData::error_message() const { return QString(); }

QVariantMap InputDialogActivityData::activityResult() const {
  QVariantMap rv;
  rv["text"] = QVariant(o_desktop_activity->mCurrentText);
  return rv;
}

QRectF InputDialogActivityData::geometry() const { return o_desktop_activity->mBoundingRect; }

QVariantMap InputDialogActivityData::result() const { return QVariantMap(); }

CherryKit::Window *InputDialogActivityData::window() const { return o_desktop_activity->mFrame; }

void InputDialogActivityData::cleanup() {
  if (o_desktop_activity->mFrame) {
    delete o_desktop_activity->mFrame;
  }

  o_desktop_activity->mFrame = 0;
}

void InputDialogActivityData::onWidgetClosed(CherryKit::Widget *widget) {
  discard_activity();
}

void InputDialogActivityData::onMotionAnimFinished() {}

void InputDialogActivityData::onOkButtonPressed() {
  o_desktop_activity->mCurrentText = o_desktop_activity->mEditor->text();
  discard_activity();
}

void InputDialogActivityData::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *widget) {}
