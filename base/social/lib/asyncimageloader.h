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
#ifndef ASYNC_IMAGE_LOADER_H
#define ASYNC_IMAGE_LOADER_H

#include <QThread>
#include <QNetworkReply>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit {

class QuetzalSocialKit_EXPORT AsyncImageLoader : public QThread {
  Q_OBJECT
public:
  AsyncImageLoader(QObject *a_parent_ptr = 0);

  virtual ~AsyncImageLoader();

  void setUrl(const QUrl &url);

  QList<QImage> thumbNails() const;

  QImage imageThumbByName(const QString &key) const;

  QString filePathFromName(const QString &key) const;

  void run();

Q_SIGNALS:
  void ready();

  void ready(const QString &key);

private
Q_SLOTS:
  void onNetworkRequestFinished(QNetworkReply *reply);

private:
  class PrivateAsyncImageLoader;
  PrivateAsyncImageLoader *const d;
};

} // namespace Social
#endif // ASYNC_IMAGE_LOADER_H
