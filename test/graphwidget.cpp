#include "graphwidget.h"
#include "clientsingleton.h"

#include <QPainter>
#include <QPen>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();

    // Подключаемся к серверу
    ClientSingleton::instance().connectToServer("calc_server", 11999);

    connect(&ClientSingleton::instance(), &ClientSingleton::responseReceived,
            this, &GraphWidget::onServerResponse);

    if (spinA && spinB && spinC)
        updateGraph();
}

GraphWidget::~GraphWidget() = default;

void GraphWidget::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(320);

    setupLeftPanel();

    mainLayout->addWidget(leftPanel);
    setLayout(mainLayout);

    setStyleSheet("font-weight: bold; QTableWidget, QTableWidget::item { font-weight: normal; }");
}

void GraphWidget::onSliderAChanged(int v)
{
    if (spinA) { spinA->blockSignals(true); spinA->setValue(v / 10.0); spinA->blockSignals(false); }
    updateGraph();
}

void GraphWidget::onSliderBChanged(int v)
{
    if (spinB) { spinB->blockSignals(true); spinB->setValue(v / 10.0); spinB->blockSignals(false); }
    updateGraph();
}

void GraphWidget::onSliderCChanged(int v)
{
    if (spinC) { spinC->blockSignals(true); spinC->setValue(v / 10.0); spinC->blockSignals(false); }
    updateGraph();
}

void GraphWidget::onSpinAChanged(double v)
{
    if (sliderA) { sliderA->blockSignals(true); sliderA->setValue(int(v * 10)); sliderA->blockSignals(false); }
    updateGraph();
}

void GraphWidget::onSpinBChanged(double v)
{
    if (sliderB) { sliderB->blockSignals(true); sliderB->setValue(int(v * 10)); sliderB->blockSignals(false); }
    updateGraph();
}

void GraphWidget::onSpinCChanged(double v)
{
    if (sliderC) { sliderC->blockSignals(true); sliderC->setValue(int(v * 10)); sliderC->blockSignals(false); }
    updateGraph();
}

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

    requestServerUpdate(a, b, c);
}

void GraphWidget::requestServerUpdate(double a, double b, double c)
{
    // Отправляем на сервер: a;b;c
    QString req = QString("%1;%2;%3").arg(a, 0, 'f', 2).arg(b, 0, 'f', 2).arg(c, 0, 'f', 2);
    ClientSingleton::instance().sendRequest(req);
}

void GraphWidget::onServerResponse(const QString &response)
{
    // Ответ от сервера — JSON-массив точек [[x1,y1],[x2,y2],...]
    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    if (!doc.isArray()) return;

    QJsonArray arr = doc.array();
    pointsBranch1.clear();
    pointsBranch2.clear();
    pointsBranch3.clear();

    for (int i = 0; i < arr.size(); i++) {
        QJsonArray pt = arr[i].toArray();
        if (pt.size() < 2) continue;
        double x = pt[0].toDouble();
        double y = pt[1].toDouble();

        if (x < -2.0)
            pointsBranch1.append(QPointF(x, y));
        else if (x < 2.0)
            pointsBranch2.append(QPointF(x, y));
        else
            pointsBranch3.append(QPointF(x, y));
    }

    fillTable(currentA, currentB, currentC);
    update(); // перерисовка
}

void GraphWidget::fillTable(double a, double b, double c)
{
    Q_UNUSED(a); Q_UNUSED(b); Q_UNUSED(c);
    if (!table) return;

    table->setRowCount(21);
    int row = 0;

    // Выводим из полученных точек (шаг ~1)
    for (int i = 0; i <= 200; i += 10) {
        double x = -10.0 + i * 0.1;
        double y = 0;
        // Ищем точку в объединённом массиве
        QPointF *all[] = { pointsBranch1.data(), pointsBranch2.data(), pointsBranch3.data() };
        int sizes[] = { pointsBranch1.size(), pointsBranch2.size(), pointsBranch3.size() };
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < sizes[j]; k++) {
                if (std::fabs(all[j][k].x() - x) < 0.05) {
                    y = all[j][k].y();
                    break;
                }
            }
        }

        table->setItem(row, 0, new QTableWidgetItem(QString::number(x, 'f', 1)));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(y, 'f', 2)));
        row++;
    }
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    Q_UNUSED(event);
}

void GraphWidget::onLogoutClicked()
{
    emit logout();
}

void GraphWidget::setUserLogin(const QString &login)
{
    userLogin = login;
    if (userLabel)
        userLabel->setText("Пользователь: " + login);
}

void GraphWidget::setupLeftPanel()
{
    auto *layout = new QVBoxLayout(leftPanel);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    formulaLabel = new QLabel("y = f(x)", leftPanel);
    formulaLabel->setStyleSheet("font-weight: bold; font-size: 13pt;");
    formulaLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(formulaLabel);
    layout->addSpacing(10);

    // A
    QHBoxLayout *aTop = new QHBoxLayout();
    QLabel *la = new QLabel("a", leftPanel);
    la->setStyleSheet("font-weight: bold; color: #ff6b6b; font-size: 11pt;");
    la->setFixedWidth(30);
    aTop->addWidget(la);
    spinA = new QDoubleSpinBox(leftPanel);
    spinA->setRange(0, 10); spinA->setValue(1.0); spinA->setFixedWidth(100);
    spinA->setDecimals(1); spinA->setSingleStep(0.1);
    spinA->setStyleSheet("font-weight: bold; font-size: 11pt;");
    aTop->addWidget(spinA); aTop->addStretch();
    layout->addLayout(aTop);
    sliderA = new QSlider(Qt::Horizontal, leftPanel);
    sliderA->setRange(0, 100); sliderA->setValue(10);
    sliderA->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#ff6b6b;border-radius:1.5px;}QSlider::handle:horizontal{background:#ff6b6b;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#ff6b6b;border-radius:1.5px;}");
    layout->addWidget(sliderA);
    layout->addSpacing(15);

    // B
    QHBoxLayout *bTop = new QHBoxLayout();
    QLabel *lb = new QLabel("b", leftPanel);
    lb->setStyleSheet("font-weight: bold; color: #4cd4b0; font-size: 11pt;");
    lb->setFixedWidth(30);
    bTop->addWidget(lb);
    spinB = new QDoubleSpinBox(leftPanel);
    spinB->setRange(0, 10); spinB->setValue(1.0); spinB->setFixedWidth(100);
    spinB->setDecimals(1); spinB->setSingleStep(0.1);
    spinB->setStyleSheet("font-weight: bold; font-size: 11pt;");
    bTop->addWidget(spinB); bTop->addStretch();
    layout->addLayout(bTop);
    sliderB = new QSlider(Qt::Horizontal, leftPanel);
    sliderB->setRange(0, 100); sliderB->setValue(10);
    sliderB->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#4cd4b0;border-radius:1.5px;}QSlider::handle:horizontal{background:#4cd4b0;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#4cd4b0;border-radius:1.5px;}");
    layout->addWidget(sliderB);
    layout->addSpacing(15);

    // C
    QHBoxLayout *cTop = new QHBoxLayout();
    QLabel *lc = new QLabel("c", leftPanel);
    lc->setStyleSheet("font-weight: bold; color: #74c0fc; font-size: 11pt;");
    lc->setFixedWidth(30);
    cTop->addWidget(lc);
    spinC = new QDoubleSpinBox(leftPanel);
    spinC->setRange(0, 10); spinC->setValue(1.0); spinC->setFixedWidth(100);
    spinC->setDecimals(1); spinC->setSingleStep(0.1);
    spinC->setStyleSheet("font-weight: bold; font-size: 11pt;");
    cTop->addWidget(spinC); cTop->addStretch();
    layout->addLayout(cTop);
    sliderC = new QSlider(Qt::Horizontal, leftPanel);
    sliderC->setRange(0, 100); sliderC->setValue(10);
    sliderC->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#74c0fc;border-radius:1.5px;}QSlider::handle:horizontal{background:#74c0fc;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#74c0fc;border-radius:1.5px;}");
    layout->addWidget(sliderC);
    layout->addSpacing(20);

    table = new QTableWidget(leftPanel);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels(QStringList() << "x" << "y");
    table->setMinimumHeight(200);
    layout->addWidget(table);
    layout->addStretch();

    userLabel = new QLabel(leftPanel);
    userLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(userLabel);

    logoutBtn = new QPushButton("Logout", leftPanel);
    logoutBtn->setStyleSheet("font-weight: bold; font-size: 10pt; padding: 6px;");
    layout->addWidget(logoutBtn);

    connect(sliderA, &QSlider::valueChanged, this, &GraphWidget::onSliderAChanged);
    connect(sliderB, &QSlider::valueChanged, this, &GraphWidget::onSliderBChanged);
    connect(sliderC, &QSlider::valueChanged, this, &GraphWidget::onSliderCChanged);
    connect(spinA, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinAChanged);
    connect(spinB, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinBChanged);
    connect(spinC, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinCChanged);
    connect(logoutBtn, &QPushButton::clicked, this, &GraphWidget::onLogoutClicked);
}