#include "facebookcontactui.h"
#include <scrollck_widget.h>
#include <ck_button.h>
#include <ck_line_edit.h>
#include "contactlist.h"

class FacebookContactUI::PrivateFacebookContactUI {
public:
  PrivateFacebookContactUI() {}
  ~PrivateFacebookContactUI() {}

  QHash<QString, QVariant> mData;
  ContactList *mScrollView;
  PlexyDesk::Button *mSearchButton;
  PlexyDesk::LineEdit *mSearchBox;
};

FacebookContactUI::FacebookContactUI(const QRectF &rect)
    : PlexyDesk::DesktopWidget(rect), d(new PrivateFacebookContactUI) {
  this->setWidgetFlag(PlexyDesk::DesktopWidget::WINDOW, true);
  this->setWidgetFlag(PlexyDesk::DesktopWidget::BACKGROUND, true);
  setWidgetFlag(PlexyDesk::DesktopWidget::SHADOW);
  setLabelName("Contacts");

  QRectF scrollRect = QRect(5.0, 24.0, rect.width() - 10, rect.height() - 80);
  d->mScrollView = new ContactList(scrollRect, this);
  d->mScrollView->setVisible(true);
  // d->mScrollView->setPos(0.0, 24.0);
  // d->mScrollView->setLabelName("Contacts");
  d->mScrollView->setFlag(QGraphicsItem::ItemIsMovable, false);

  d->mSearchBox = new PlexyDesk::LineEdit(this);
  d->mSearchBox->setSize(QSizeF(rect.width() - 40, 32));
  d->mSearchBox->show();
  d->mSearchBox->setPos((rect.width() / 2) -
                            (d->mSearchBox->boundingRect().width() / 2),
                        38 + d->mScrollView->boundingRect().height());

  connect(d->mSearchBox, SIGNAL(text(QString)), d->mScrollView,
          SLOT(filter(QString)));
  connect(d->mScrollView, SIGNAL(clicked(QString)), this,
          SLOT(onViewClicked(QString)));

  setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

FacebookContactUI::~FacebookContactUI() {
  d->mData.clear();
  delete d;
}

void FacebookContactUI::setFacebookContactData(QHash<QString, QVariant> data) {
  d->mData = data;

  Q_FOREACH(const QString & name, data.keys()) {
    // d->mScrollView->addContact(name);
  }
}

void FacebookContactUI::addContact(const QVariantMap &data) {
  if (d->mScrollView)
    d->mScrollView->addContact(data["id"].toString(), data["name"].toString(),
                               data["message"].toString(),
                               data["picture"].value<QPixmap>());
}

void FacebookContactUI::onViewClicked(QString id) {
  qDebug() << Q_FUNC_INFO << id;
  Q_EMIT addContactCard(id);
}

void FacebookContactUI::paintFrontView(QPainter *painter, const QRectF &rect) {
  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 6.0, 6.0);
  painter->fillPath(backgroundPath, QColor(235, 235, 235));

  QPen pen = QPen(QColor(220, 220, 220), 1, Qt::SolidLine, Qt::RoundCap,
                  Qt::RoundJoin);
  painter->setPen(pen);

  if (d->mSearchBox) {
    QRectF searchBoxRect = QRectF(
        0.0, d->mScrollView->boundingRect().bottomRight().y() + 5, rect.width(),
        (rect.height() - d->mScrollView->boundingRect().height() + 10));

    QRectF searchBoxLineRect =
        QRectF(0.0, d->mScrollView->boundingRect().height() + 29, rect.width(),
               rect.height());

    QPen pen = QPen(QColor(220, 220, 220), 1, Qt::SolidLine, Qt::RoundCap,
                    Qt::RoundJoin);
    painter->setPen(pen);

    QLinearGradient linearGrad(searchBoxRect.topRight(),
                               searchBoxRect.bottomRight());

    linearGrad.setColorAt(0, QColor(141, 184, 202));
    linearGrad.setColorAt(0.125, QColor(112, 167, 193));
    linearGrad.setColorAt(0.875, QColor(69, 112, 162));
    linearGrad.setColorAt(1, QColor(62, 92, 130));

    // painter->fillRect(searchBoxRect, QColor(0, 0, 0));
    painter->fillRect(searchBoxRect, linearGrad);
    painter->drawLine(searchBoxRect.topRight(), searchBoxRect.topLeft());
    pen =
        QPen(QColor(88, 88, 88), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawLine(searchBoxLineRect.topRight(),
                      searchBoxLineRect.topLeft());
  }
}
