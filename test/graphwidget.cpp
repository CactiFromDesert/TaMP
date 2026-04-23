#include "graphwidget.h"

#include <QPainter>
#include <QPen>
#include <cmath>
#include <iostream>

// ?????????????????????????????????????????????
// CONSTRUCTOR
// ?????????????????????????????????????????????
GraphWidget::GraphWidget(QWidget* parent)
    : QWidget(parent)
    , currentA(1.0)
    , currentB(1.0)
    , currentC(1.0)
{
    setupUI();

    if (spinA && spinB && spinC)
        updateGraph();
}

GraphWidget::~GraphWidget() = default;

// ?????????????????????????????????????????????
// UI SETUP
// ?????????????????????????????????????????????
void GraphWidget::setupUI()
{
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(320);

    setupLeftPanel();

    mainLayout->addWidget(leftPanel);
    setLayout(mainLayout);

    setStyleSheet(
        "font-weight: bold;"
        "QTableWidget, QTableWidget::item { font-weight: normal; }"
    );
}

// ?????????????????????????????????????????????
// SLIDERS ? SPINS (SAFE)
// ?????????????????????????????????????????????
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

// ?????????????????????????????????????????????
// MATH ONLY (NO BACKEND)
// ?????????????????????????????????????????????
double GraphWidget::calculate(double x, double a, double b, double c)
{
    if (x < 0)
    {
        return sqrt(-x) + a;
    }
    else if (x >= 0 && x < PI)
    {
        double argument = x / PI;

        if (argument < -1.0 || argument > 1.0)
        {
            std::cerr << "Ошибка: аргумент arcsin вне диапазона [-1, 1] для x = " << x << "\n";
            return NAN;
        }

        return asin(argument) + b;
    }
    else
    {
        double argument = x / PI;

        if (argument < -1.0 || argument > 1.0)
        {
            std::cerr << "Ошибка: аргумент arccos вне диапазона [-1, 1] для x = " << x << "\n";
            return NAN;
        }

        return acos(argument) - c;
    }
}

// ?????????????????????????????????????????????
// GRAPH UPDATE
// ?????????????????????????????????????????????
void GraphWidget::updateGraph()
{
    if (!spinA || !spinB || !spinC || !table)
        return;

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

// ?????????????????????????????????????????????
// TABLE
// ?????????????????????????????????????????????
void GraphWidget::fillTable(double a, double b, double c)
{
    if (!table)
        return;

    table->setRowCount(21);

    int row = 0;

    for (int x = -10; x <= 10; x++)
    {
        double y = calculate(x, a, b, c);

        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(x)));

        table->setItem(row, 1,
            new QTableWidgetItem(QString::number(y, 'f', 2)));

        row++;
    }
}

// ?????????????????????????????????????????????
// LOGOUT
// ?????????????????????????????????????????????
void GraphWidget::onLogoutClicked()
{
    emit logout();
}

// ?????????????????????????????????????????????
// USER
// ?????????????????????????????????????????????
void GraphWidget::setUserLogin(const QString& login)
{
    userLogin = login;

    if (userLabel)
        userLabel->setText("Пользователь: " + login);
}

// ?????????????????????????????????????????????
// PAINT
// ?????????????????????????????????????????????
void GraphWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    Q_UNUSED(event);
}

// ?????????????????????????????????????????????
// LEFT PANEL UI (СТИЛЬ ПЕРЕНЕСЁН)
// ?????????????????????????????????????????????
void GraphWidget::setupLeftPanel()
{
    auto* layout = new QVBoxLayout(leftPanel);

    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    formulaLabel = new QLabel("y = f(x)", leftPanel);
    formulaLabel->setStyleSheet("font-weight: bold; font-size: 13pt;");
    formulaLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(formulaLabel);

    layout->addSpacing(10);

    // A
    QHBoxLayout* aLayout = new QHBoxLayout();

    QLabel* labelA = new QLabel("a", leftPanel);
    labelA->setStyleSheet("font-weight: bold; color: #ff6b6b; font-size: 11pt;");
    labelA->setFixedWidth(30);

    aLayout->addWidget(labelA);

    spinA = new QDoubleSpinBox(leftPanel);
    spinA->setStyleSheet("font-weight: bold; font-size: 11pt;");
    spinA->setRange(0, 10);
    spinA->setValue(1.0);
    spinA->setDecimals(1);
    spinA->setSingleStep(0.1);
    spinA->setFixedWidth(100);

    aLayout->addWidget(spinA);
    aLayout->addStretch();

    layout->addLayout(aLayout);

    sliderA = new QSlider(Qt::Horizontal, leftPanel);
    sliderA->setRange(0, 100);
    sliderA->setValue(10);

    sliderA->setStyleSheet(
        "QSlider::groove:horizontal { height: 3px; background: #ff6b6b; border-radius: 1.5px; }"
        "QSlider::handle:horizontal { background: #ff6b6b; width: 10px; height: 10px; margin: -4px 0; border-radius: 5px; }"
        "QSlider::handle:horizontal:hover { background: #ff8888; }"
        "QSlider::sub-page:horizontal { background: #ff6b6b; border-radius: 1.5px; }"
    );

    layout->addWidget(sliderA);

    layout->addSpacing(15);

    // B
    QHBoxLayout* bLayout = new QHBoxLayout();

    QLabel* labelB = new QLabel("b", leftPanel);
    labelB->setStyleSheet("font-weight: bold; color: #4cd4b0; font-size: 11pt;");
    labelB->setFixedWidth(30);

    bLayout->addWidget(labelB);

    spinB = new QDoubleSpinBox(leftPanel);
    spinB->setStyleSheet("font-weight: bold; font-size: 11pt;");
    spinB->setRange(0, 10);
    spinB->setValue(1.0);
    spinB->setDecimals(1);
    spinB->setSingleStep(0.1);
    spinB->setFixedWidth(100);

    bLayout->addWidget(spinB);
    bLayout->addStretch();

    layout->addLayout(bLayout);

    sliderB = new QSlider(Qt::Horizontal, leftPanel);
    sliderB->setRange(0, 100);
    sliderB->setValue(10);

    sliderB->setStyleSheet(
        "QSlider::groove:horizontal { height: 3px; background: #4cd4b0; border-radius: 1.5px; }"
        "QSlider::handle:horizontal { background: #4cd4b0; width: 10px; height: 10px; margin: -4px 0; border-radius: 5px; }"
        "QSlider::handle:horizontal:hover { background: #6ee0c4; }"
        "QSlider::sub-page:horizontal { background: #4cd4b0; border-radius: 1.5px; }"
    );

    layout->addWidget(sliderB);

    layout->addSpacing(15);

    // C
    QHBoxLayout* cLayout = new QHBoxLayout();

    QLabel* labelC = new QLabel("c", leftPanel);
    labelC->setStyleSheet("font-weight: bold; color: #74c0fc; font-size: 11pt;");
    labelC->setFixedWidth(30);

    cLayout->addWidget(labelC);

    spinC = new QDoubleSpinBox(leftPanel);
    spinC->setStyleSheet("font-weight: bold; font-size: 11pt;");
    spinC->setRange(0, 10);
    spinC->setValue(1.0);
    spinC->setDecimals(1);
    spinC->setSingleStep(0.1);
    spinC->setFixedWidth(100);

    cLayout->addWidget(spinC);
    cLayout->addStretch();

    layout->addLayout(cLayout);

    sliderC = new QSlider(Qt::Horizontal, leftPanel);
    sliderC->setRange(0, 100);
    sliderC->setValue(10);

    sliderC->setStyleSheet(
        "QSlider::groove:horizontal { height: 3px; background: #74c0fc; border-radius: 1.5px; }"
        "QSlider::handle:horizontal { background: #74c0fc; width: 10px; height: 10px; margin: -4px 0; border-radius: 5px; }"
        "QSlider::handle:horizontal:hover { background: #90d4ff; }"
        "QSlider::sub-page:horizontal { background: #74c0fc; border-radius: 1.5px; }"
    );

    layout->addWidget(sliderC);

    layout->addSpacing(20);

    table = new QTableWidget(leftPanel);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels(QStringList() << "x" << "y");

    table->setStyleSheet(
        "QTableWidget { font-weight: normal; }"
        "QTableWidget::item { font-weight: normal; }"
    );

    table->setMinimumHeight(200);

    layout->addWidget(table);

    layout->addStretch();

    userLabel = new QLabel(leftPanel);
    userLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(userLabel);

    logoutBtn = new QPushButton("Logout", leftPanel);
    logoutBtn->setStyleSheet(
        "font-weight: bold;"
        "font-size: 10pt;"
        "padding: 6px;"
    );

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