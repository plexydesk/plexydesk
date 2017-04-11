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
    text_view_context() {
        m_font_height = font_metrics::font_height();
        m_font_width = font_metrics::font_width();
    }
    ~text_view_context() {}

    int hit_test(float, float);

    std::string m_text;
    font_metrics *m_engine;

    /* the visual caret pos */
    QPoint m_text_cursor_pos;
    QTextLayout *m_layout_mgr;

    int m_font_height;
    int m_font_width;

    QTextDocument m_doc;
    QTextCursor m_cursor;
};

text_view::text_view(widget *a_parent) : widget(a_parent),
    ctx(new text_view_context) {
    ctx->m_engine = new font_metrics();
    ctx->m_layout_mgr = new QTextLayout();
}

text_view::~text_view(){
    delete ctx;
}

void text_view::set_text(const std::__cxx11::string &a_text) {
   ctx->m_text = a_text;
   ctx->m_doc.setPageSize(geometry().size());
   ctx->m_cursor = QTextCursor(&ctx->m_doc); 
   ctx->m_cursor.setVisualNavigation(true);
   ctx->m_cursor.insertText(a_text.c_str());
   update();
}

void text_view::paint_view(QPainter *a_painter, const QRectF &a_rect) {
    a_painter->fillRect(a_rect, Qt::white);
    /* use Qt */

    a_painter->save();
    a_painter->setClipRect(a_rect);
    a_painter->setRenderHint(QPainter::TextAntialiasing, true);
    a_painter->setRenderHint(QPainter::Antialiasing, true);
    a_painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    ctx->m_doc.drawContents(a_painter, a_rect);
    
    const QTextBlock current_block = ctx->m_doc.findBlock(ctx->m_cursor.position()); //ctx->m_cursor.block(); 
    QTextLayout *current_layout = current_block.layout();

    current_layout->drawCursor(a_painter,
                                  QPointF(),
                                  ctx->m_cursor.positionInBlock(),
                                  2);
    a_painter->restore();
}

void text_view::keyPressEvent(QKeyEvent *a_event)
{
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
     return;
  }

  if (a_event->key() == Qt::Key_Down) {
     ctx->m_cursor.movePosition(QTextCursor::Down);
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

  ctx->m_cursor.clearSelection();
  ctx->m_cursor.insertText(a_event->text().at(0));
  update();
}

void text_view::mousePressEvent(QGraphicsSceneMouseEvent *a_event) {
  QPointF pos = a_event->pos();

  int cursor_pos = ctx->hit_test(pos.x(), pos.y()); 

  if (cursor_pos > 0) {
    ctx->m_cursor.setPosition(cursor_pos, QTextCursor::MoveAnchor);
  }
 
  update();
  a_event->accept();
}

int text_view::text_view_context::hit_test(float a_x, float a_y) {
  QTextBlock block = m_doc.begin();
  int block_count = 0;

  while(block.isValid()) {
    QTextLayout *layout = block.layout();

    if (!layout) {
      block_count++;
      block = block.next();
      continue;
    }

    QRectF block_bound = layout->boundingRect();
    QPointF block_pos = layout->position();

    QPointF src_pos = QPointF(block_pos.x() + 1, a_y);
   
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
