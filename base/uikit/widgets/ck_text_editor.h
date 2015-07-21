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
#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <ck_widget.h>

#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT text_editor : public widget {
  Q_OBJECT
public:
  text_editor(widget *a_parent_ptr = 0);
  virtual ~text_editor();

  virtual void set_text(const QString &a_text);
  virtual void set_placeholder_text(const QString &a_placeholderText);

  virtual void set_font_point_size(qreal a_s);

  virtual QString text() const;

  virtual void style(const QString &a_style);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;
  virtual void setGeometry(const QRectF &a_rect);

  virtual void set_text_scale_factor(qreal a_scale_factor);
  virtual qreal text_scale_factor() const;
  virtual void update_text_scale();

  virtual void begin_list();
  virtual void end_list();

  virtual void convert_to_link();
  void update_scrollbars();
  public
Q_SLOTS:
  void on_text_updated();
  void on_block_count_changed(int a_count);
Q_SIGNALS:
  void text_updated(const QString &a_text);
  void document_title_available(const QString &a_title);

private:
  class PrivateTextEditor;
  PrivateTextEditor *const priv;
};
}

#endif // TEXTEDITOR_H
