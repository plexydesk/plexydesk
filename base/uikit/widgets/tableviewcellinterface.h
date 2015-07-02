/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : Siraj Razick <siraj@plexydesk.org>
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

#ifndef TABLEVIEWCELLINTERFACE_H
#define TABLEVIEWCELLINTERFACE_H

#include <plexy.h>
#include <widget.h>
#include <abstractcellcomponent.h>
#include <plexydesk_ui_exports.h>

namespace CherryKit {
/**
    * \class this is a interface definition cell creators
    *
    * @brief Implement this class to build cells required by
  *UI::TableView
    */
class DECL_UI_KIT_EXPORT TableModel : public QObject {
  Q_OBJECT

public:
  /**
      * @brief Cell Type to use for the TableView
      * List for Vriticle list of cell items
      * and Grid to place items on a grid.
      */
  enum TableRenderMode {
    kRenderAsListView,
    kRenderAsGridView
  };
  /**
      * @brief Constructor for TableViewCellInterface
      *
      * @param rect Recangular size of of the initial TableCellArea.
      * @param parent Parent of the TableViewCellInterface
      */
  TableModel(QObject *a_parent_ptr = 0);

  /**
      * @brief  Class Distructor
      */
  virtual ~TableModel();

  /**
      * @brief Pure virtual method which triggers the cell creation.
      *
      * Classes which impliments TableViewCellInterface should add the init code
      * under method, TableView class which makes use of this method will call
    *this
      * method when the Cell Interface Delegate is set for a given View.
      *
      * @return Should return true if the cell creation was sucsessful, false it
    *not.
      */
  virtual bool init() = 0;

  /**
      * @brief The Cell Margin to be used
      * The table view will use the Cell margine defined here.
      *
      * @return  Return the Cell Margin, the default value is 0.0f
      */
  virtual float margin() const;

  /**
      * @brief The Cell Spacing to be used.
      *
      * This meethod defines the Cell Spacing to be used by the Table View
      *
      * @return  Returns the Cell Spacing, the default value is 0.0f.
      */
  virtual float padding() const;

  /**
      * @brief Left Cell Margin
      *
      * This method define the left cell margin for the Cell;
      * @return
      */
  virtual float left_margin() const;

  /**
      * @brief Right Cell Margin
      *
      * Define the right cell margin for the cell item.
      * @return
      */
  virtual float right_margin() const;

  /**
      * @brief returns the Cell types to create
      *
      * Return the cell type to use for this TableCellInterface, Grid will place
    *them on a Grid
      * Layout, and List will place them on a Verticle List
      * @return returns the Cell Layout Type
      */
  virtual TableRenderMode render_type() const;

Q_SIGNALS:
  /**
      * @brief Signal to be emitted with the cells are ready
      *
      * When the cells are ready the implimentors of this class should emit this
    *signal
      * so that the the TableView can update the cells.
      */
  void updated();

  void add(CherryKit::TableViewItem *a_item_ptr);

  void removed(CherryKit::TableViewItem *a_item_ptr);

  void cleared();
};
}
#endif // TABLEVIEWCELLINTERFACE_H
