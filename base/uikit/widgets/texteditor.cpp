#include "texteditor.h"
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

#include <widget.h>
#include <resource_manager.h>

namespace UIKit {

class TextEditor::PrivateTextEditor {
public:
  PrivateTextEditor() {}
  ~PrivateTextEditor() {}

  QString extractHeader(const QString &headerText);

  QGraphicsProxyWidget *mProxyWidget;
  QTextBrowser *mEditor;
  qreal mTextScaleFactor;
};

TextEditor::TextEditor(Widget *parent)
    : UIKit::Widget(parent), d(new PrivateTextEditor) {
  d->mProxyWidget = new QGraphicsProxyWidget(this);
  d->mEditor = new QTextBrowser(0);
  d->mEditor->setFontPointSize(16 * ResourceManager::style()->scale_factor());
  d->mEditor->setReadOnly(false);
  d->mEditor->setAcceptRichText(true);
  d->mEditor->setAutoFormatting(QTextEdit::AutoAll);
  d->mEditor->setStyleSheet("background-color: #ffffff;"
                            "border : 0");
  d->mEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->mEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  d->mProxyWidget->setWidget(d->mEditor);

  d->mProxyWidget->show();
  d->mEditor->move(0.0, 0.0);
  d->mProxyWidget->setPos(0.0, 0.0);

  d->mTextScaleFactor = 1.0;
  set_widget_flag(UIKit::Widget::kRenderDropShadow, false);
  set_widget_flag(UIKit::Widget::kRenderBackground, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  connect(d->mEditor, SIGNAL(textChanged()), this, SLOT(on_text_updated()));
  connect(d->mEditor->document(), SIGNAL(blockCountChanged(int)), this,
          SLOT(on_block_count_changed(int)));
}

TextEditor::~TextEditor() { delete d; }

void TextEditor::set_text(const QString &a_text) {
  d->mEditor->setText(a_text);
}

void TextEditor::set_placeholder_text(const QString &a_placeholderText) {
  // if (d->mEditor)
  //   d->mEditor->setPlaceholderText(placeholderText);
}

void TextEditor::set_font_point_size(qreal a_s) {
  if (d->mEditor) {
    d->mEditor->setFontPointSize(a_s);
  }
}

QString TextEditor::text() const {
  if (d->mEditor) {
    return d->mEditor->toPlainText();
  }

  return QString();
}

void TextEditor::style(const QString &style) {
  if (d->mEditor) {
    d->mEditor->setStyleSheet(style);
  }
}

QSizeF TextEditor::sizeHint(Qt::SizeHint which,
                            const QSizeF &constraint) const {
  return d->mEditor->size();
}

void TextEditor::setGeometry(const QRectF &a_rect) {
  d->mProxyWidget->setMinimumSize(a_rect.size());
  d->mProxyWidget->setMaximumSize(a_rect.size());
  d->mProxyWidget->resize(a_rect.size());
  d->mEditor->move(0.0, 0.0);

  Widget::setGeometry(a_rect);
}

void TextEditor::update_text_scale() {
  QRectF bounds = boundingRect();
  const QRectF newBounds(bounds.x(), bounds.y(),
                         bounds.width() / d->mTextScaleFactor,
                         bounds.height() / d->mTextScaleFactor);
  setGeometry(newBounds);

  d->mProxyWidget->setMinimumSize(newBounds.size());
  d->mProxyWidget->setMaximumSize(newBounds.size());

  d->mProxyWidget->setGeometry(newBounds);
  d->mProxyWidget->resize(newBounds.size());

  setScale(d->mTextScaleFactor);
}

void TextEditor::begin_list() {
  if (!d->mEditor) {
    return;
  }

  QTextCursor cursor = d->mEditor->textCursor();

  cursor.beginEditBlock();
  cursor.insertList(QTextListFormat::ListCircle);
  cursor.insertText("");
  cursor.endEditBlock();
}

void TextEditor::end_list() {
  if (!d->mEditor) {
    return;
  }
}

void TextEditor::convert_to_link() {
  if (!d->mEditor) {
    return;
  }

  QTextCursor cursor = d->mEditor->textCursor();

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

void TextEditor::on_text_updated() {
  QTextDocument *doc = d->mEditor->document();

  Q_EMIT text_updated(doc->toPlainText());
}

void TextEditor::on_block_count_changed(int a_count) {
  if (a_count == 2) {
    QTextDocument *document = d->mEditor->document();
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

void TextEditor::set_text_scale_factor(qreal a_scale_factor) {
  d->mTextScaleFactor = a_scale_factor;
  update_text_scale();
}

qreal TextEditor::text_scale_factor() const { return d->mTextScaleFactor; }

QString TextEditor::PrivateTextEditor::extractHeader(
    const QString &headerText) {
  QString headerString = QString("<h1><b>" + headerText + "</b></h1>");

  return headerString;
}
}
