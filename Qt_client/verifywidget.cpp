#include "verifywidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include "authclient.h"

#define GH_BG          "#0d1117"
#define GH_CARD        "#161b22"
#define GH_BORDER      "#30363d"
#define GH_TEXT        "#e6edf3"
#define GH_MUTED       "#8b949e"
#define GH_GREEN       "#238636"
#define GH_GREEN_H     "#2ea043"
#define GH_BLUE        "#388bfd"
#define GH_RED         "#f85149"
#define GH_INPUT_BG    "#0d1117"

#define FONT_FAMILY     "Segoe UI"
#define FONT_SIZE_INPUT 11

static QString inputStyle()
{
    return QString(
        "QLineEdit {"
        " background-color:%1; color:%2; border:1px solid %3;"
        " border-radius:6px; padding:6px 10px;"
        " font-family:'%4'; font-size:%5pt; font-weight: bold;"
        "}"
        "QLineEdit:focus { border-color:%6; }"
    ).arg(GH_INPUT_BG, GH_TEXT, GH_BORDER, FONT_FAMILY)
     .arg(FONT_SIZE_INPUT).arg(GH_BLUE);
}

static QString primaryBtnStyle()
{
    return QString(
        "QPushButton {"
        " background:%1; color:#fff; border-radius:6px;"
        " padding:6px 16px; font-weight: bold; font-size: 12pt;"
        "}"
        "QPushButton:hover { background:%2; }"
        "QPushButton:disabled {"
        " background:rgba(35,134,54,0.35); color:rgba(255,255,255,0.4);"
        "}"
    ).arg(GH_GREEN, GH_GREEN_H);
}

static QString secondaryBtnStyle()
{
    return QString(
        "QPushButton {"
        " background:transparent; color:%1; border:1px solid %2;"
        " border-radius:6px; padding:6px 16px;"
        " font-weight: bold; font-size: 12pt;"
        "}"
        "QPushButton:hover { background:%3; }"
    ).arg(GH_TEXT, GH_BORDER, GH_BORDER);
}

VerifyWidget::VerifyWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(QString(
        "background:%1; color:%2; font-family:'%3'; font-weight: bold; font-size: 12pt;"
    ).arg(GH_BG, GH_TEXT, FONT_FAMILY));

    setupUI();
    connect(&AuthClient::instance(), &AuthClient::responseReceived,
            this, &VerifyWidget::onAuthResponse);
}

VerifyWidget::~VerifyWidget() = default;

void VerifyWidget::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->addStretch();

    auto *card = new QWidget(this);
    card->setMinimumWidth(340);
    card->setMaximumWidth(500);
    card->setStyleSheet(QString(
        "background:%1; border:1px solid %2; border-radius:12px; font-weight: bold;"
    ).arg(GH_CARD, GH_BORDER));

    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    auto *title = new QLabel("Enter verification code", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("border:none; font-size:14pt; font-weight: bold;");
    layout->addWidget(title);

    infoLabel = new QLabel("Code sent to your email", card);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet(QString("border:none; color:%1; font-weight: bold;").arg(GH_MUTED));
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    codeEdit = new QLineEdit(card);
    codeEdit->setPlaceholderText("Code");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setStyleSheet(inputStyle() + "QLineEdit { font-size:15pt; letter-spacing:4px; }");
    layout->addWidget(codeEdit);

    statusLabel = new QLabel(card);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->hide();
    layout->addWidget(statusLabel);

    verifyBtn = new QPushButton("Verify", card);
    verifyBtn->setMinimumHeight(38);
    verifyBtn->setStyleSheet(primaryBtnStyle());
    layout->addWidget(verifyBtn);

    backBtn = new QPushButton("Back", card);
    backBtn->setStyleSheet(secondaryBtnStyle());
    layout->addWidget(backBtn);

    auto *cardRow = new QHBoxLayout();
    cardRow->addStretch();
    cardRow->addWidget(card);
    cardRow->addStretch();
    outer->addLayout(cardRow);
    outer->addStretch();

    connect(verifyBtn, &QPushButton::clicked, this, &VerifyWidget::onVerifyClicked);
    connect(backBtn, &QPushButton::clicked, this, &VerifyWidget::onBackClicked);
}

void VerifyWidget::setLogin(const QString &l)
{
    login = l;
}

void VerifyWidget::onVerifyClicked()
{
    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        statusLabel->setText("Enter code");
        statusLabel->setStyleSheet("color:#f85149; font-weight: bold;");
        statusLabel->show();
        return;
    }

    QJsonObject obj;
    obj["code"] = code;
    AuthClient::instance().sendRequest("verify_login", obj);
}

void VerifyWidget::onAuthResponse(const QString &response)
{
    if (!isVisible()) return;

    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString status = obj["status"].toString();

    if (status == "ok") {
        emit verificationSuccess(login);
    } else {
        statusLabel->setText(obj["message"].toString());
        statusLabel->setStyleSheet("color:#f85149; font-weight: bold;");
        statusLabel->show();
    }
}

void VerifyWidget::onBackClicked()
{
    emit backToAuth();
}
