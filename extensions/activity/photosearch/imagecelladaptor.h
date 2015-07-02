#ifndef IMAGE_CELL_ADAPTOR_H
#define IMAGE_CELL_ADAPTOR_H

#include <QString>
#include <tableviewcellinterface.h>
#include <webservice.h>

using namespace CherryKit;

class ImageCellAdaptor : public TableModel {
  Q_OBJECT

public:
  ImageCellAdaptor(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ImageCellAdaptor();

  float margin() const;

  float padding() const;

  virtual float left_margin() const;

  virtual float right_margin() const;

  virtual bool init();

  virtual TableRenderMode render_type() const;

  void addDataItem(const QString &label, const QImage &pixmap,
                   bool selected = false,
                   const QVariantMap &metaData = QVariantMap());

  void setLabelVisibility(bool visibility);

  void setCellSize(const QSize &size);

  QUrl requestImageUrl(const QString &id, const QString &size) const;

  QList<QString> availableSizesForImage(const QString &id);

  void setSearchQuery(const QString &query, int pageNumber = 1);

  void addPathList(const QStringList &pathList);

  bool hasRunningThreads();

Q_SIGNALS:
  void pageCount(int pagecount);
  void progressRange(int range);
  void completed(int count);

public
Q_SLOTS:
  void onImageRady();

private:
  void loadLocalImageFiles(QList<QUrl> &list);
  void getImageFromPath(const QString &str);

  class PrivateImageCellAdaptor;
  PrivateImageCellAdaptor *const d;
};

#endif // IMAGE_CELL_ADAPTOR_H
