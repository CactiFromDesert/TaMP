#include "verifywidget.h"
#include <QVBoxLayout>

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
               " font-weight: bold;"
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
               " font-weight: bold;"
               " font-size: 12pt;"
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
               " font-weight: bold;"
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
=======
>>>>>>> my-real-code
VerifyWidget::VerifyWidget(QWidget *parent)
    : QWidget(parent)
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
                      "QWidget { background:%1; color:%2; font-family:'%3'; font-size:%4pt; font-weight: bold; }"
                      ).arg(GH_BG, GH_TEXT, FONT_FAMILY)
                      .arg(FONT_SIZE_INPUT));

    setupUI();
}

VerifyWidget::~VerifyWidget() = default;

static QString btnStyle()
{
    return "QPushButton {background:#238636;color:white;padding:8px;font-weight:bold;}";
}

void VerifyWidget::setupUI()
{
    auto *layout = new QVBoxLayout(this);

    infoLabel = new QLabel("Enter code", this);

<<<<<<< HEAD
    QWidget *card = new QWidget(this);
    card->setFixedWidth(340);
    card->setStyleSheet(QString(
                            "background:%1;"
                            "border:1px solid %2;"
                            "border-radius:10px;"
                            "font-weight: bold;"
                            ).arg(GH_CARD, GH_BORDER));

    verifyBtn = new QPushButton("Verify", this);
    verifyBtn->setStyleSheet(btnStyle());

<<<<<<< HEAD
    QLabel *title = new QLabel("Подтверждение входа", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("color:%1; font-weight: bold; font-size: 14pt;").arg(GH_TEXT));
    cardLayout->addWidget(title);

    infoLabel = new QLabel("Код отправлен на вашу почту", card);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_MUTED));
    cardLayout->addWidget(infoLabel);

    codeEdit = new QLineEdit(card);
    codeEdit->setPlaceholderText("Введите код");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setStyleSheet(inputStyle() +
                            "QLineEdit { font-size:15pt; letter-spacing:4px; font-weight: bold; }");
    cardLayout->addWidget(codeEdit);

    statusLabel = new QLabel(card);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("font-weight: bold;");
    statusLabel->hide();
    cardLayout->addWidget(statusLabel);

    attemptsLabel = new QLabel(card);
    attemptsLabel->setAlignment(Qt::AlignCenter);
    attemptsLabel->setStyleSheet("font-weight: bold;");
    attemptsLabel->hide();
    cardLayout->addWidget(attemptsLabel);

    verifyBtn = new QPushButton("Подтвердить", card);
    verifyBtn->setMinimumHeight(38);
    verifyBtn->setStyleSheet(primaryBtnStyle());
=======
    backBtn = new QPushButton("Back", this);

    statusLabel = new QLabel(this);

    layout->addWidget(infoLabel);
    layout->addWidget(codeEdit);
    layout->addWidget(verifyBtn);
    layout->addWidget(backBtn);
    layout->addWidget(statusLabel);

>>>>>>> my-real-code
    connect(verifyBtn, &QPushButton::clicked,
            this, &VerifyWidget::onVerifyClicked);

    connect(backBtn, &QPushButton::clicked,
            this, &VerifyWidget::onBackClicked);
}

void VerifyWidget::setLogin(const QString &l)
{
<<<<<<< HEAD
    isLocked = true;
    verifyBtn->setEnabled(false);

    statusLabel->setText(message);
    statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_RED));
    statusLabel->show();

    attemptsLabel->hide();

    lockTimer->start(minutes == 0
                         ? 30 * 1000
                         : minutes * 60 * 1000);
=======
    login = l;
>>>>>>> my-real-code
}

void VerifyWidget::onVerifyClicked()
{
    if (codeEdit->text().isEmpty()) {
        statusLabel->setText("Empty code");
        return;
    }

<<<<<<< HEAD
    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        statusLabel->setText("Введите код.");
        statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_RED));
        statusLabel->show();
        return;
    }

    m_waitingForVerify = true;
    verifyBtn->setEnabled(false);

    statusLabel->setText("Проверка...");
    statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_MUTED));
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
        statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_RED));
        statusLabel->show();
        return;
    }

    if (r.startsWith("auth+")) {
        statusLabel->setText("Успешный вход!");
        statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_GREEN_H));
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
            statusLabel->setStyleSheet(QString("color:%1; font-weight: bold;").arg(GH_RED));
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
=======
    statusLabel->setText("Success (mock)");

    emit verificationSuccess(login);
}

>>>>>>> my-real-code
void VerifyWidget::onBackClicked()
{
    emit backToAuth();
}