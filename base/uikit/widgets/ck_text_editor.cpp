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

namespace cherry_kit {

class text_editor::PrivateTextEditor {
public:
  PrivateTextEditor() {}
  ~PrivateTextEditor() {}

  QString extractHeader(const QString &headerText);

  QGraphicsProxyWidget *mProxyWidget;
  QTextBrowser *mEditor;
  qreal mTextScaleFactor;
};

text_editor::text_editor(widget *parent)
    : cherry_kit::widget(parent), o_text_editor(new PrivateTextEditor) {
  o_text_editor->mProxyWidget = new QGraphicsProxyWidget(this);
  o_text_editor->mEditor = new QTextBrowser(0);
  o_text_editor->mEditor->setFontPointSize(
      16 * resource_manager::style()->scale_factor());
  o_text_editor->mEditor->setReadOnly(false);
  o_text_editor->mEditor->setAcceptRichText(true);
  o_text_editor->mEditor->setAutoFormatting(QTextEdit::AutoAll);
  o_text_editor->mEditor->setStyleSheet("background-color: #ffffff;"
                                        "border : 0");
  o_text_editor->mEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  o_text_editor->mEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  o_text_editor->mProxyWidget->setWidget(o_text_editor->mEditor);

  o_text_editor->mProxyWidget->show();
  o_text_editor->mEditor->move(0.0, 0.0);
  o_text_editor->mProxyWidget->setPos(0.0, 0.0);

  o_text_editor->mTextScaleFactor = 1.0;
  set_widget_flag(cherry_kit::widget::kRenderDropShadow, false);
  set_widget_flag(cherry_kit::widget::kRenderBackground, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  connect(o_text_editor->mEditor, SIGNAL(textChanged()), this,
          SLOT(on_text_updated()));
  connect(o_text_editor->mEditor->document(), SIGNAL(blockCountChanged(int)),
          this, SLOT(on_block_count_changed(int)));
}

text_editor::~text_editor() { delete o_text_editor; }

void text_editor::set_text(const QString &a_text) {
  o_text_editor->mEditor->setText(a_text);
}

void text_editor::set_placeholder_text(const QString &a_placeholderText) {
  // if (d->mEditor)
  //   d->mEditor->setPlaceholderText(placeholderText);
}

void text_editor::set_font_point_size(qreal a_s) {
  if (o_text_editor->mEditor) {
    o_text_editor->mEditor->setFontPointSize(a_s);
  }
}

QString text_editor::text() const {
  if (o_text_editor->mEditor) {
    return o_text_editor->mEditor->toPlainText();
  }

  return QString();
}

void text_editor::style(const QString &style) {
  if (o_text_editor->mEditor) {
    o_text_editor->mEditor->setStyleSheet(style);
  }
}

QSizeF text_editor::sizeHint(Qt::SizeHint which,
                            const QSizeF &constraint) const {
  return o_text_editor->mEditor->size();
}

void text_editor::setGeometry(const QRectF &a_rect) {
  o_text_editor->mProxyWidget->setMinimumSize(a_rect.size());
  o_text_editor->mProxyWidget->setMaximumSize(a_rect.size());
  o_text_editor->mProxyWidget->resize(a_rect.size());
  o_text_editor->mEditor->resize(a_rect.width() - 10, a_rect.height());
  o_text_editor->mEditor->move(0.0, 0.0);

  widget::setGeometry(a_rect);
}

void text_editor::update_text_scale() {
  QRectF bounds = boundingRect();
  const QRectF newBounds(bounds.x(), bounds.y(),
                         bounds.width() / o_text_editor->mTextScaleFactor,
                         bounds.height() / o_text_editor->mTextScaleFactor);
  setGeometry(newBounds);

  o_text_editor->mProxyWidget->setMinimumSize(newBounds.size());
  o_text_editor->mProxyWidget->setMaximumSize(newBounds.size());

  o_text_editor->mProxyWidget->setGeometry(newBounds);
  o_text_editor->mProxyWidget->resize(newBounds.size());

  setScale(o_text_editor->mTextScaleFactor);
}

void text_editor::begin_list() {
  if (!o_text_editor->mEditor) {
    return;
  }

  QTextCursor cursor = o_text_editor->mEditor->textCursor();

  cursor.beginEditBlock();
  cursor.insertList(QTextListFormat::ListCircle);
  cursor.insertText("");
  cursor.endEditBlock();
}

void text_editor::end_list() {
  if (!o_text_editor->mEditor) {
    return;
  }
}

void text_editor::convert_to_link() {
  if (!o_text_editor->mEditor) {
    return;
  }

  QTextCursor cursor = o_text_editor->mEditor->textCursor();

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
  QTextDocument *doc = o_text_editor->mEditor->document();

  Q_EMIT text_updated(doc->toPlainText());
}

void text_editor::on_block_count_changed(int a_count) {
  if (a_count == 2) {
    QTextDocument *document = o_text_editor->mEditor->document();
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
  o_text_editor->mTextScaleFactor = a_scale_factor;
  update_text_scale();
}

qreal text_editor::text_scale_factor() const {
  return o_text_editor->mTextScaleFactor;
}

QString
text_editor::PrivateTextEditor::extractHeader(const QString &headerText) {
  QString headerString = QString("<h1><b>" + headerText + "</b></h1>");

  return headerString;
}
}
