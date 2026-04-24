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

// ── Light Blue Theme palette ─────────────────────────────────────────────
#define UI_BG          "#0b1e2d"
#define UI_TOPBAR      "#102a43"
#define UI_BORDER      "#1f4e79"
#define UI_TEXT        "#e6f2ff"
#define UI_BTN         "#3da9fc"
#define UI_BTN_HOVER   "#74c0fc"
#define UI_BTN_PRESS   "#2f8fe6"
#define FONT_FAMILY    "Segoe UI"

static QIcon tintIcon(const QIcon &src, const QColor &color, int sz = 18)
{
    QPixmap pm = src.pixmap(sz, sz);
    QPainter painter(&pm);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pm.rect(), color);
    painter.end();
    return QIcon(pm);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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

void MainWindow::setupUI()
{
    setWindowTitle("ТиМП — Подгруппа 2");
    setMinimumSize(1000, 700);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainVLayout = new QVBoxLayout(centralWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);

    // ── Top bar ─────────────────────────────────────────────
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
    topBarLayout->setSpacing(6);

    const QColor iconColor(31, 42, 68);

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

    // ── StackedWidget ───────────────────────────────────────
    stackedWidget = new QStackedWidget(centralWidget);

    authWidget   = new AuthWidget(stackedWidget);
    regWidget    = new RegWidget(stackedWidget);
    verifyWidget = new VerifyWidget(stackedWidget);
    graphWidget  = new GraphWidget(stackedWidget);
    resetWidget  = new ResetWidget(stackedWidget);

    stackedWidget->addWidget(authWidget);
    stackedWidget->addWidget(regWidget);
    stackedWidget->addWidget(verifyWidget);
    stackedWidget->addWidget(graphWidget);
    stackedWidget->addWidget(resetWidget);

    mainVLayout->addWidget(topBar);
    mainVLayout->addWidget(stackedWidget, 1);
}

void MainWindow::connectSignals()
{
    connect(taskBtn, &QPushButton::clicked,
            this, &MainWindow::onTaskBtnClicked);

    connect(schemaBtn, &QPushButton::clicked,
            this, &MainWindow::onSchemaBtnClicked);

    connect(authWidget, &AuthWidget::showVerifyAuth,
            this, &MainWindow::onShowVerifyAuth);

    connect(authWidget, &AuthWidget::showRegister,
            this, &MainWindow::onShowRegister);

    connect(authWidget, &AuthWidget::showReset,
            this, &MainWindow::onShowReset);

    connect(authWidget, &AuthWidget::loginSuccess,
            this, [this](const QString &login) {
                graphWidget->setUserLogin(login);
                stackedWidget->setCurrentIndex(IDX_GRAPH);
                QTimer::singleShot(0, graphWidget,
                                   &GraphWidget::updateGraph);
            });

    connect(regWidget, &RegWidget::registrationSuccess,
            this, &MainWindow::onRegistrationSuccess);

    connect(regWidget, &RegWidget::showAuth,
            this, &MainWindow::onShowAuth);

    connect(verifyWidget, &VerifyWidget::verificationSuccess,
            this, &MainWindow::onVerificationSuccess);

    connect(verifyWidget, &VerifyWidget::backToAuth,
            this, &MainWindow::onBackToAuth);

    connect(resetWidget, &ResetWidget::backToAuth,
            this, &MainWindow::onShowAuth);

    connect(resetWidget, &ResetWidget::resetSuccess,
            this, &MainWindow::onResetSuccess);

    connect(graphWidget, &GraphWidget::logout,
            this, &MainWindow::onLogout);
}

// ── Navigation ─────────────────────────────────────────────
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

    QTimer::singleShot(0, graphWidget,
                       &GraphWidget::updateGraph);
}

void MainWindow::onBackToAuth()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

void MainWindow::onRegistrationSuccess(const QString &login)
{
    graphWidget->setUserLogin(login);
    stackedWidget->setCurrentIndex(IDX_GRAPH);

    QTimer::singleShot(0, graphWidget,
                       &GraphWidget::updateGraph);
}

void MainWindow::onLogout()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

void MainWindow::onResetSuccess()
{
    stackedWidget->setCurrentIndex(IDX_AUTH);
}

// ── dialogs ─────────────────────────────────────────────
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