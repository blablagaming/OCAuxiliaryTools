#include "tooltip.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>

#include "mainwindow.h"

extern MainWindow* mw_one;

Tooltip::Tooltip(QWidget* parent) : QDialog(parent) {
  setAttribute(Qt::WA_DeleteOnClose);  // tooltip关闭后自动销毁自身对象
  // this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
  // this->setAttribute(Qt::WA_TranslucentBackground, true);

  // this->setAutoFillBackground(true);
  // QPalette palette = this->palette();
  // palette.setColor(QPalette::Base, QColor(0, 0, 0, 0));
  // palette.setColor(QPalette::Window, QColor(0, 0, 0, 0));
  // palette.setColor(QPalette::Text, QColor(0, 0, 0, 255));
  // this->setPalette(palette);

  edit = new QTextEdit(this);
  edit->setWordWrapMode(QTextOption::WordWrap);
  edit->setReadOnly(true);

  QHBoxLayout* mLayout = new QHBoxLayout(this);
  mLayout->setMargin(0);
  mLayout->addWidget(edit);

  this->installEventFilter(this);

  thisWidth = 500;
  thisHeight = 250;

#ifdef Q_OS_WIN32
  thisWidth = 600;
  thisHeight = 300;
#endif

  this->resize(thisWidth, thisHeight);
}

Tooltip::~Tooltip() {}

void Tooltip::setMyText(QString strHead, const QString& text) {
  QString str;
  QStringList strList;

  if (text.contains("----")) {
    strList = text.split("----");
    if (mw_one->zh_cn) {
      str = strList.at(1);
    } else
      str = strList.at(0);
  } else
    str = text;

  this->setWindowTitle(strHead);
  edit->setText(str.trimmed());

  // 文本高度
  QTextDocument* document = edit->document();  // new QTextDocument(edit);
  document->setTextWidth(thisWidth);
  QTextOption op;
  op.setWrapMode(QTextOption::WordWrap);
  document->setDefaultTextOption(op);
  document->adjustSize();
  QAbstractTextDocumentLayout* layout = document->documentLayout();

  int newHeight = layout->documentSize().height();

  if (newHeight < thisHeight)
    currentHeight = newHeight;
  else
    currentHeight = thisHeight;

  edit->setFixedHeight(currentHeight * 1.05);
  this->setFixedHeight(currentHeight * 1.05);
}

bool Tooltip::eventFilter(QObject* obj, QEvent* e) {
  if (obj == this) {
    if (QEvent::WindowDeactivate == e->type()) {
      this->close();
      e->accept();
      return true;
    }
  }
  return QWidget::eventFilter(obj, e);
}

void Tooltip::popup(QPoint pos, QString strHead, const QString& text) {
  Tooltip* t = new Tooltip();
  t->setMyText(strHead, text);

  int newX;
  if (pos.x() + thisWidth > mw_one->getMainWidth())
    newX = pos.x() - thisWidth;
  else
    newX = pos.x();

  pos.setY(pos.y() - t->height());
  pos.setX(newX);

  t->move(pos);
  t->show();
}

void Tooltip::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(QBrush(QColor(0xff, 0xff, 0xe1)));
  // painter.setBrush(QBrush(Qt::lightGray));
  painter.setPen(Qt::black);
  QRect rect = this->rect();
  rect.setWidth(rect.width() - 1);
  rect.setHeight(rect.height() - 1);
  // painter.drawRoundedRect(rect, 15, 15);

  QPainterPath painterPath;
  if (mw_one->mac)
    painterPath.addRoundedRect(rect, 10, 10);
  else
    painterPath.addRoundedRect(rect, 10, 10);
  painter.drawPath(painterPath);

  QWidget::paintEvent(event);
}
