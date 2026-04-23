#include "schemadialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFont>
#include <QPolygon>
#include <QPushButton>

// ── UI STUB SWITCH ───────────────────────────────────────────
#ifndef UI_STUB
#define UI_STUB 1
#endif

// ── COLORS ───────────────────────────────────────────────────
#define FC_BG      QColor(0x0b, 0x1e, 0x2d)
#define FC_BORDER  QColor(0x1f, 0x4e, 0x79)
#define FC_TEXT    QColor(0xe6, 0xf2, 0xff)
#define FC_ARROW   QColor(0x9e, 0xca, 0xe6)
#define FC_LABEL   QColor(0x74, 0xc0, 0xfc)

#define FONT_FAMILY "Segoe UI"

// ──────────────────────────────────────────────────────────────
// FlowchartWidget
// ──────────────────────────────────────────────────────────────

FlowchartWidget::FlowchartWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(700, 735);
}

void FlowchartWidget::drawRoundedBlock(QPainter &p,
                                       int cx, int cy, int w, int h,
                                       const QString &text,
                                       const QColor &fill,
                                       const QColor &border)
{
    QRect rect(cx - w/2, cy - h/2, w, h);

    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);

    p.fillPath(path, fill);
    p.setPen(QPen(border, 1.5));
    p.drawPath(path);

    p.setPen(FC_TEXT);
    p.drawText(rect, Qt::AlignCenter, text);
}

void FlowchartWidget::drawDiamond(QPainter &p,
                                  int cx, int cy, int w, int h,
                                  const QString &text,
                                  const QColor &fill,
                                  const QColor &border)
{
    QPolygon d;
    d << QPoint(cx, cy - h/2)
      << QPoint(cx + w/2, cy)
      << QPoint(cx, cy + h/2)
      << QPoint(cx - w/2, cy);

    QPainterPath path;
    path.addPolygon(d);

    p.fillPath(path, fill);
    p.setPen(QPen(border, 1.5));
    p.drawPath(path);

    p.setPen(FC_TEXT);
    p.drawText(QRect(cx - w/2, cy - h/2, w, h),
               Qt::AlignCenter,
               text);
}

void FlowchartWidget::drawArrowDown(QPainter &p, int cx, int y1, int y2)
{
    p.setPen(QPen(FC_ARROW, 2));
    p.drawLine(cx, y1, cx, y2);
}

void FlowchartWidget::drawArrowRight(QPainter &p, int x1, int x2, int y)
{
    p.setPen(QPen(FC_ARROW, 2));
    p.drawLine(x1, y, x2, y);
}

void FlowchartWidget::drawArrowLine(QPainter &p, int x1, int y1, int x2, int y2)
{
    p.setPen(QPen(FC_ARROW, 2));
    p.drawLine(x1, y1, x2, y2);
}

void FlowchartWidget::drawText(QPainter &p,
                               int cx, int cy, int w, int h,
                               const QString &text)
{
    QRect r(cx - w/2, cy - h/2, w, h);
    p.setPen(FC_LABEL);
    p.drawText(r, Qt::AlignCenter, text);
}

// ──────────────────────────────────────────────────────────────
// paintEvent (ОБЯЗАТЕЛЬНО: только ОДИН QPainter)
// ──────────────────────────────────────────────────────────────

void FlowchartWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), FC_BG);

#if UI_STUB
    p.setPen(FC_TEXT);
    p.drawText(rect(), Qt::AlignCenter,
               "Flowchart UI Stub Mode\n(без логики backend)");
    return;
#endif

    int cx = width() / 2;

    drawRoundedBlock(p, cx, 60, 200, 50, "Start", FC_BG, FC_BORDER);
    drawArrowDown(p, cx, 85, 130);

    drawDiamond(p, cx, 170, 160, 80, "Condition?", FC_BG, FC_BORDER);
    drawArrowDown(p, cx, 210, 260);

    drawRoundedBlock(p, cx, 300, 220, 60, "Process", FC_BG, FC_BORDER);
}

// ──────────────────────────────────────────────────────────────
// SchemaDialog
// ──────────────────────────────────────────────────────────────

SchemaDialog::SchemaDialog(QWidget *parent)
    : QDialog(parent)
{
    setStyleSheet(QString(
                      "QDialog { background-color: #0b1e2d; color: #e6f2ff; font-family:'%1'; font-weight: bold; }"
                      ).arg(FONT_FAMILY));

    setupUI();
}

SchemaDialog::~SchemaDialog() {}

void SchemaDialog::setupUI()
{
    setWindowTitle("Блок-схема вычислительного процесса");

    setFixedSize(760, 795);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    canvas = new FlowchartWidget(this);
    mainLayout->addWidget(canvas);

    QPushButton *closeBtn = new QPushButton("Закрыть", this);
    closeBtn->setMinimumHeight(36);
    closeBtn->setStyleSheet(
        "QPushButton{background:#3da9fc;color:#fff;border-radius:6px;padding:6px;font-weight: bold;font-size:12pt;}"
        "QPushButton:hover{background:#74c0fc;}"
        );

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);
    btnRow->addStretch();

    mainLayout->addLayout(btnRow);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

#if UI_STUB
void SchemaDialog::enableStubMode(bool enable)
{
    m_stubMode = enable;
    canvas->update();
}
#endif