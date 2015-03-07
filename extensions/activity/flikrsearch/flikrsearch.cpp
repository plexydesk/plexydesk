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
#include "flikrsearch.h"
#include "photocell.h"
#include "photocelladaptor.h"

// Qt includes
#include <QGraphicsLinearLayout>
#include <QPropertyAnimation>
#include <QGraphicsWidget>

// Qt Includes
#include <widget.h>
#include <tableview.h>
#include <button.h>
#include <lineedit.h>
#include <label.h>
#include <progressbar.h>
#include <toolbar.h>

#include <themepackloader.h>

class FlickrSearchActivity::PrivateFlikrSearch
{
public:
  PrivateFlikrSearch() {}

  ~PrivateFlikrSearch()
  {
    // if (m_view_delegate_window)
  }

  QGraphicsWidget *m_main_frame_widget;
  UIKit::Widget *mWindowContentWidget;
  QGraphicsLinearLayout *m_verticle_layout;

  UIKit::ToolBar *m_top_toolbar;
  UIKit::ToolBar *m_bottom_toolbar;

  UIKit::Window *m_view_delegate_window;

  UIKit::TableView *m_image_grid_view;
  PhotoCellAdaptor *m_image_cell_model;

  UIKit::ProgressBar *m_progress_widget;
  UIKit::LineEdit *m_search_input_box;

  int m_page_count;
  int m_current_page_index;
  bool m_is_new_search;

  QString m_current_search_query_text;
  QVariantMap m_activity_result_map;
};

FlickrSearchActivity::FlickrSearchActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateFlikrSearch) {}

FlickrSearchActivity::~FlickrSearchActivity()
{
  // discardActivity();
  delete d;
}

void FlickrSearchActivity::createWindow(const QRectF &aWindowGeometry,
                                        const QString &window_title,
                                        const QPointF &window_pos)
{
  d->m_is_new_search = true;

  d->m_view_delegate_window = new UIKit::Window();
  d->m_view_delegate_window->setGeometry(aWindowGeometry);

  d->mWindowContentWidget = new UIKit::Widget();
  d->m_main_frame_widget = new QGraphicsWidget(d->mWindowContentWidget);

  d->m_main_frame_widget->setGeometry(aWindowGeometry);
  d->mWindowContentWidget->setGeometry(aWindowGeometry);

  d->m_view_delegate_window->setWindowContent(d->mWindowContentWidget);

  d->m_top_toolbar = new UIKit::ToolBar(d->m_main_frame_widget);
  d->m_top_toolbar->setIconSize(QSize(32, 32));
  d->m_top_toolbar->setMinimumSize(QSizeF(aWindowGeometry.width() - 20, 42));

  d->m_search_input_box = new UIKit::LineEdit(d->m_top_toolbar);
  d->m_search_input_box->setMinimumSize(
    QSizeF(aWindowGeometry.width() - 58, 32));

  d->m_top_toolbar->insertWidget(d->m_search_input_box);
  d->m_top_toolbar->addAction(tr("search"), "pd_out_icon", false);

  d->m_verticle_layout = new QGraphicsLinearLayout(d->m_main_frame_widget);
  d->m_verticle_layout->setOrientation(Qt::Vertical);
  d->m_verticle_layout->setContentsMargins(5.0, 0.0, 5.0, 0.0);
  d->m_verticle_layout->setGeometry(aWindowGeometry);
  d->m_verticle_layout->setSpacing(0);

  d->m_verticle_layout->addItem(d->m_top_toolbar);

  // image
  d->m_image_grid_view = new UIKit::TableView(d->m_main_frame_widget);
  d->m_image_cell_model = new PhotoCellAdaptor(d->m_image_grid_view);
  d->m_image_cell_model->setLabelVisibility(false);
  d->m_image_grid_view->setModel(d->m_image_cell_model);

  d->m_image_grid_view->setMinimumSize(QSizeF(aWindowGeometry.width(), 308));

  d->m_verticle_layout->addItem(d->m_image_grid_view);

  // toolbar
  d->m_bottom_toolbar = new UIKit::ToolBar(d->m_main_frame_widget);
  d->m_bottom_toolbar->setIconSize(QSize(32, 32));
  d->m_bottom_toolbar->setMinimumSize(
    QSizeF(d->m_top_toolbar->frameGeometry().width(),
           d->m_top_toolbar->frameGeometry().height()));

  d->m_progress_widget = new UIKit::ProgressBar(d->m_bottom_toolbar);
  d->m_progress_widget->setMinimumSize(
    QSizeF(aWindowGeometry.width() - 116, 32));

  d->m_bottom_toolbar->addAction(tr("next"), "pd_in_icon", false);
  d->m_bottom_toolbar->insertWidget(d->m_progress_widget);
  d->m_bottom_toolbar->addAction(tr("previous"), "pd_out_icon", false);

  d->m_verticle_layout->addItem(d->m_bottom_toolbar);

  // signals
  connect(d->m_image_cell_model, SIGNAL(completed(int)), this,
          SLOT(onCompleted(int)));
  connect(d->m_image_cell_model, SIGNAL(progressRange(int)), this,
          SLOT(onProgressRange(int)));
  connect(d->m_search_input_box, SIGNAL(submit()), this,
          SLOT(onButtonClicked()));
  connect(d->m_top_toolbar, SIGNAL(action(QString)), this,
          SLOT(onButtonClicked()));
  connect(d->m_bottom_toolbar, SIGNAL(action(QString)), this,
          SLOT(onToolbarAction(QString)));
  connect(d->m_image_grid_view, SIGNAL(activated(TableViewItem *)), this,
          SLOT(photoItemClicked(TableViewItem *)));

  exec(window_pos);

  d->m_image_cell_model->setSearchQuery("nature");

  d->m_view_delegate_window->onWindowDiscarded([this](UIKit::Window * aWindow) {
    discardActivity();
  });
}

QVariantMap FlickrSearchActivity::result() const
{
  return d->m_activity_result_map;
}

Window *FlickrSearchActivity::window() const
{
  return d->m_view_delegate_window;
}

void FlickrSearchActivity::cleanup()
{
  if (!d->m_view_delegate_window) {
    return;
  }

  delete d->m_view_delegate_window;
  d->m_view_delegate_window = 0;
}

void FlickrSearchActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void FlickrSearchActivity::photoItemClicked(UIKit::TableViewItem *item)
{
  PhotoCell *i = qobject_cast<PhotoCell *>(item);
  if (i) {
    d->m_activity_result_map["action"] = QString("Change Background");

    QUrl url = d->m_image_cell_model->requestImageUrl(i->label(), "Original");
    if (url.isValid()) {
      d->m_activity_result_map["background"] = url.toString();
      updateAction();
      // d->m_view_delegate_window->hide();
      // connect(this, SIGNAL(discarded()), this,
      // SLOT(onHideAnimationFinished()));
      // discardActivity();
    }
  }
}

void FlickrSearchActivity::onButtonClicked()
{
  if (d->m_search_input_box) {
    d->m_is_new_search = true;
    d->m_current_search_query_text = d->m_search_input_box->text();
    d->m_image_cell_model->setSearchQuery(d->m_search_input_box->text());
    d->m_progress_widget->show();
  }
}

void FlickrSearchActivity::onNextButtonClicked()
{
  d->m_is_new_search = false;
  d->m_current_page_index++;
  d->m_image_cell_model->setSearchQuery(d->m_current_search_query_text,
                                        d->m_current_page_index);
  d->m_progress_widget->show();
}

void FlickrSearchActivity::onPreviousButtonClicked()
{
  d->m_is_new_search = false;
  d->m_current_page_index--;
  d->m_image_cell_model->setSearchQuery(d->m_current_search_query_text,
                                        d->m_current_page_index);
  d->m_progress_widget->show();
}

void FlickrSearchActivity::onHideAnimationFinished()
{
  // delete d->m_view_delegate_window;
  // d->m_view_delegate_window = 0;
  // Q_EMIT finished();
}

void FlickrSearchActivity::onPageCount(int count)
{
  if (d->m_is_new_search) {
    d->m_page_count = count;
    d->m_current_page_index = 1;
  }
}

void FlickrSearchActivity::onProgressRange(int range)
{
  d->m_progress_widget->setRange(1, range);
  d->m_progress_widget->show();
}

void FlickrSearchActivity::onCompleted(int count)
{
  d->m_progress_widget->setValue(count);

  if (count == d->m_progress_widget->maxRange()) {
    d->m_progress_widget->hide();
  }
}

void FlickrSearchActivity::onToolbarAction(const QString &action)
{
  if (action == tr("next")) {
    onNextButtonClicked();
  } else if (action == tr("previous")) {
    onPreviousButtonClicked();
  }
}
