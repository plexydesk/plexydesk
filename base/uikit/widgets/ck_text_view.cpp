#include "ck_text_view.h"

#include <QTextCursor>
#include <QTextLayout>
#include <QTextBlock>
#include <QTextDocument>
#include <QClipboard>
#include <QApplication>

#include <QGraphicsSceneMouseEvent>
#include <QAbstractTextDocumentLayout>

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
    text_view_context() : m_last_anchor(0), m_scroll_y(0.0), m_cursor_width(1) {
        m_font_height = font_metrics::font_height();
        m_font_width = font_metrics::font_width();
        m_background_color = "#ffffff";
        m_text_color = "#2b2b2b";
    }
    ~text_view_context() {}

    int hit_test(float, float);
    
    int needs_page_scroll();

    void notify_text_update();

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
    int m_cursor_width;
    
    /* style properties */
    std::string m_background_color;
    std::string m_text_color;

    /* update notify */
    std::vector<text_view::update_notify_callback_t> m_text_update_notify_chain;
};

text_view::text_view(widget *a_parent) : widget(a_parent),
    ctx(new text_view_context) {
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

QString text_view::text() const {
   return ctx->m_doc.toPlainText();
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
    background_path.addRoundRect(a_rect, 2, 2);

    /* use Qt */
    a_painter->save();
    a_painter->setClipRect(a_rect);
    a_painter->setRenderHint(QPainter::TextAntialiasing, true);
    a_painter->setRenderHint(QPainter::Antialiasing, true);
    a_painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    a_painter->fillPath(background_path, QColor(ctx->m_background_color.c_str()));

    a_painter->save();
    a_painter->setOpacity(0.3);
    //a_painter->fillRect(QRect(0, 0, 16.0, a_rect.height()), QColor("#888888"));
    a_painter->restore();

    QRectF document_geometry = QRectF(a_rect.x(),
                                      a_rect.y(),
                                      a_rect.width(),
                                      a_rect.height() + ctx->m_scroll_y);
    a_painter->translate(a_rect.x(), a_rect.y() - (ctx->m_scroll_y));

    a_painter->save();
    QPen text_pen;
    text_pen.setColor(QColor(ctx->m_text_color.c_str()));
    a_painter->setPen(text_pen);

    QAbstractTextDocumentLayout::PaintContext _doc_ctx;
    _doc_ctx.palette.setColor(QPalette::Text, a_painter->pen().color());

    ctx->m_doc.documentLayout()->draw(a_painter, _doc_ctx);
    a_painter->restore();
        
    const QTextBlock current_block = 
      ctx->m_doc.findBlock(ctx->m_cursor.position()); 

    if (!current_block.isValid() || ctx->m_doc.isEmpty()) {
      text_pen.setColor(QColor(ctx->m_text_color.c_str()));
      a_painter->setPen(text_pen);
      a_painter->drawRect(QRect(4, 0, 1, 17));
      a_painter->restore();
      a_painter->restore();
      return;
    }
    
    QTextLayout *current_layout = current_block.layout();

    if (current_layout && hasFocus() && !ctx->m_cursor.isNull() && !ctx->m_cursor.hasSelection()) {
      QTextLine current_line = current_layout->lineForTextPosition(ctx->m_cursor.positionInBlock()); 

      if (current_layout->lineCount() == 0 || current_line.width() <= 0) {
        a_painter->restore();
        a_painter->restore();
        return;
      }

      QPointF current_block_pos = current_layout->position();
      qreal current_cursor_x = (current_line.cursorToX(ctx->m_cursor.positionInBlock()) + current_block_pos.x());
      qreal current_cursor_y = current_block_pos.y() + current_line.rect().y();
      qreal leading = current_line.rect().height();

      a_painter->save();
      QPen cursor_pen;
      QPainterPath text_cursor_path; 

      QLinearGradient cursor_fill(QPointF(ctx->m_cursor_width / 2, 0), 
                                   QPointF(ctx->m_cursor_width / 2, leading));

      cursor_fill.setColorAt(0, QColor("#EFEFEF"));
      cursor_fill.setColorAt(0.50, QColor("#A0C7F1"));
      cursor_fill.setColorAt(0.70, QColor("#87BAF2"));
      cursor_fill.setColorAt(1, QColor("#C9F5FC"));

      cursor_pen.setColor(QColor("#F65C9F"));
           
      a_painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
      text_cursor_path.addRoundRect(QRectF(0, 0, ctx->m_cursor_width, leading), 1, 1);
      a_painter->translate(current_cursor_x, current_cursor_y);

      a_painter->setOpacity(0.8);
      a_painter->fillPath(text_cursor_path, QBrush(cursor_fill));

      a_painter->setOpacity(0.4);
      text_pen.setColor(QColor(ctx->m_text_color.c_str()));
      a_painter->setPen(text_pen);
      a_painter->drawPath(text_cursor_path);
      a_painter->restore();
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
         //a_painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
         a_painter->setOpacity(0.5);
         QRectF selection_rect = selection_path.boundingRect();
         QRectF selection_rect_adjusted = QRectF(0, 0, selection_rect.width(), selection_rect.height());

         QLinearGradient selection_fill(QPointF(selection_rect.width() / 2, selection_rect.y()), 
                                        QPointF(selection_rect.width() / 2, 
                                        selection_rect.y() + selection_rect.height()));
         selection_fill.setColorAt(0, QColor("#E5F0FC"));
         selection_fill.setColorAt(0.50, QColor("#A0C7F1"));
         //selection_fill.setColorAt(0.70, QColor("#87BAF2"));
         selection_fill.setColorAt(0.70, QColor("#98C7F9"));
         selection_fill.setColorAt(1, QColor("#C9F5FC"));

         a_painter->fillPath(selection_path.simplified(), QBrush(selection_fill));
         
         QPen cursor_pen;
         cursor_pen.setColor(Qt::blue);
         a_painter->setPen(cursor_pen);
         a_painter->setOpacity(0.2);
         //a_painter->strokePath(selection_path.simplified(), a_painter->pen());

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
     ctx->m_cursor_width = 10;
     update();
     return;
  }

  if (a_event->key() == Qt::Key_Right) {
     ctx->m_cursor.movePosition(QTextCursor::Right);
     ctx->m_cursor_width = 10;
     update();
     return;
  }

  if (a_event->key() == Qt::Key_Up) {
     ctx->m_cursor.movePosition(QTextCursor::Up);
     ctx->m_cursor_width = 10;
     scroll_up();
     return;
  }

  if (a_event->key() == Qt::Key_Down) {
     ctx->m_cursor.movePosition(QTextCursor::Down);
     ctx->m_cursor_width = 10;
     scroll_down();
    return;
  }
 
  if (a_event->key() == Qt::Key_Backspace) {
     ctx->m_cursor.deletePreviousChar();
     ctx->m_cursor_width = 1;
     update();
     return;
  }
 
  if (a_event->key() == Qt::Key_Delete) {
     ctx->m_cursor.deleteChar();
     ctx->m_cursor_width = 1;
     update();
     return;
  }

  if (a_event->key() == Qt::Key_Shift) {
    return;
  }
  
  if (a_event->key() == Qt::Key_Enter) { 
   ctx->m_cursor_width = 1;
   scroll_down();
  }
 
  if (a_event->key() == Qt::Key_Return) { 
   ctx->m_cursor_width = 1;
   scroll_down();
  }

  ctx->m_cursor.insertText(a_event->text().at(0));
  ctx->m_cursor_width = 1;
  update();
  ctx->notify_text_update();
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
    return ((current_cursor_y - view_height) < 0) ? -32 : 32;
  } 
  
  return 0;
}

void text_view::text_view_context::notify_text_update()
{
    std::for_each(std::begin(m_text_update_notify_chain),
                  std::end(m_text_update_notify_chain), [this](text_view::update_notify_callback_t a_callback) {
        if (a_callback)
            a_callback();
    });
}

void text_view::scroll_up() {
  update();
  int scroll_by = ctx->needs_page_scroll();

  if (scroll_by == 0) 
    ctx->m_scroll_y = 0;
  else
   ctx->m_scroll_y -= scroll_by;
  update();
}

void text_view::scroll_down() {
  update();
  int scroll_by = ctx->needs_page_scroll();

  if (scroll_by == 0) 
    ctx->m_scroll_y = ctx->m_scroll_y;
  else
    ctx->m_scroll_y += scroll_by;

  update();
}

void text_view::set_background_color(const std::string &a_color) {
  ctx->m_background_color = a_color;
  update();
} 

void text_view::set_text_color(const std::string &a_color) {
  ctx->m_text_color = a_color;
  update();
} 

void text_view::copy() {
    QClipboard *clipboard = QApplication::clipboard();

    if (clipboard)
        clipboard->setText(ctx->m_doc.toPlainText());
}

void text_view::paste() {
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
      ctx->m_cursor.insertText(clipboard->text());

    ctx->notify_text_update();
}

void text_view::on_text_changed(text_view::update_notify_callback_t a_callback) {
  ctx->m_text_update_notify_chain.push_back(a_callback);
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
