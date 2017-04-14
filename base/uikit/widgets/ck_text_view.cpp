#include "ck_text_view.h"

#include <QTextCursor>
#include <QTextLayout>
#include <QTextBlock>
#include <QTextDocument>

#include <QGraphicsSceneMouseEvent>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace cherry_kit {

class font_metrics {
   public:
    font_metrics() {
    }

    ~font_metrics() {}

    static int font_width() {
      QFont current_font;
      QFontMetrics metric(current_font);
      return metric.width('_');
    }

    static int font_width(char a_letter) {
      QFont current_font;
      QFontMetrics metric(current_font);
      return metric.width(a_letter);
    }


    static int font_height() {
      QFont current_font;
      QFontMetrics metric(current_font);
      return metric.height();
    }
};

class text_view::text_view_context {
public:
    text_view_context() : m_last_anchor(0), m_scroll_y(0.0) {
        m_font_height = font_metrics::font_height();
        m_font_width = font_metrics::font_width();
    }
    ~text_view_context() {}

    int hit_test(float, float);
    
    int needs_page_scroll();

    std::string m_text;
    font_metrics *m_engine;

    /* the visual caret pos */
    QPoint m_text_cursor_pos;
    QTextLayout *m_layout_mgr;

    int m_font_height;
    int m_font_width;

    QTextDocument m_doc;
    QTextCursor m_cursor;
    QTextBlock m_anchor_block; 
    int m_last_anchor;

    float m_scroll_y;
};

text_view::text_view(widget *a_parent) : widget(a_parent),
    ctx(new text_view_context) {
    ctx->m_engine = new font_metrics();
    ctx->m_layout_mgr = new QTextLayout();
}

text_view::~text_view(){
    delete ctx;
}

void text_view::set_text(const std::string &a_text) {
   QFont default_font("Courier");
   QSizeF default_page_size = QSizeF(geometry().width(), geometry().height());

   default_font.setPointSize(14);

   ctx->m_text = a_text;
   ctx->m_doc.setDefaultFont(default_font);
   ctx->m_doc.setPageSize(default_page_size);
   ctx->m_cursor = QTextCursor(&ctx->m_doc); 
   ctx->m_cursor.setVisualNavigation(true);
   ctx->m_cursor.insertText(a_text.c_str());
   update();
}

static void do_layout(QTextLayout *layout) {
  int leading = 17;// fontMetrics.leading();
  qreal height = layout->position().y();

  layout->beginLayout();
  while (1) {
    QTextLine line = layout->createLine();
    if (!line.isValid())
        break;

    //line.setLineWidth(lineWidth);
    height += leading;
    line.setPosition(QPointF(0, height));
    height += line.height();
  }

  layout->endLayout();
}

void text_view::paint_view(QPainter *a_painter, const QRectF &a_rect) {
    QPainterPath background_path;
    background_path.addRoundRect(a_rect, 1.5, 1.5);
    /* use Qt */

    a_painter->save();
    a_painter->setClipRect(a_rect);
    a_painter->setRenderHint(QPainter::TextAntialiasing, true);
    a_painter->setRenderHint(QPainter::Antialiasing, true);
    a_painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    a_painter->fillPath(background_path, Qt::white);

    a_painter->save();
    a_painter->setOpacity(0.3);
    //a_painter->fillRect(QRect(0, 0, 16.0, a_rect.height()), QColor("#888888"));
    a_painter->restore();

    QRectF document_geometry = QRectF(a_rect.x(),
                                      a_rect.y(),
                                      a_rect.width(),
                                      a_rect.height() + ctx->m_scroll_y);
    //a_painter->save();
    a_painter->translate(a_rect.x(), a_rect.y() - (ctx->m_scroll_y));
    ctx->m_doc.drawContents(a_painter, QRectF());
   // a_painter->restore();
        
    const QTextBlock current_block = 
      ctx->m_doc.findBlock(ctx->m_cursor.position()); 
    QTextLayout *current_layout = current_block.layout();

    if (hasFocus() && !ctx->m_cursor.hasSelection()) {
      current_layout->drawCursor(a_painter,
                                  QPointF(),
                                  ctx->m_cursor.positionInBlock(),
                                  10);
    }
    /* draw selection */
   if (current_layout && ctx->m_cursor.hasSelection()) {
     QPointF cursor_pos = current_layout->position();
     QTextBlock anchor_block = ctx->m_doc.findBlock(ctx->m_cursor.anchor());
     QTextLayout *anchor_layout = anchor_block.layout();
     
     if (anchor_layout) {
       QTextLine anchor_line = anchor_layout->lineForTextPosition(ctx->m_last_anchor); 
       QPointF anchor_block_pos = anchor_layout->position();
       qreal anchor_x = anchor_line.cursorToX(ctx->m_last_anchor) + anchor_block_pos.x();
       qreal anchor_y = anchor_block_pos.y() + anchor_line.rect().y();
       qreal leading = anchor_line.rect().height();

       QTextLine current_line = current_layout->lineForTextPosition(ctx->m_cursor.positionInBlock()); 
       QPointF current_block_pos = current_layout->position();
       qreal current_cursor_x = (current_line.cursorToX(ctx->m_cursor.positionInBlock()) + current_block_pos.x());
       qreal current_cursor_y = current_block_pos.y() + current_line.rect().y();

       qreal selection_length = current_cursor_x - anchor_x;
          
       /* check if we are on the same block */
       bool on_same_block = false;
       bool multi_line_selection = false;

       if (current_block.blockNumber() == anchor_block.blockNumber()) {
         on_same_block = true;
       }
      
       /* we are on the same line */
       QPainterPath selection_path;

       if (current_cursor_y == anchor_y) {
         selection_path.addRect(QRectF(anchor_x, anchor_y, selection_length , leading));
       }

       /* select text top down direction */ 
       if (current_cursor_y > anchor_y) {
         QRectF selection_rect_start = QRectF(anchor_x, anchor_y, anchor_line.rect().width() - anchor_x , leading);
         QRectF selection_rect_end = QRectF(current_line.rect().x(), current_cursor_y, current_cursor_x , leading);

         int line_count = (current_cursor_y - anchor_y) / leading;
         
         if (line_count >= 1) multi_line_selection = true;
         
         /* we are on the same block but we have multi-line selection */
         if (on_same_block && multi_line_selection) {
           QRectF inline_selction_start = QRectF(anchor_x, anchor_y, anchor_line.rect().width() - anchor_x, leading);
           QRectF inline_selction_end = QRectF(current_line.rect().x(), current_cursor_y, current_cursor_x, leading);

           selection_path.addRect(inline_selction_end);

           if (line_count > 1) {;
             QRectF selection_line_start = QRectF(0, anchor_y + leading, anchor_line.rect().width(), (line_count - 1) * leading);
             selection_path.addRect(selection_line_start);
           }
         
           selection_path.addRect(inline_selction_start);
           //qDebug() << Q_FUNC_INFO << "selection on same block @ : " << on_same_block << "Has Multi Selection @: " << multi_line_selection;
         } 

         /* we are on differnt blocks but we have multi-line selection */
         if (multi_line_selection && !on_same_block){ 
           QRectF selection_line_start = QRectF(0, anchor_y + leading, anchor_line.rect().width(), (line_count - 1) * leading);

           selection_path.addRect(selection_rect_start);
           selection_path.addRect(selection_line_start);
           selection_path.addRect(selection_rect_end);
         } 
       }

       /* select text buttom up direction and backwards*/ 
       if (current_cursor_y < anchor_y) {
         QRectF selection_rect_start = QRectF(0, anchor_y, anchor_x , leading); 
         QRectF selection_rect_end = QRectF(current_cursor_x, current_cursor_y, current_line.rect().width() - current_cursor_x , leading);

         int line_count = (anchor_y - current_cursor_y) / leading;

         if (line_count >= 1) multi_line_selection = true;
         
         if (multi_line_selection && !on_same_block) {
           selection_path.addRect(selection_rect_start);

           if (line_count > 1) {
             QRectF selection_line_start = QRectF(0, current_cursor_y + leading, anchor_line.rect().width(), (line_count - 1) * leading);
             selection_path.addRect(selection_line_start);
            }

           selection_path.addRect(selection_rect_end);
         } 

         /* we are on the same block but we have multi-line selection */
         if (on_same_block && multi_line_selection) {
	   QRectF inline_selction_start = QRectF(0, anchor_y, anchor_x, leading);
           QRectF inline_selction_end = QRectF(current_cursor_x, current_cursor_y, current_line.rect().width() - current_cursor_x, leading);

           selection_path.addRect(inline_selction_start);

           if (line_count > 1) {;
             QRectF selection_line_start = QRectF(0, current_cursor_y + leading, anchor_line.rect().width(), (line_count - 1) * leading);
             selection_path.addRect(selection_line_start);
           }
         
           selection_path.addRect(inline_selction_end);
         } 
       }

         a_painter->save();
         a_painter->setOpacity(0.4);
         QRectF selection_rect = selection_path.boundingRect();
         QLinearGradient selection_fill(QPointF(selection_rect.width() / 2, 0), 
                                        QPointF(selection_rect.width() / 2, selection_rect.height()));
         selection_fill.setColorAt(0, QColor("#EFEFEF"));
         selection_fill.setColorAt(0.50, QColor("#A0C7F1"));
         selection_fill.setColorAt(0.70, QColor("#87BAF2"));
         selection_fill.setColorAt(1, QColor("#C9F5FC"));

         a_painter->fillPath(selection_path, QBrush(selection_fill));
         a_painter->restore();

         a_painter->save();
         QPainterPathStroker stroke;
         a_painter->setOpacity(0.2);
         stroke.setJoinStyle(Qt::RoundJoin);
         stroke.setCapStyle(Qt::RoundCap);
         a_painter->strokePath(stroke.createStroke(selection_path.simplified()), QPen(QColor("#2f2c92")));
         a_painter->restore();

     }
   }
   
   a_painter->restore();
}

void text_view::keyPressEvent(QKeyEvent *a_event)
{
  ctx->m_cursor.clearSelection();

  if (a_event->key() == Qt::Key_Left) {
     ctx->m_cursor.movePosition(QTextCursor::Left);
     update();
     return;
  }

  if (a_event->key() == Qt::Key_Right) {
     ctx->m_cursor.movePosition(QTextCursor::Right);
     update();
     return;
  }

  if (a_event->key() == Qt::Key_Up) {
     ctx->m_cursor.movePosition(QTextCursor::Up);
     update();

     int scroll_by = ctx->needs_page_scroll();

     if (scroll_by == 0) 
       ctx->m_scroll_y = 0;
     else
      ctx->m_scroll_y -= scroll_by;
     update();

     return;
  }

  if (a_event->key() == Qt::Key_Down) {
     ctx->m_cursor.movePosition(QTextCursor::Down);
     update();

     int scroll_by = ctx->needs_page_scroll();
     if (scroll_by == 0) 
       ctx->m_scroll_y = ctx->m_scroll_y;
     else
       ctx->m_scroll_y += scroll_by;

     update();

    return;
  }
 
  if (a_event->key() == Qt::Key_Backspace) {
     ctx->m_cursor.deletePreviousChar();
     update();
     return;
  }
 
  if (a_event->key() == Qt::Key_Delete) {
     ctx->m_cursor.deleteChar();
     update();
     return;
  }

  ctx->m_cursor.insertText(a_event->text().at(0));
  update();
}

void text_view::mousePressEvent(QGraphicsSceneMouseEvent *a_event) {
  QPointF pos = a_event->pos();

  int cursor_pos = ctx->hit_test(pos.x(), pos.y() + ctx->m_scroll_y); 

  if (cursor_pos >= 0) {
    ctx->m_cursor.clearSelection();
    ctx->m_cursor.setPosition(cursor_pos, QTextCursor::MoveAnchor);
    ctx->m_last_anchor = ctx->m_cursor.positionInBlock();
    ctx->m_anchor_block = ctx->m_doc.findBlock(ctx->m_cursor.anchor());
  }
 
  update();
  a_event->accept();
}

void text_view::mouseMoveEvent(QGraphicsSceneMouseEvent *a_event) {
  QPointF pos = a_event->pos();

  int cursor_pos = ctx->hit_test(pos.x(), pos.y() + ctx->m_scroll_y); 

  if (cursor_pos >= 0) {
     // if (a_event->button() == Qt::LeftButton) {
        ctx->m_cursor.setPosition(cursor_pos, QTextCursor::KeepAnchor);
     // }
  }
 
  update();
  a_event->accept();
}

void text_view::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event) {
  QPointF pos = a_event->pos();

  int cursor_pos = ctx->hit_test(pos.x(), pos.y() + ctx->m_scroll_y); 

  if (cursor_pos >= 0) {
    //ctx->m_cursor.setPosition(cursor_pos, QTextCursor::KeepAnchor);
  }
 
  update();
  a_event->accept();
}

int text_view::text_view_context::needs_page_scroll() {
  int current_cursor = m_cursor.position();
  float view_height = m_doc.pageSize().height();
  float document_height = m_doc.size().height();
  const QTextBlock current_block = 
    m_doc.findBlock(m_cursor.position()); 
  QTextLayout *current_layout = current_block.layout();

  if (!current_layout)
    return 0;
 
  if (m_cursor.atEnd())
    return 0;

  QTextLine current_line = current_layout->lineForTextPosition(m_cursor.positionInBlock()); 
  QPointF current_block_pos = current_layout->position();
  float current_cursor_y = current_block_pos.y() + current_line.rect().y();
  
  if (current_cursor_y > view_height) {
    return ((current_cursor_y - view_height) < 0) ? -17 : 17;
  } 
  
  return 0;
}

int text_view::text_view_context::hit_test(float a_x, float a_y) {
  int block_count = 0;
  QTextBlock block = m_doc.begin();

  while(block.isValid()) {
    QTextLayout *layout = block.layout();

    if (!layout) {
      block_count++;
      block = block.next();
      continue;
    }

    QRectF block_bound = layout->boundingRect();
    QPointF block_pos = layout->position();

    QPointF src_pos = QPointF(block_pos.x(), a_y);
   
    block_bound = QRectF(block_pos.x(), block_pos.y(),
                         block_bound.width(),
                         block_bound.height());

    if (!block_bound.contains(src_pos)){
      block_count++;
      block = block.next();
      continue;
    }

    for (int i = 0 ; i < layout->lineCount(); i++) {
      QTextLine line = layout->lineAt(i);
      
      if (!line.isValid())
       continue;
     
     QRectF line_bounds = line.rect();
     line_bounds = QRectF(block_bound.x(),
                          block_bound.y() + line_bounds.y(),
                          line_bounds.width(),
                          line_bounds.height());

     if (!line_bounds.contains(src_pos))
       continue;

      int line_num = line.xToCursor(a_x);
      return (block.position() + line_num);
    }

    block = block.next();
  }

  return -1;
}

}
