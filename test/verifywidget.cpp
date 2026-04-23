#include "verifywidget.h"
#include "clientsingleton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QFont>
#include <QFrame>

// ── GitHub dark palette ─────────────────────────────────────
#define GH_BG          "#0d1117"
#define GH_CARD        "#161b22"
#define GH_BORDER      "#30363d"
#define GH_TEXT        "#e6edf3"
#define GH_MUTED       "#8b949e"
#define GH_GREEN       "#238636"
#define GH_GREEN_H     "#2ea043"
#define GH_BLUE        "#388bfd"
#define GH_BLUE_H      "#58a6ff"
#define GH_RED         "#f85149"
#define GH_INPUT_BG    "#0d1117"

#define FONT_FAMILY     "Segoe UI"
#define FONT_SIZE_TITLE 16
#define FONT_SIZE_BTN   11
#define FONT_SIZE_INPUT 11
#define FONT_SIZE_SMALL 9

// ── Styles ──────────────────────────────────────────────────
static QString inputStyle()
{
    return QString(
               "QLineEdit {"
               " background-color:%1;"
               " color:%2;"
               " border:1px solid %3;"
               " border-radius:6px;"
               " padding:6px 10px;"
               " font-family:'%4';"
               " font-size:%5pt;"
               "}"
               "QLineEdit:focus { border-color:%6; }"
               ).arg(GH_INPUT_BG, GH_TEXT, GH_BORDER, FONT_FAMILY)
        .arg(FONT_SIZE_INPUT).arg(GH_BLUE);
}

static QString primaryBtnStyle()
{
    return QString(
               "QPushButton {"
               " background:%1;"
               " color:#fff;"
               " border-radius:6px;"
               " padding:6px 16px;"
               " font-weight:bold;"
               "}"
               "QPushButton:hover { background:%2; }"
               "QPushButton:disabled {"
               " background:rgba(35,134,54,0.35);"
               " color:rgba(255,255,255,0.4);"
               "}"
               ).arg(GH_GREEN, GH_GREEN_H);
}

static QString linkBtnStyle()
{
    return QString(
               "QPushButton {"
               " color:%1;"
               " border:none;"
               " background:transparent;"
               " font-family:'%2';"
               " font-size:%3pt;"
               "}"
               "QPushButton:hover {"
               " color:%4;"
               " text-decoration:underline;"
               "}"
               ).arg(GH_BLUE, FONT_FAMILY)
        .arg(FONT_SIZE_BTN)
        .arg(GH_BLUE_H);
}

// ── Constructor ──────────────────────────────────────────────
VerifyWidget::VerifyWidget(QWidget *parent)
    : QWidget(parent),
    failedAttempts(0),
    lockLevel(0),
    isLocked(false),
    m_waitingForVerify(false)
{
    lockTimer = new QTimer(this);
    lockTimer->setSingleShot(true);

    connect(lockTimer, &QTimer::timeout,
            this, &VerifyWidget::onLockTimerFired);

    connect(&ClientSingleton::instance(),
            &ClientSingleton::responseReceived,
            this,
            &VerifyWidget::onVerifyResponseReceived);

    setStyleSheet(QString(
                      "QWidget { background:%1; color:%2; font-family:'%3'; font-size:%4pt; }"
                      ).arg(GH_BG, GH_TEXT, FONT_FAMILY)
                      .arg(FONT_SIZE_INPUT));

    setupUI();
}

VerifyWidget::~VerifyWidget() {}

// ── Set login ───────────────────────────────────────────────
void VerifyWidget::setLogin(const QString &loginVal)
{
    login = loginVal;

    failedAttempts = 0;
    lockLevel = 0;
    isLocked = false;
    m_waitingForVerify = false;

    if (lockTimer->isActive())
        lockTimer->stop();

    statusLabel->hide();
    attemptsLabel->hide();
    verifyBtn->setEnabled(true);
    codeEdit->clear();
}

// ── UI ───────────────────────────────────────────────────────
void VerifyWidget::setupUI()
{
    QVBoxLayout *outerV = new QVBoxLayout(this);
    outerV->setContentsMargins(0, 0, 0, 0);
    outerV->addStretch();

    QHBoxLayout *outerH = new QHBoxLayout();
    outerH->addStretch();

    QWidget *card = new QWidget(this);
    card->setFixedWidth(340);
    card->setStyleSheet(QString(
                            "background:%1;"
                            "border:1px solid %2;"
                            "border-radius:10px;"
                            ).arg(GH_CARD, GH_BORDER));

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(28, 28, 28, 28);
    cardLayout->setSpacing(10);

    QLabel *title = new QLabel("Подтверждение входа", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("color:%1;").arg(GH_TEXT));
    cardLayout->addWidget(title);

    infoLabel = new QLabel("Код отправлен на вашу почту", card);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet(QString("color:%1;").arg(GH_MUTED));
    cardLayout->addWidget(infoLabel);

    codeEdit = new QLineEdit(card);
    codeEdit->setPlaceholderText("Введите код");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setStyleSheet(inputStyle() +
                            "QLineEdit { font-size:15pt; letter-spacing:4px; }");
    cardLayout->addWidget(codeEdit);

    statusLabel = new QLabel(card);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->hide();
    cardLayout->addWidget(statusLabel);

    attemptsLabel = new QLabel(card);
    attemptsLabel->setAlignment(Qt::AlignCenter);
    attemptsLabel->hide();
    cardLayout->addWidget(attemptsLabel);

    verifyBtn = new QPushButton("Подтвердить", card);
    verifyBtn->setMinimumHeight(38);
    verifyBtn->setStyleSheet(primaryBtnStyle());
    connect(verifyBtn, &QPushButton::clicked,
            this, &VerifyWidget::onVerifyClicked);
    cardLayout->addWidget(verifyBtn);

    QFrame *line = new QFrame(card);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet(QString("background:%1; max-height:1px;").arg(GH_BORDER));
    cardLayout->addWidget(line);

    backBtn = new QPushButton("Назад", card);
    backBtn->setStyleSheet(linkBtnStyle());
    connect(backBtn, &QPushButton::clicked,
            this, &VerifyWidget::onBackClicked);
    cardLayout->addWidget(backBtn, 0, Qt::AlignCenter);

    outerH->addWidget(card);
    outerH->addStretch();

    outerV->addLayout(outerH);
    outerV->addStretch();

    setLayout(outerV);
}

// ── Lock ─────────────────────────────────────────────────────
void VerifyWidget::applyLock(int minutes, const QString &message)
{
    isLocked = true;
    verifyBtn->setEnabled(false);

    statusLabel->setText(message);
    statusLabel->setStyleSheet(QString("color:%1;").arg(GH_RED));
    statusLabel->show();

    attemptsLabel->hide();

    lockTimer->start(minutes == 0
                         ? 30 * 1000
                         : minutes * 60 * 1000);
}

void VerifyWidget::onLockTimerFired()
{
    isLocked = false;
    verifyBtn->setEnabled(true);
    statusLabel->hide();
    attemptsLabel->hide();
}

// ── Verify click ─────────────────────────────────────────────
void VerifyWidget::onVerifyClicked()
{
    if (isLocked) {
        int sec = lockTimer->remainingTime() / 1000;
        int min = sec / 60;
        int rem = sec % 60;

        statusLabel->setText(min > 0
                                 ? QString("Заблокировано: %1 мин %2 сек").arg(min).arg(rem)
                                 : QString("Заблокировано: %1 сек").arg(sec));

        statusLabel->show();
        return;
    }

    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        statusLabel->setText("Введите код.");
        statusLabel->setStyleSheet(QString("color:%1;").arg(GH_RED));
        statusLabel->show();
        return;
    }

    m_waitingForVerify = true;
    verifyBtn->setEnabled(false);

    statusLabel->setText("Проверка...");
    statusLabel->setStyleSheet(QString("color:%1;").arg(GH_MUTED));
    statusLabel->show();

    ClientSingleton::instance().sendRequestAsync(
        QString("verify_auth||%1||%2").arg(login, code)
        );
}

// ── Server response ───────────────────────────────────────────
void VerifyWidget::onVerifyResponseReceived(const QString &response)
{
    if (!m_waitingForVerify)
        return;

    m_waitingForVerify = false;
    QString r = response.trimmed();

    if (r.isEmpty()) {
        verifyBtn->setEnabled(true);
        statusLabel->setText("Ошибка соединения.");
        statusLabel->setStyleSheet(QString("color:%1;").arg(GH_RED));
        statusLabel->show();
        return;
    }

    if (r.startsWith("auth+")) {
        statusLabel->setText("Успешный вход!");
        statusLabel->setStyleSheet(QString("color:%1;").arg(GH_GREEN_H));
        statusLabel->show();

        verifyBtn->setEnabled(false);

        QTimer::singleShot(500, this, [this]() {
            emit verificationSuccess(login);
        });
        return;
    }

    if (r.startsWith("auth-")) {
        failedAttempts++;

        if (failedAttempts < 4) {
            statusLabel->setText(
                QString("Неверный код (%1/4)").arg(failedAttempts)
                );
            statusLabel->setStyleSheet(QString("color:%1;").arg(GH_RED));
            statusLabel->show();
            verifyBtn->setEnabled(true);
            return;
        }

        if (failedAttempts == 4)
            applyLock(0, "Блокировка 30 сек");
        else if (failedAttempts == 5)
            applyLock(5, "Блокировка 5 мин");
        else if (failedAttempts == 6)
            applyLock(10, "Блокировка 10 мин");
        else
            applyLock(9999, "Аккаунт заблокирован");

        return;
    }

    verifyBtn->setEnabled(true);
}

// ── Back ─────────────────────────────────────────────────────
void VerifyWidget::onBackClicked()
{
    if (lockTimer->isActive())
        lockTimer->stop();

    isLocked = false;
    failedAttempts = 0;
    lockLevel = 0;
    m_waitingForVerify = false;

    statusLabel->hide();
    attemptsLabel->hide();
    verifyBtn->setEnabled(true);
    codeEdit->clear();

    emit backToAuth();
}