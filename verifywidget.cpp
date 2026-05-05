#include "verifywidget.h"
#include <QVBoxLayout>

VerifyWidget::VerifyWidget(QWidget *parent)
    : QWidget(parent)
{
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

    codeEdit = new QLineEdit(this);
    codeEdit->setPlaceholderText("Code");

    verifyBtn = new QPushButton("Verify", this);
    verifyBtn->setStyleSheet(btnStyle());

    backBtn = new QPushButton("Back", this);

    statusLabel = new QLabel(this);

    layout->addWidget(infoLabel);
    layout->addWidget(codeEdit);
    layout->addWidget(verifyBtn);
    layout->addWidget(backBtn);
    layout->addWidget(statusLabel);

    connect(verifyBtn, &QPushButton::clicked,
            this, &VerifyWidget::onVerifyClicked);

    connect(backBtn, &QPushButton::clicked,
            this, &VerifyWidget::onBackClicked);
}

void VerifyWidget::setLogin(const QString &l)
{
    login = l;
}

void VerifyWidget::onVerifyClicked()
{
    if (codeEdit->text().isEmpty()) {
        statusLabel->setText("Empty code");
        return;
    }

    statusLabel->setText("Success (mock)");

    emit verificationSuccess(login);
}

void VerifyWidget::onBackClicked()
{
    emit backToAuth();
}