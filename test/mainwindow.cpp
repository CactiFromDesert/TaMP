#include "mainwindow.h"
#include "taskdialog.h"
#include "schemadialog.h"

#include <QStyle>
#include <QApplication>
#include <QFrame>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QTimer>

// ── Theme ─────────────────────────────────────────────
#define UI_BG          "#0b1e2d"
#define UI_TOPBAR      "#102a43"
#define UI_BORDER      "#1f4e79"
#define UI_TEXT        "#e6f2ff"
#define UI_BTN         "#3da9fc"
#define UI_BTN_HOVER   "#74c0fc"
#define UI_BTN_PRESS   "#2f8fe6"
#define FONT_FAMILY    "Segoe UI"

// ─────────────────────────────────────────────

static QIcon tintIcon(const QIcon &src, const QColor &color, int sz = 18)
{
    QPixmap pm = src.pixmap(sz, sz);
    QPainter painter(&pm);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pm.rect(), color);
    painter.end();
    return QIcon(pm);
}

// ─────────────────────────────────────────────
// CONSTRUCTOR (ТОЛЬКО ДОБАВЛЕН backend)
// ─────────────────────────────────────────────

MainWindow::MainWindow(Database& db, QWidget *parent)
    : QMainWindow(parent),
    m_db(db)
{
    m_auth = new Auth(m_db); // 🔥 ЕДИНСТВЕННОЕ ВАЖНОЕ ДОБАВЛЕНИЕ

    setStyleSheet(QString(
                      "QMainWindow { background-color: %1; font-weight: bold; }"
                      "QWidget { background-color: %1; color: %2; font-family: '%3'; font-weight: bold; }"
                      ).arg(UI_BG).arg(UI_TEXT).arg(FONT_FAMILY));

    setupUI();
    connectSignals();

    QTimer::singleShot(0, this, [this]() {
        stackedWidget->setCurrentIndex(IDX_AUTH);
    });
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────
// UI (НЕ ИЗМЕНЕНО)
// ─────────────────────────────────────────────

void MainWindow::setupUI()
{
    setWindowTitle("ТиМП — Подгруппа 2");
    setMinimumSize(1000, 700);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainVLayout = new QVBoxLayout(centralWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);

    // ── TOP BAR ─────────────────────────────
    QWidget *topBar = new QWidget(centralWidget);
    topBar->setFixedHeight(44);

    topBar->setStyleSheet(QString(
                              "QWidget { background-color: %1; border-bottom: 1px solid %2; font-weight: bold; }"
                              "QPushButton {"
                              "  background-color: %3;"
                              "  color: %4;"
                              "  border: 1px solid %2;"
                              "  border-radius: 6px;"
                              "  padding: 4px 12px;"
                              "  font-size: 14px;"
                              "  font-family: '%5';"
                              "  font-weight: bold;"
                              "}"
                              "QPushButton:hover { background-color: %6; }"
                              "QPushButton:pressed { background-color: %7; }"
                              )
                              .arg(UI_TOPBAR)
                              .arg(UI_BORDER)
                              .arg(UI_BTN)
                              .arg(UI_TEXT)
                              .arg(FONT_FAMILY)
                              .arg(UI_BTN_HOVER)
                              .arg(UI_BTN_PRESS)
                          );


    topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(12, 0, 12, 0);

    taskBtn = new QPushButton("Задание", topBar);
    schemaBtn = new QPushButton("Блок-схема", topBar);

    taskBtn = new QPushButton(topBar);
    taskBtn->setIcon(tintIcon(
        style()->standardIcon(QStyle::SP_FileDialogInfoView),
        iconColor
        ));
    taskBtn->setText("  Задание");

    schemaBtn = new QPushButton(topBar);
    schemaBtn->setIcon(tintIcon(
        style()->standardIcon(QStyle::SP_FileDialogContentsView),
        iconColor
        ));
    schemaBtn->setText("  Блок-схема");

    appTitleLabel = new QLabel("ТиМП — Подгруппа 2", topBar);
    appTitleLabel->setAlignment(Qt::AlignCenter);

    appTitleLabel->setStyleSheet(QString(
                                     "QLabel {"
                                     " color: %1;"
                                     " font-size: 16px;"
                                     " font-weight: bold;"
                                     " background: transparent;"
                                     " border: none;"
                                     "}"
                                     ).arg(UI_TEXT));

    topBarLayout->addWidget(taskBtn);
    topBarLayout->addWidget(schemaBtn);
    topBarLayout->addStretch(1);
    topBarLayout->addWidget(appTitleLabel);
    topBarLayout->addStretch(1);

    // ── STACK ───────────────────────────────
    stackedWidget = new QStackedWidget(centralWidget);

    authWidget   = new AuthWidget(stackedWidget);
    regWidget    = new RegWidget(stackedWidget);
    verifyWidget = new VerifyWidget(stackedWidget);
    resetWidget  = new ResetWidget(stackedWidget);
    graphWidget  = new GraphWidget(stackedWidget);

    stackedWidget->addWidget(authWidget);
    stackedWidget->addWidget(regWidget);
    stackedWidget->addWidget(verifyWidget);
    stackedWidget->addWidget(graphWidget);
    stackedWidget->addWidget(resetWidget);

    mainVLayout->addWidget(topBar);
    mainVLayout->addWidget(stackedWidget, 1);
}

// ─────────────────────────────────────────────
// SIGNALS (НЕ ТРОГАЕМ ЛОГИКУ)
// ─────────────────────────────────────────────

void MainWindow::connectSignals()
{   connect(taskBtn, &QPushButton::clicked,
            this, &MainWindow::onTaskBtnClicked);

    connect(schemaBtn, &QPushButton::clicked,
            this, &MainWindow::onSchemaBtnClicked);

    connect(authWidget, &AuthWidget::showVerifyAuth,
            this, &MainWindow::onShowVerifyAuth);

    connect(authWidget, &AuthWidget::loginSuccess,
            this, [this](const QString &login) {
                graphWidget->setUserLogin(login);
                stackedWidget->setCurrentIndex(IDX_GRAPH);
            });

    connect(verifyWidget, &VerifyWidget::verificationSuccess,
            this, &MainWindow::onVerificationSuccess);
}

// ─────────────────────────────────────────────
// NAVIGATION (100% твоя логика)
// ─────────────────────────────────────────────

void MainWindow::onShowRegister()  { stackedWidget->setCurrentIndex(IDX_REG); }
void MainWindow::onShowAuth()      { stackedWidget->setCurrentIndex(IDX_AUTH); }
void MainWindow::onShowReset()     { stackedWidget->setCurrentIndex(IDX_RESET); }

void MainWindow::onShowVerifyAuth(const QString &login)
{
    verifyWidget->setLogin(login);
    stackedWidget->setCurrentIndex(IDX_VERIFY);
}

void MainWindow::onVerificationSuccess(const QString &login)
{
    graphWidget->setUserLogin(login);
    stackedWidget->setCurrentIndex(IDX_GRAPH);
}

void MainWindow::onBackToAuth()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

void MainWindow::onRegistrationSuccess(const QString &login)
{
    graphWidget->setUserLogin(login);
    stackedWidget->setCurrentIndex(IDX_GRAPH);
}

void MainWindow::onLogout()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

void MainWindow::onResetSuccess()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

// ─────────────────────────────────────────────
// BUTTONS
// ─────────────────────────────────────────────

void MainWindow::onTaskBtnClicked()
{
    TaskDialog dlg(this);
    dlg.exec();
}

void MainWindow::onSchemaBtnClicked()
{
    SchemaDialog dlg(this);
    dlg.exec();
}