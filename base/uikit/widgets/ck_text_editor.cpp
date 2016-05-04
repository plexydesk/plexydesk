#include "ck_text_editor.h"
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QAbstractScrollArea>
#include <QObject>
#include <QTextBlock>
#include <QTextBrowser>
#include <QClipboard>
#include <QMimeData>
#include <QTextFragment>
#include <QTextDocumentFragment>
#include <QDebug>
#include <QApplication>

#include <ck_widget.h>
#include <ck_resource_manager.h>
#include <ck_scrollbar.h>
#include <QScrollBar>

namespace cherry_kit {

class PrivateTextBrowser : public QTextEdit {
public:
  PrivateTextBrowser(QWidget *parent = 0) : QTextEdit(parent) {}
  virtual ~PrivateTextBrowser() {}

  void scrollContents(int dx, int dy) {
    if (dy < 0)
      moveCursor(QTextCursor::Down, QTextCursor::MoveAnchor);
    else
      moveCursor(QTextCursor::Up, QTextCursor::MoveAnchor);
  }
};

class text_editor::PrivateTextEditor {
public:
  PrivateTextEditor() {}
  ~PrivateTextEditor() {}

  QString extractHeader(const QString &headerText);

  QGraphicsProxyWidget *m_proxy_widget;
  PrivateTextBrowser *m_text_editor;
  float m_text_scale_factor;

  cherry_kit::scrollbar *m_v_scrollbar;
};

void text_editor::update_scrollbars() {
  priv->m_v_scrollbar->set_size(
      QSizeF(16, priv->m_proxy_widget->geometry().height()));
  priv->m_v_scrollbar->setPos(priv->m_proxy_widget->geometry().width() - 16, 0);

  priv->m_v_scrollbar->set_maximum_value(
      priv->m_text_editor->document()->size().height());
}

text_editor::text_editor(widget *parent)
    : cherry_kit::widget(parent), priv(new PrivateTextEditor) {
  priv->m_proxy_widget = new QGraphicsProxyWidget(this);
  priv->m_text_editor = new PrivateTextBrowser(0);
  priv->m_text_editor->setFontPointSize(14);
  priv->m_text_editor->setReadOnly(false);
  priv->m_text_editor->setAcceptRichText(true);
  priv->m_text_editor->setAutoFormatting(QTextEdit::AutoAll);
  priv->m_text_editor->setStyleSheet("background-color: #ffffff;"
                                     "border : 0");
  priv->m_text_editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  priv->m_text_editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  priv->m_proxy_widget->setWidget(priv->m_text_editor);

  priv->m_proxy_widget->show();
  priv->m_text_editor->move(0.0, 0.0);
  priv->m_proxy_widget->setPos(0.0, 0.0);

  priv->m_text_scale_factor = resource_manager::style()->scale_factor();
  set_widget_flag(cherry_kit::widget::kRenderDropShadow, false);
  set_widget_flag(cherry_kit::widget::kRenderBackground, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  connect(priv->m_text_editor, SIGNAL(textChanged()), this,
          SLOT(on_text_updated()));
  connect(priv->m_text_editor->document(), SIGNAL(blockCountChanged(int)), this,
          SLOT(on_block_count_changed(int)));

  priv->m_v_scrollbar = new scrollbar(this);
  priv->m_v_scrollbar->setZValue(priv->m_proxy_widget->zValue() + 2);
  priv->m_v_scrollbar->set_page_step(1);

  priv->m_v_scrollbar->on_value_changed([this](int value) {
    priv->m_text_editor->scrollContents(0, value);
  });

  update_scrollbars();
}

text_editor::~text_editor() { delete priv; }

void text_editor::set_text(const QString &a_text) {
  priv->m_text_editor->setText(a_text);
}

void text_editor::set_placeholder_text(const QString &a_placeholderText) {
  // if (d->mEditor)
  //   d->mEditor->setPlaceholderText(placeholderText);
}

void text_editor::set_font_point_size(qreal a_s) {
  if (priv->m_text_editor) {
    priv->m_text_editor->setFontPointSize(a_s);
  }
}

QString text_editor::text() const {
  if (priv->m_text_editor) {
    return priv->m_text_editor->toPlainText();
  }

  return QString();
}

void text_editor::style(const QString &style) {
  if (priv->m_text_editor) {
    priv->m_text_editor->setStyleSheet(style);
  }
}

QSizeF text_editor::sizeHint(Qt::SizeHint which,
                             const QSizeF &constraint) const {
  return priv->m_text_editor->size();
}

void text_editor::set_geometry(const QRectF &a_rect) {
  priv->m_proxy_widget->setMinimumSize(a_rect.size());
  priv->m_proxy_widget->setMaximumSize(a_rect.size());
  priv->m_proxy_widget->resize(a_rect.size());
  priv->m_text_editor->resize(a_rect.width(), a_rect.height());
  priv->m_text_editor->move(0.0, 0.0);

  widget::set_geometry(a_rect);
  update_scrollbars();
}

void text_editor::update_text_scale() {
  QRectF bounds = contents_geometry();
  const QRectF newBounds(bounds.x(), bounds.y(),
                         bounds.width() / priv->m_text_scale_factor,
                         bounds.height() / priv->m_text_scale_factor);
  set_geometry(newBounds);

  priv->m_proxy_widget->setMinimumSize(newBounds.size());
  priv->m_proxy_widget->setMaximumSize(newBounds.size());

  priv->m_proxy_widget->setGeometry(newBounds);
  priv->m_proxy_widget->resize(newBounds.size());

  setScale(priv->m_text_scale_factor);
}

void text_editor::begin_list() {
  if (!priv->m_text_editor) {
    return;
  }

  QTextCursor cursor = priv->m_text_editor->textCursor();

  cursor.beginEditBlock();
  cursor.insertList(QTextListFormat::ListCircle);
  cursor.insertText("");
  cursor.endEditBlock();
}

void text_editor::end_list() {
  if (!priv->m_text_editor) {
    return;
  }
}

void text_editor::convert_to_link() {
  if (!priv->m_text_editor) {
    return;
  }

  QTextCursor cursor = priv->m_text_editor->textCursor();

  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  if (mimeData->hasUrls()) {
    Q_FOREACH(const QUrl & url, mimeData->urls()) {
      qDebug() << Q_FUNC_INFO << url;
      cursor.beginEditBlock();
      cursor.insertFragment(QTextDocumentFragment::fromHtml(
          QString("<a href=\"%1\"> %2</a> <p> </p>").arg(url.toString()).arg(
              url.toString())));
      cursor.endEditBlock();
    }
  } else if (mimeData->hasText()) {
    QString data = mimeData->text();
    QString proto = data.left(5);
    QUrl testUrl(data);

    if (proto == "http:" || proto == "https" || proto == "ftp:/") {
      cursor.beginEditBlock();
      cursor.insertFragment(QTextDocumentFragment::fromHtml(
          QString("<a href=\"%1\">%2</a> <p> </p>").arg(testUrl.toString()).arg(
              testUrl.toString())));
      cursor.endEditBlock();
    }
  }
}

void text_editor::on_text_updated() {
  QTextDocument *doc = priv->m_text_editor->document();

  Q_EMIT text_updated(doc->toPlainText());
  update_scrollbars();
}

void text_editor::on_block_count_changed(int a_count) {
  if (a_count == 2) {
    QTextDocument *document = priv->m_text_editor->document();
    QTextCursor cursor(document);

    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    // format.setFontPointSize (18);

    cursor.mergeCharFormat(format); // do the text as Bold
    Q_EMIT document_title_available(document->firstBlock().text());
  }
}

void text_editor::set_text_scale_factor(qreal a_scale_factor) {
  priv->m_text_scale_factor = a_scale_factor;
  update_text_scale();
}

qreal text_editor::text_scale_factor() const {
  return priv->m_text_scale_factor;
}

QString
text_editor::PrivateTextEditor::extractHeader(const QString &headerText) {
  QString headerString = QString("<h1><b>" + headerText + "</b></h1>");

  return headerString;
}
}
