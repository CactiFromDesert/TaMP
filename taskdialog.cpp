#include "taskdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

// ── Sky-blue palette ─────────────────────────────────────
#define GH_BG          "#0b1e2d"
#define GH_CARD        "#102a43"
#define GH_BORDER      "#1f4e79"
#define GH_TEXT        "#e6f2ff"
#define GH_MUTED       "#9ecae6"
#define GH_BLUE        "#3da9fc"
#define GH_BLUE_H      "#74c0fc"
#define FONT_FAMILY    "Segoe UI"

TaskDialog::TaskDialog(QWidget *parent)
    : QDialog(parent)
{
    setStyleSheet(QString(
                      "QDialog { background-color: %1; color: %2; font-family: '%3'; font-size: 10pt; font-weight: bold; }"
                      "QLabel  { background: transparent; color: %2; font-weight: bold; }"
                      ).arg(GH_BG).arg(GH_TEXT).arg(FONT_FAMILY));

    setupUI();
}

TaskDialog::~TaskDialog() {}

void TaskDialog::setupUI()
{
    setWindowTitle("Задание — Подгруппа 2");
    setFixedSize(560, 720);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 20);
    mainLayout->setSpacing(12);

    auto makeSep = [&]() {
        QFrame *sep = new QFrame(this);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet(QString(
                               "QFrame { background: %1; border: none; max-height: 1px; font-weight: bold; }"
                               ).arg(GH_BORDER));
        return sep;
    };

    // ── Title ────────────────────────────────────────────────
    titleLabel = new QLabel(this);
    titleLabel->setText(QString(
                            "<b style='font-size:17.5pt; color:%1; font-weight: bold;'>Задание</b>"
                            ).arg(GH_TEXT));
    titleLabel->setTextFormat(Qt::RichText);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(makeSep());

    // ── Work title ───────────────────────────────────────────
    workTitleLabel = new QLabel(this);
    workTitleLabel->setText(
        "<p style='font-size:12pt; font-weight: bold;'>"
        "Графическое отображение ветвящейся функции<br>"
        "в рамках клиент-серверного проекта"
        "</p>"
        );
    workTitleLabel->setTextFormat(Qt::RichText);
    workTitleLabel->setWordWrap(true);
    workTitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(workTitleLabel);

    // ── Group ────────────────────────────────────────────────
    groupLabel = new QLabel(this);
    groupLabel->setText(
        "<p style='font-size:17.5pt; font-weight: bold;'><b>Подгруппа 2</b></p>"
        );
    groupLabel->setTextFormat(Qt::RichText);
    groupLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(groupLabel);

    // ── Members ──────────────────────────────────────────────
    membersLabel = new QLabel(this);
    membersLabel->setText(
        "<p style='font-size:17.5pt; font-weight: bold;'>"
        "<b>Участники:</b><br>"
        "<span style='font-size:12pt;'>● Миханошин Дмитрий<br>"
        "● Акимова Ангелина<br>"
        "● Сорокина Алина<br>"
        "● Николаев Максим<br></span>"
        "</p>"
        );
    membersLabel->setTextFormat(Qt::RichText);
    membersLabel->setWordWrap(true);
    membersLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    membersLabel->setStyleSheet(QString(
                                    "QLabel { background-color:%1; border:1px solid %2; border-radius:8px; padding:14px; font-weight: bold; }"
                                    ).arg(GH_CARD, GH_BORDER));
    mainLayout->addWidget(membersLabel);

    mainLayout->addWidget(makeSep());

    // ── Formula ──────────────────────────────────────────────
    formulaLabel = new QLabel(this);
    formulaLabel->setText(
        "<p style='font-size:17.5pt; font-weight: bold;'>"
        "<b>Функция №10:</b><br>"
        "<span style='color:#ff6b6b; font-weight: bold; font-size:12pt;'>&bull; &radic;(-x) + a, x &lt; 0 </span><br>"
        "<span style='color:#4cd4b0; font-weight: bold; font-size:12pt;'>&bull; arcsin(x / π) + b, 0 ≤ x < π</span><br>"
        "<span style='color:#74c0fc; font-weight: bold; font-size:12pt;'>&bull; arccos(x / π) - c, x ≥ π</span>"
        "</p>"
        );
    formulaLabel->setTextFormat(Qt::RichText);
    formulaLabel->setWordWrap(true);
    formulaLabel->setAlignment(Qt::AlignLeft);
    formulaLabel->setStyleSheet(QString(
                                    "QLabel { background-color:%1; border:1px solid %2; border-radius:8px; padding:14px; font-weight: bold; }"
                                    ).arg(GH_BG, GH_BORDER));
    mainLayout->addWidget(formulaLabel);

    mainLayout->addStretch(1);

    // ── Close button ─────────────────────────────────────────
    closeBtn = new QPushButton("Закрыть", this);
    closeBtn->setMinimumHeight(36);
    closeBtn->setStyleSheet(QString(
                                "QPushButton {"
                                " background-color:%1; color:#fff;"
                                " border:1px solid rgba(240,246,252,0.1);"
                                " border-radius:6px;"
                                " font-size:12pt;"
                                " padding:4px 20px;"
                                " font-family:'%3';"
                                " font-weight: bold;"
                                "}"
                                "QPushButton:hover { background-color:%2; }"
                                ).arg(GH_BLUE, GH_BLUE_H, FONT_FAMILY));

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);
    btnRow->addStretch();

    mainLayout->addLayout(btnRow);

    setLayout(mainLayout);
}