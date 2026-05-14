#include "schemadialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFont>
#include <QPolygon>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

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

void FlowchartWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), FC_BG);

    // Ничего не рисуем — картинка показывается в SchemaDialog через QLabel
}

// ──────────────────────────────────────────────────────────────
// FlowchartWidget
// ──────────────────────────────────────────────────────────────

FlowchartWidget::FlowchartWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(700, 735);
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

    imageLabel = new QLabel(this);
    QPixmap pixmap("Schema.png");

    if (!pixmap.isNull()) {
        // Масштабируем картинку под размер окна
        QPixmap scaled = pixmap.scaled(728, 695, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaled);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setScaledContents(true);
        mainLayout->addWidget(imageLabel);
    } else {
        // Если картинка не найдена - показываем сообщение
        QLabel *errorLabel = new QLabel("Файл Schema.png не найден!\nПоложите картинку в папку с программой", this);
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("color: red;");
        mainLayout->addWidget(errorLabel);
    }

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