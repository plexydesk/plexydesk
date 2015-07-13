/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include <ck_widget.h>
#include <ck_config.h>
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

#include <ck_desktop_controller_interface.h>
#include <ck_style.h>
#include <ck_resource_manager.h>
#include <ck_button.h>
#include <ck_text_editor.h>

class input_dialog::PrivateInputDialogActivity {
public:
  PrivateInputDialogActivity() {}
  ~PrivateInputDialogActivity() {}

  cherry_kit::window *mFrame;
  QGraphicsBlurEffect *mBackgroundEffect;

  /*Widgets*/
  cherry_kit::button *mOkButton;
  cherry_kit::button *mCancelButton;

  cherry_kit::widget *mLayoutBase;
  cherry_kit::widget *mHLayoutBase;

  QGraphicsLinearLayout *mVLayout;
  QGraphicsLinearLayout *mHLayout;

  QLineEdit *mLineEdit;
  QGraphicsProxyWidget *mLineEditProxy;
  cherry_kit::text_editor *mEditor;

  QRectF mBoundingRect;
  QString mCurrentText;
};

input_dialog::input_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateInputDialogActivity) {
  o_desktop_dialog->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
}

input_dialog::~input_dialog() { delete o_desktop_dialog; }

void input_dialog::create_window(const QRectF &window_geometry,
                                            const QString &window_title,
                                            const QPointF &window_pos) {
  if (has_attribute("geometry")) {
    o_desktop_dialog->mBoundingRect = attributes()["geometry"].toRect();
  } else {
    o_desktop_dialog->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
  }

  // todo: invoke UI
  o_desktop_dialog->mFrame = new cherry_kit::window();
  o_desktop_dialog->mFrame->setGeometry(geometry());
  o_desktop_dialog->mFrame->setVisible(true);
  o_desktop_dialog->mFrame->set_widget_name("Message Dialog");

  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  o_desktop_dialog->mBackgroundEffect = new QGraphicsBlurEffect(this);
  o_desktop_dialog->mBackgroundEffect->setBlurHints(
      QGraphicsBlurEffect::PerformanceHint);
  o_desktop_dialog->mBackgroundEffect->setBlurRadius(0.0);
  o_desktop_dialog->mBackgroundEffect->setEnabled(true);

  o_desktop_dialog->mLayoutBase =
      new cherry_kit::widget(o_desktop_dialog->mFrame);
  o_desktop_dialog->mLayoutBase->setPos(0.0, 72.0);
  o_desktop_dialog->mVLayout =
      new QGraphicsLinearLayout(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mVLayout->setOrientation(Qt::Vertical);

  QRectF _layoutRect(0.0, 0.0, geometry().width(), geometry().height() - 64.0);
  o_desktop_dialog->mVLayout->setGeometry(_layoutRect);
  o_desktop_dialog->mVLayout->setMaximumHeight(_layoutRect.height());

  o_desktop_dialog->mHLayoutBase =
      new cherry_kit::widget(o_desktop_dialog->mLayoutBase);

  o_desktop_dialog->mOkButton =
      new cherry_kit::button(o_desktop_dialog->mHLayoutBase);
  o_desktop_dialog->mCancelButton =
      new cherry_kit::button(o_desktop_dialog->mHLayoutBase);

  QRectF _editorRect(
      0.0, 0.0, geometry().width(),
      geometry().height() -
          (96 + o_desktop_dialog->mOkButton->boundingRect().height()));
  o_desktop_dialog->mEditor =
      new cherry_kit::text_editor(o_desktop_dialog->mLayoutBase);
  o_desktop_dialog->mVLayout->addItem(o_desktop_dialog->mEditor);

  o_desktop_dialog->mHLayout =
      new QGraphicsLinearLayout(o_desktop_dialog->mHLayoutBase);
  o_desktop_dialog->mHLayout->setOrientation(Qt::Horizontal);

  o_desktop_dialog->mOkButton->set_label(tr("Post"));
  o_desktop_dialog->mCancelButton->set_label(tr("Cancel"));

  o_desktop_dialog->mHLayout->setContentsMargins(25.0, 0.0, 25.0, 25.0);
  o_desktop_dialog->mVLayout->setContentsMargins(10.0, 10.0, 10.0, 10.0);
  o_desktop_dialog->mVLayout->addItem(o_desktop_dialog->mHLayoutBase);
  o_desktop_dialog->mHLayout->addStretch(1);
  o_desktop_dialog->mHLayout->addItem(o_desktop_dialog->mCancelButton);
  o_desktop_dialog->mHLayout->addItem(o_desktop_dialog->mOkButton);

  o_desktop_dialog->mEditor->setFocus();

  connect(o_desktop_dialog->mCancelButton, SIGNAL(clicked()), this,
          SIGNAL(canceled()));
  connect(o_desktop_dialog->mOkButton, SIGNAL(clicked()), this,
          SLOT(onOkButtonPressed()));

  this->exec();
  show_activity();
}

QString input_dialog::error_message() const { return QString(); }

QVariantMap input_dialog::activityResult() const {
  QVariantMap rv;
  rv["text"] = QVariant(o_desktop_dialog->mCurrentText);
  return rv;
}

QRectF input_dialog::geometry() const {
  return o_desktop_dialog->mBoundingRect;
}

QVariantMap input_dialog::result() const { return QVariantMap(); }

cherry_kit::window *input_dialog::activity_window() const {
  return o_desktop_dialog->mFrame;
}

void input_dialog::cleanup() {
  if (o_desktop_dialog->mFrame) {
    delete o_desktop_dialog->mFrame;
  }

  o_desktop_dialog->mFrame = 0;
}

void input_dialog::onWidgetClosed(cherry_kit::widget *widget) {
  discard_activity();
}

void input_dialog::onMotionAnimFinished() {}

void input_dialog::onOkButtonPressed() {
  o_desktop_dialog->mCurrentText = o_desktop_dialog->mEditor->text();
  discard_activity();
}

void input_dialog::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *widget) {}
