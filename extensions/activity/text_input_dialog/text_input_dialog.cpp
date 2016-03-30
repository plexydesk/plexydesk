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
#include "text_input_dialog.h"
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

class text_input_dialog::PrivateInputDialogActivity {
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

text_input_dialog::text_input_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      priv(new PrivateInputDialogActivity) {
  priv->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
}

text_input_dialog::~text_input_dialog() { delete priv; }

void text_input_dialog::create_window(const QRectF &window_geometry,
                                            const QString &window_title,
                                            const QPointF &window_pos) {
  /*
  if (has_attribute("geometry")) {
    priv->mBoundingRect = attributes()["geometry"].toRect();
  } else {
    priv->mBoundingRect = QRectF(0.0, 0.0, 320.0, 240.0);
  }

  // todo: invoke UI
  priv->mFrame = new cherry_kit::window();
  priv->mFrame->set_geometry(geometry());
  priv->mFrame->setVisible(true);
  priv->mFrame->set_widget_name("Message Dialog");

  priv->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  priv->mFrame->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  priv->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  priv->mBackgroundEffect = new QGraphicsBlurEffect(this);
  priv->mBackgroundEffect->setBlurHints(
      QGraphicsBlurEffect::PerformanceHint);
  priv->mBackgroundEffect->setBlurRadius(0.0);
  priv->mBackgroundEffect->setEnabled(true);

  priv->mLayoutBase =
      new cherry_kit::widget(priv->mFrame);
  priv->mLayoutBase->setPos(0.0, 72.0);
  priv->mVLayout =
      new QGraphicsLinearLayout(priv->mLayoutBase);
  priv->mVLayout->setOrientation(Qt::Vertical);

  QRectF _layoutRect(0.0, 0.0, geometry().width(), geometry().height() - 64.0);
  priv->mVLayout->setGeometry(_layoutRect);
  priv->mVLayout->setMaximumHeight(_layoutRect.height());

  priv->mHLayoutBase =
      new cherry_kit::widget(priv->mLayoutBase);

  priv->mOkButton =
      new cherry_kit::button(priv->mHLayoutBase);
  priv->mCancelButton =
      new cherry_kit::button(priv->mHLayoutBase);

  QRectF _editorRect(
      0.0, 0.0, geometry().width(),
      geometry().height() -
          (96 + priv->mOkButton->boundingRect().height()));
  priv->mEditor =
      new cherry_kit::text_editor(priv->mLayoutBase);
  priv->mVLayout->addItem(priv->mEditor);

  priv->mHLayout =
      new QGraphicsLinearLayout(priv->mHLayoutBase);
  priv->mHLayout->setOrientation(Qt::Horizontal);

  priv->mOkButton->set_label(tr("Post"));
  priv->mCancelButton->set_label(tr("Cancel"));

  priv->mHLayout->setContentsMargins(25.0, 0.0, 25.0, 25.0);
  priv->mVLayout->setContentsMargins(10.0, 10.0, 10.0, 10.0);
  priv->mVLayout->addItem(priv->mHLayoutBase);
  priv->mHLayout->addStretch(1);
  priv->mHLayout->addItem(priv->mCancelButton);
  priv->mHLayout->addItem(priv->mOkButton);

  priv->mEditor->setFocus();

  connect(priv->mCancelButton, SIGNAL(clicked()), this,
          SIGNAL(canceled()));
  connect(priv->mOkButton, SIGNAL(clicked()), this,
          SLOT(onOkButtonPressed()));

  this->exec();
  show_activity();
  */
}

QString text_input_dialog::error_message() const { return QString(); }

QVariantMap text_input_dialog::activityResult() const {
  QVariantMap rv;
  rv["text"] = QVariant(priv->mCurrentText);
  return rv;
}

QRectF text_input_dialog::geometry() const {
  return priv->mBoundingRect;
}

QVariantMap text_input_dialog::result() const { return QVariantMap(); }

cherry_kit::window *text_input_dialog::dialog_window() const {
  return priv->mFrame;
}

void text_input_dialog::purge() {
  if (priv->mFrame) {
    delete priv->mFrame;
  }

  priv->mFrame = 0;
}

void text_input_dialog::onWidgetClosed(cherry_kit::widget *widget) {
  discard_activity();
}

void text_input_dialog::onMotionAnimFinished() {}

void text_input_dialog::onOkButtonPressed() {
  priv->mCurrentText = priv->mEditor->text();
  discard_activity();
}

void text_input_dialog::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *widget) {}
