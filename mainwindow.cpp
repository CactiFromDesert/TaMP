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

    topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(12, 0, 12, 0);

    taskBtn = new QPushButton("Задание", topBar);
    schemaBtn = new QPushButton("Блок-схема", topBar);

    appTitleLabel = new QLabel("ТиМП — Подгруппа 2", topBar);

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