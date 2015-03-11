#include "folderitem.h"
#include <button.h>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsSceneEvent>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QDesktopServices>
#include <QColor>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>

class FolderItem::PrivateFolderItem
{
public:
  PrivateFolderItem() {}
  ~PrivateFolderItem() {}
  QRectF mRect;
  QIcon mIcon;
  QString mFileName;
  QString mFilePath;
  QFileInfo mFileInfo;
  QPointF mDragStartPosition;
  bool mSelected;

  QGraphicsDropShadowEffect *mShadowEffect;
};

FolderItem::FolderItem(const QRectF &rect, QGraphicsItem *parent)
  : UIKit::TableViewItem(rect, parent), d(new PrivateFolderItem)
{
  this->setCacheMode(DeviceCoordinateCache);
  d->mRect = rect;
  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
  setAcceptTouchEvents(true);
  setAcceptHoverEvents(true);
  setGraphicsItem(this);
  d->mSelected = false;

  d->mShadowEffect = new QGraphicsDropShadowEffect(this);
  d->mShadowEffect->setBlurRadius(6.0);
  d->mShadowEffect->setEnabled(true);
  d->mShadowEffect->setColor(Qt::black);
  d->mShadowEffect->setXOffset(0);
  d->mShadowEffect->setYOffset(0);
  // this->setGraphicsEffect(d->mShadowEffect);
}

QRectF FolderItem::boundingRect() const { return QRectF(0.0, 0.0, 96, 96); }

QSizeF FolderItem::sizeHint(Qt::SizeHint hint, const QSizeF &size) const
{
  return boundingRect().size();
}

void FolderItem::setIcon(const QIcon &icon) { d->mIcon = icon; }

void FolderItem::setFileName(const QString &fileName)
{
  d->mFileName = fileName;
}

void FolderItem::setFilePath(const QString &path) { d->mFilePath = path; }

void FolderItem::setFileInfo(const QFileInfo &info) { d->mFileInfo = info; }

QFileInfo FolderItem::fileInfo() const { return d->mFileInfo; }

QIcon FolderItem::icon() const { return d->mIcon; }

void FolderItem::set_selected()
{
  d->mSelected = true;
  update();
}

void FolderItem::clear_selection()
{
  d->mSelected = false;
  update();
}

void FolderItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
  if (!painter->isActive()) {
    return;
  }
  if (isObscured()) {
    return;
  }

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath path;
  path.addRoundedRect(boundingRect(), 4.0, 4.0);

  if (d->mSelected) {
    painter->setOpacity(0.5);
    painter->setPen(QColor("#e7e7e7"));
    painter->drawPath(path);
    // painter->drawRect(option->exposedRect);
  }

  QRect iconRect((boundingRect().width() - 48) / 2, 10.0, 48, 48);
  QRectF labelRectF(0, iconRect.height() + 2.0, option->exposedRect.width(),
                    option->rect.height() - iconRect.height());

  if (d->mSelected)
    d->mIcon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Active,
                   QIcon::On);
  else
    d->mIcon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal,
                   QIcon::On);

  QTextOption txtOption;
  txtOption.setAlignment(Qt::AlignCenter);
  txtOption.setWrapMode(QTextOption::WrapAnywhere);

  QFontMetrics metrics(painter->font());
  QString elidedText =
    metrics.elidedText(d->mFileName, Qt::ElideMiddle, labelRectF.width());

  // Shadow

  // text
  QPen pen;
  pen.setColor(QColor(0, 0, 0));

  painter->setPen(pen);
  painter->drawText(labelRectF, elidedText, txtOption);
}

void FolderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << Q_FUNC_INFO << endl;
  event->accept();
  if (event->button() == Qt::LeftButton) {
    d->mDragStartPosition = event->pos();
  }
}

void FolderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << Q_FUNC_INFO << endl;
  event->accept();
  Q_EMIT clicked(this);
  // QGraphicsItem::mouseReleaseEvent(event);
}

void FolderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  if ((event->pos() - d->mDragStartPosition).manhattanLength() <
      QApplication::startDragDistance()) {
    return;
  }

  QMimeData *data = new QMimeData;
  data->setImageData(d->mIcon.pixmap(64, 64).toImage());
  data->setUrls(QList<QUrl>() << QUrl(d->mFilePath));

  QDrag *drag = new QDrag(event->widget());
  drag->setPixmap(d->mIcon.pixmap(64, 64));
  drag->setObjectName(d->mFileName);
  drag->setMimeData(data);
  drag->start();
  // Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void FolderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << Q_FUNC_INFO << "Run File: " << d->mFilePath;
  Q_EMIT doubleClicked(this);
  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(d->mFilePath))) {
    qWarning() << "File opening failed";
  }
  // QGraphicsObject::mouseDoubleClickEvent(event);
}
