/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef ASYNCIMAGECREATOR_H
#define ASYNCIMAGECREATOR_H

#include <QThread>
#include <QImage>
#include <QuetzalSocialKit_export.h>

#include <functional>
#include <string>

namespace QuetzalSocialKit {

class QuetzalSocialKit_EXPORT AsyncImageCreator : public QThread {
  Q_OBJECT
public:
  explicit AsyncImageCreator(QObject *a_parent_ptr = 0);
  virtual ~AsyncImageCreator();

  void setMetaData(const QVariantMap &data);

  QVariantMap metaData() const;

  void setData(const QString &path, const QString &prefix, bool save = false);
  void setData(const QByteArray &data, const QString &path, bool save = false);
  void setData(const QImage &data, const QString &path, bool save = false);

  void setCrop(const QRectF &cropRect);
  void setScaleToHeight(int height);
  void setScaleToWidth(int width);

  void setThumbNailSize(const QSize &size);
  QImage thumbNail() const;

  QImage image() const;
  QString imagePath() const;

  bool offline() const;

  QByteArray imageToByteArray(const QImage &img) const;

  virtual void on_task_complete(
          std::function<void (AsyncImageCreator *)> a_callback);

Q_SIGNALS:
  void ready();

private Q_SLOTS:
  void on_finished();

protected:
  void run();

private:
  QImage genThumbNail(const QImage &img) const;

  class PrivateAsyncImageCreator;
  PrivateAsyncImageCreator *const d;
};
}

#endif // ASYNCIMAGECREATOR_H
