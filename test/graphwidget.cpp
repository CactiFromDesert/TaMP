#include "graphwidget.h"

#include <QPainter>
#include <QPen>
#include <cmath>

// ─────────────────────────────────────────────
// CONSTRUCTOR
// ─────────────────────────────────────────────
GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
    , currentA(1.0)
    , currentB(1.0)
    , currentC(1.0)
{
    setupUI();

    // безопасный старт (если UI ещё не готов — не падаем)
    if (spinA && spinB && spinC)
        updateGraph();
}

GraphWidget::~GraphWidget() = default;

// ─────────────────────────────────────────────
// UI SETUP
// ─────────────────────────────────────────────
void GraphWidget::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(300);

    setupLeftPanel(); // теперь ОБЯЗАТЕЛЬНО должен существовать (stub допустим)

    mainLayout->addWidget(leftPanel);
    setLayout(mainLayout);
}

// ─────────────────────────────────────────────
// SLIDERS → SPINS (SAFE)
// ─────────────────────────────────────────────
void GraphWidget::onSliderAChanged(int v)
{
    if (spinA) spinA->setValue(v / 10.0);
    updateGraph();
}

void GraphWidget::onSliderBChanged(int v)
{
    if (spinB) spinB->setValue(v / 10.0);
    updateGraph();
}

void GraphWidget::onSliderCChanged(int v)
{
    if (spinC) spinC->setValue(v / 10.0);
    updateGraph();
}

void GraphWidget::onSpinAChanged(double v)
{
    if (sliderA) sliderA->setValue(int(v * 10));
    updateGraph();
}

void GraphWidget::onSpinBChanged(double v)
{
    if (sliderB) sliderB->setValue(int(v * 10));
    updateGraph();
}

void GraphWidget::onSpinCChanged(double v)
{
    if (sliderC) sliderC->setValue(int(v * 10));
    updateGraph();
}

// ─────────────────────────────────────────────
// MATH ONLY (NO BACKEND)
// ─────────────────────────────────────────────
double GraphWidget::calculate(double x, double a, double b, double c) const
{
    if (x < -2.0)
        return std::fabs(x * a) - 2.0;
    else if (x < 2.0)
        return b * x * x + x + 1.0;
    else
        return std::fabs(x - 2.0) + c;
}

// ─────────────────────────────────────────────
// GRAPH UPDATE
// ─────────────────────────────────────────────
void GraphWidget::updateGraph()
{
    if (!spinA || !spinB || !spinC || !table)
        return; // UI stub safety

    double a = spinA->value();
    double b = spinB->value();
    double c = spinC->value();

    currentA = a;
    currentB = b;
    currentC = c;

    pointsBranch1.clear();
    pointsBranch2.clear();
    pointsBranch3.clear();

    for (double x = -10; x <= 10; x += 0.1)
    {
        double y = calculate(x, a, b, c);
        QPointF pt(x, y);

        if (x < -2) pointsBranch1.append(pt);
        else if (x < 2) pointsBranch2.append(pt);
        else pointsBranch3.append(pt);
    }

    fillTable(a, b, c);
    update();
}

// ─────────────────────────────────────────────
// TABLE
// ─────────────────────────────────────────────
void GraphWidget::fillTable(double a, double b, double c)
{
    if (!table)
        return;

    table->setRowCount(21);

    int row = 0;
    for (int x = -10; x <= 10; x++)
    {
        double y = calculate(x, a, b, c);

        table->setItem(row, 0, new QTableWidgetItem(QString::number(x)));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(y, 'f', 2)));

        row++;
    }
}

// ─────────────────────────────────────────────
// LOGOUT
// ─────────────────────────────────────────────
void GraphWidget::onLogoutClicked()
{
    emit logout();
}

// ─────────────────────────────────────────────
// USER
// ─────────────────────────────────────────────
void GraphWidget::setUserLogin(const QString &login)
{
    userLogin = login;

    if (userLabel)
        userLabel->setText("Пользователь: " + login);
}

// ─────────────────────────────────────────────
// PAINT
// ─────────────────────────────────────────────
void GraphWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // оставлено как hook для будущего кастомного рендера
    Q_UNUSED(event);
}

void GraphWidget::setupLeftPanel()
{
    auto *layout = new QVBoxLayout(leftPanel);

    formulaLabel = new QLabel("y = f(x)", leftPanel);

    spinA = new QDoubleSpinBox(leftPanel);
    spinB = new QDoubleSpinBox(leftPanel);
    spinC = new QDoubleSpinBox(leftPanel);

    sliderA = new QSlider(Qt::Horizontal, leftPanel);
    sliderB = new QSlider(Qt::Horizontal, leftPanel);
    sliderC = new QSlider(Qt::Horizontal, leftPanel);

    table = new QTableWidget(leftPanel);
    table->setColumnCount(2);

    logoutBtn = new QPushButton("Logout", leftPanel);
    userLabel = new QLabel(leftPanel);

    layout->addWidget(formulaLabel);
    layout->addWidget(spinA);
    layout->addWidget(sliderA);
    layout->addWidget(spinB);
    layout->addWidget(sliderB);
    layout->addWidget(spinC);
    layout->addWidget(sliderC);
    layout->addWidget(table);
    layout->addWidget(userLabel);
    layout->addWidget(logoutBtn);

    connect(sliderA, &QSlider::valueChanged,
            this, &GraphWidget::onSliderAChanged);

    connect(sliderB, &QSlider::valueChanged,
            this, &GraphWidget::onSliderBChanged);

    connect(sliderC, &QSlider::valueChanged,
            this, &GraphWidget::onSliderCChanged);

    connect(spinA, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GraphWidget::onSpinAChanged);

    connect(spinB, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GraphWidget::onSpinBChanged);

    connect(spinC, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GraphWidget::onSpinCChanged);

    connect(logoutBtn, &QPushButton::clicked,
            this, &GraphWidget::onLogoutClicked);
}