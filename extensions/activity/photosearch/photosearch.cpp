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
#include "photosearch.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>

#include <plexyconfig.h>
#include <view_controller.h>
#include <tableview.h>
#include <default_table_component.h>
#include <progressbar.h>
#include <default_table_model.h>
#include "imagecell.h"
#include "imagecelladaptor.h"
#include <asyncimageloader.h>
#include <asyncimagecreator.h>
#include <viewbuilder.h>
#include <button.h>
#include <dialwidget.h>
#include <item_view.h>

#include "localwallpapers.h"

class PhotoSearchActivity::PrivatePhotoSearch {
public:
  PrivatePhotoSearch() {}
  ~PrivatePhotoSearch() {}

  CherryKit::Window *m_window;
  CherryKit::ProgressBar *mProgressBar;
  CherryKit::TableView *mTable;

  CherryKit::ItemView *m_image_view;

  ImageCellAdaptor *mFactory;
  QRectF mGeometry;
  QVariantMap mResult;
};

PhotoSearchActivity::PhotoSearchActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), d(new PrivatePhotoSearch) {}

PhotoSearchActivity::~PhotoSearchActivity() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void PhotoSearchActivity::create_window(const QRectF &aWindowGeometry,
                                        const QString &aWindowTitle,
                                        const QPointF &window_pos) {
  d->mGeometry = aWindowGeometry;

  d->m_window = new CherryKit::Window();
  d->m_window->setGeometry(aWindowGeometry);
  d->m_window->set_window_title(aWindowTitle);

  CherryKit::HybridLayout *ck_ui = new CherryKit::HybridLayout(d->m_window);

  ck_ui->set_content_margin(10, 10, 10, 10);
  ck_ui->set_geometry(0, 0, aWindowGeometry.width(), aWindowGeometry.height());

  ck_ui->add_rows(2);
  ck_ui->add_segments(0, 1);
  ck_ui->add_segments(1, 6);

  ck_ui->set_row_height(0, "95%");
  ck_ui->set_row_height(1, "5%");

  CherryKit::WidgetProperties ui_data;

  CherryKit::Widget *ck_icon_gird = ck_ui->add_widget(0, 0, "widget", ui_data);
  d->m_image_view =
      new CherryKit::ItemView(ck_icon_gird, CherryKit::ItemView::kGridModel);

  ck_icon_gird->on_geometry_changed([&](const QRectF &a_rect) {
    d->m_image_view->setGeometry(a_rect);
  });

  ui_data["label"] = "00";

  ck_ui->add_widget(1, 0, "button", ui_data);
  ck_ui->add_widget(1, 1, "button", ui_data);
  ck_ui->add_widget(1, 2, "button", ui_data);

  d->m_window->set_window_content(ck_ui->viewport());

  QTimer::singleShot(500, this, SLOT(load_from_system_path()));

  /*
  connect(d->mTable, SIGNAL(activated(TableViewItem *)), this,
          SLOT(onClicked(TableViewItem *)));
  connect(d->mFactory, SIGNAL(completed(int)), this,
          SLOT(onProgressValue(int)));

  */
  exec(window_pos);
}

QRectF PhotoSearchActivity::geometry() const { return d->mGeometry; }

QVariantMap PhotoSearchActivity::result() const { return d->mResult; }

Window *PhotoSearchActivity::window() const { return d->m_window; }

void PhotoSearchActivity::cleanup() {
  if (d->m_window) {
    delete d->m_window;
  }

  d->m_window = 0;
}

void PhotoSearchActivity::onShowAnimationFinished() {
  this->load_from_system_path();
}

void PhotoSearchActivity::onClicked(TableViewItem *item) {
  if (d->mFactory && d->mFactory->hasRunningThreads()) {
    return;
  }

  ImageCell *i = qobject_cast<ImageCell *>(item);
  if (i) {
    d->mResult["action"] = QString("Change Background");
    d->mResult["background"] = "file:///" + i->label();
    update_action();
  }
}

void PhotoSearchActivity::onProgressValue(int value) {
  if (value == 100) {
    if (d->m_window) {
      if (has_attribute("title")) {
      }
    }
  }
}

void PhotoSearchActivity::load_from_system_path() const {
  std::vector<std::string> image_path_list;

#ifdef Q_OS_LINUX
  image_path_list.push_back("/usr/share/backgrounds/");
  image_path_list.push_back("/usr/share/backgrounds/gnome");
#elif defined(Q_OS_MAC)
  image_path_list.push_back("/Library/Desktop Pictures/");
#elif defined(Q_OS_WIN)
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Theme1\\");
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Theme2\\");
  image_path_list.push_back("C:\\Windows\\Web\\Wallpaper\\Windows\\");
#endif

  image_path_list.push_back(
      CherryKit::Config::cache_dir("wallpaper").toStdString());

  std::for_each(std::begin(image_path_list), std::end(image_path_list),
                [&](const std::string &a_path) {
    qDebug() << Q_FUNC_INFO << a_path.c_str();
    QFileInfo path_meta_data(a_path.c_str());

    if (path_meta_data.isDir()) {
      QStringList allowed_mime_types;
      QDir qt_path(path_meta_data.filePath());

      allowed_mime_types << "*.png"
                         << "*.jpg"
                         << "*.jpeg"
                         << "*.tiff"
                         << "*.svg";
      qt_path.setNameFilters(allowed_mime_types);

      QStringList filtered_local_files = qt_path.entryList();

      Q_FOREACH(const QString & image_file_name, filtered_local_files) {
        QString image_full_path = QDir::toNativeSeparators(
            qt_path.absolutePath() + "/" + image_file_name);

        QuetzalSocialKit::AsyncImageCreator *ck_image_service =
            new QuetzalSocialKit::AsyncImageCreator();
        ck_image_service->setData(image_full_path,
                                  CherryKit::Config::cache_dir());
        ck_image_service->on_task_complete([this](
            QuetzalSocialKit::AsyncImageCreator *a_service) {
           if (!a_service)
               return;

           qDebug() << Q_FUNC_INFO << a_service->imagePath();
           qDebug() << Q_FUNC_INFO << "Image Data : " << a_service->image().isNull();
           qDebug() << Q_FUNC_INFO << "Image Thumbnail : " << a_service->thumbNail().isNull();
           delete a_service;
        });
        ck_image_service->start();
      }
    }
  });
}
