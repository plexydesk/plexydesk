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
#ifndef ASYNCDATADOWNLOADER_H
#define ASYNCDATADOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit {

class QuetzalSocialKit_EXPORT AsyncDataDownloader : public QObject {
  Q_OBJECT
public:
  explicit AsyncDataDownloader(QObject *a_parent_ptr = 0);
  virtual ~AsyncDataDownloader();

  void setMetaData(const QVariantMap &metaData);
  QVariantMap metaData() const;

  void setUrl(const QUrl &url);

  QByteArray data() const;

Q_SIGNALS:
  void progress(float progress);
  void ready();

private
Q_SLOTS:
  void onDownloadComplete(QNetworkReply *reply);
  void onDownloadProgress(qint64, qint64);

private:
  class PrivateAsyncDataDownloader;
  PrivateAsyncDataDownloader *const d;
};
}

#endif // ASYNCDATADOWNLOADER_H
