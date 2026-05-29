#include "graphwidget.h"
#include "clientsingleton.h"

#include <QPainter>
#include <QPen>
#include <QJsonDocument>
#include <QJsonArray>
#include <cmath>
#include <QHeaderView>

const double PI = 3.14159265358979323846;

// ─────────────────────────────────────────
// GraphCanvas
// ─────────────────────────────────────────
GraphCanvas::GraphCanvas(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(600, 500);
    setStyleSheet("background-color: #0a1929;");
}

void GraphCanvas::setPoints(const QVector<QPointF> &b1,
                             const QVector<QPointF> &b2,
                             const QVector<QPointF> &b3)
{
    m_branch1 = b1;
    m_branch2 = b2;
    m_branch3 = b3;
    update();
}

void GraphCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    int margin = 50;

    double xmin = -2.5, xmax = PI + 1.0;
    double ymin = -5.5, ymax = 5.5;

    auto toScreen = [&](double x, double y) -> QPointF {
        double sx = margin + (x - xmin) / (xmax - xmin) * (w - 2 * margin);
        double sy = margin + (ymax - y) / (ymax - ymin) * (h - 2 * margin);
        return QPointF(sx, sy);
    };

    // Область отсечения — без выезда за границы
    QRect chartRect(margin, margin, w - 2*margin, h - 2*margin);
    p.setClipRect(chartRect);

    p.fillRect(rect(), QColor("#0a1929"));

    // Сетка
    p.setPen(QPen(QColor("#1a3a5c"), 0.5));
    for (int i = -2; i <= (int)(xmax); i++)
        p.drawLine(toScreen(i, ymin), toScreen(i, ymax));
    for (int j = -5; j <= 5; j++)
        p.drawLine(toScreen(xmin, j), toScreen(xmax, j));

    // Оси
    p.setPen(QPen(QColor("#e6f2ff"), 2));
    p.drawLine(toScreen(xmin, 0), toScreen(xmax, 0));
    p.drawLine(toScreen(0, ymin), toScreen(0, ymax));

    // Метки
    p.setPen(QColor("#9ecae6"));
    QFont f = p.font();
    f.setPixelSize(10);
    f.setBold(true);
    p.setFont(f);

    // X
    for (int i = -2; i <= (int)(xmax); i++) {
        QPointF pt = toScreen(i, 0);
        p.drawLine(pt + QPointF(0, -3), pt + QPointF(0, 3));
        p.drawText(QRectF(pt.x()-12, pt.y()+4, 24, 14), Qt::AlignCenter, QString::number(i));
    }
    // π
    QPointF piPt = toScreen(PI, 0);
    p.drawLine(piPt + QPointF(0, -3), piPt + QPointF(0, 3));
    p.drawText(QRectF(piPt.x()-12, piPt.y()+4, 24, 14), Qt::AlignCenter, "π");

    // Y
    for (int j = -5; j <= 5; j++) {
        QPointF pt = toScreen(0, j);
        p.drawLine(pt + QPointF(-3, 0), pt + QPointF(3, 0));
        p.drawText(QRectF(pt.x()-32, pt.y()-7, 28, 14), Qt::AlignRight | Qt::AlignVCenter, QString::number(j));
    }

    auto drawBranch = [&](const QVector<QPointF> &pts, const QColor &color) {
        if (pts.size() < 2) return;
        QPen pen(color, 2.5);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        p.setPen(pen);
        for (int i = 0; i < pts.size() - 1; i++) {
            if (std::isnan(pts[i].y()) || std::isnan(pts[i+1].y())) continue;
            QPointF s1 = toScreen(pts[i].x(), pts[i].y());
            QPointF s2 = toScreen(pts[i+1].x(), pts[i+1].y());
            p.drawLine(s1, s2);
        }
    };

    drawBranch(m_branch1, QColor("#ff6b6b"));  // x < 0
    drawBranch(m_branch2, QColor("#4cd4b0"));  // 0 ≤ x < π
    drawBranch(m_branch3, QColor("#74c0fc"));  // x ≥ π
}

// ─────────────────────────────────────────
// GraphWidget
// ─────────────────────────────────────────
GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    ClientSingleton::instance().connectToServer("127.0.0.1", 11999);
    connect(&ClientSingleton::instance(), &ClientSingleton::responseReceived,
            this, &GraphWidget::onServerResponse);
    if (spinA && spinB && spinC) updateGraph();
}

GraphWidget::~GraphWidget() = default;

void GraphWidget::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(380);
    setupLeftPanel();

    canvas = new GraphCanvas(this);
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(canvas, 1);
    setLayout(mainLayout);

    setStyleSheet("font-weight: bold; QTableWidget, QTableWidget::item { font-weight: normal; }");
}

void GraphWidget::setupLeftPanel()
{
    auto *layout = new QVBoxLayout(leftPanel);
    layout->setSpacing(6);
    layout->setContentsMargins(8, 8, 8, 8);

    formulaLabel = new QLabel("f(x) = √(-x)+a | arcsin(x/π)+b | arccos(x/π)-c", leftPanel);
    formulaLabel->setStyleSheet("font-weight: bold; font-size: 9pt; color: #e6f2ff;");
    formulaLabel->setWordWrap(true);
    formulaLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(formulaLabel);

    // A
    auto *aRow = new QHBoxLayout();
    QLabel *la = new QLabel("a", leftPanel);
    la->setStyleSheet("font-weight: bold; color: #ff6b6b; font-size: 11pt;");
    la->setFixedWidth(20);
    aRow->addWidget(la);
    spinA = new QDoubleSpinBox(leftPanel);
    spinA->setRange(-5,5); spinA->setValue(1.0); spinA->setFixedWidth(90);
    spinA->setDecimals(1); spinA->setSingleStep(0.1);
    spinA->setStyleSheet("font-weight: bold; font-size: 11pt; background:#102a43; color:#e6f2ff;");
    aRow->addWidget(spinA); aRow->addStretch();
    layout->addLayout(aRow);
    sliderA = new QSlider(Qt::Horizontal, leftPanel);
    sliderA->setRange(-50,50); sliderA->setValue(10);
    sliderA->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#ff6b6b;border-radius:1.5px;}QSlider::handle:horizontal{background:#ff6b6b;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#ff6b6b;border-radius:1.5px;}");
    layout->addWidget(sliderA);

    // B
    auto *bRow = new QHBoxLayout();
    QLabel *lb = new QLabel("b", leftPanel);
    lb->setStyleSheet("font-weight: bold; color: #4cd4b0; font-size: 11pt;");
    lb->setFixedWidth(20);
    bRow->addWidget(lb);
    spinB = new QDoubleSpinBox(leftPanel);
    spinB->setRange(-5,5); spinB->setValue(1.0); spinB->setFixedWidth(90);
    spinB->setDecimals(1); spinB->setSingleStep(0.1);
    spinB->setStyleSheet("font-weight: bold; font-size: 11pt; background:#102a43; color:#e6f2ff;");
    bRow->addWidget(spinB); bRow->addStretch();
    layout->addLayout(bRow);
    sliderB = new QSlider(Qt::Horizontal, leftPanel);
    sliderB->setRange(-50,50); sliderB->setValue(10);
    sliderB->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#4cd4b0;border-radius:1.5px;}QSlider::handle:horizontal{background:#4cd4b0;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#4cd4b0;border-radius:1.5px;}");
    layout->addWidget(sliderB);

    // C
    auto *cRow = new QHBoxLayout();
    QLabel *lc = new QLabel("c", leftPanel);
    lc->setStyleSheet("font-weight: bold; color: #74c0fc; font-size: 11pt;");
    lc->setFixedWidth(20);
    cRow->addWidget(lc);
    spinC = new QDoubleSpinBox(leftPanel);
    spinC->setRange(-5,5); spinC->setValue(1.0); spinC->setFixedWidth(90);
    spinC->setDecimals(1); spinC->setSingleStep(0.1);
    spinC->setStyleSheet("font-weight: bold; font-size: 11pt; background:#102a43; color:#e6f2ff;");
    cRow->addWidget(spinC); cRow->addStretch();
    layout->addLayout(cRow);
    sliderC = new QSlider(Qt::Horizontal, leftPanel);
    sliderC->setRange(-50,50); sliderC->setValue(10);
    sliderC->setStyleSheet("QSlider::groove:horizontal{height:3px;background:#74c0fc;border-radius:1.5px;}QSlider::handle:horizontal{background:#74c0fc;width:10px;height:10px;margin:-4px 0;border-radius:5px;}QSlider::sub-page:horizontal{background:#74c0fc;border-radius:1.5px;}");
    layout->addWidget(sliderC);

    layout->addSpacing(5);

    // Таблица — используется всё пространство
    table = new QTableWidget(leftPanel);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels(QStringList() << "x" << "y");
    table->setStyleSheet(
        "QTableWidget { background:#102a43; color:#e6f2ff; gridline-color:#1f4e79; font-size:10pt; }"
        "QTableWidget::item { padding:2px; }"
        "QHeaderView::section { background:#3da9fc; color:white; font-weight:bold; }"
    );
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setMinimumHeight(200);
    layout->addWidget(table, 1); // занимает всё доступное место

    userLabel = new QLabel(leftPanel);
    userLabel->setStyleSheet("font-weight: bold; color:#e6f2ff;");
    layout->addWidget(userLabel);

    logoutBtn = new QPushButton("Logout", leftPanel);
    logoutBtn->setStyleSheet("font-weight: bold; font-size: 10pt; padding: 6px; background:#3da9fc; color:white; border-radius:6px;");
    layout->addWidget(logoutBtn);

    connect(sliderA, &QSlider::valueChanged, this, &GraphWidget::onSliderAChanged);
    connect(sliderB, &QSlider::valueChanged, this, &GraphWidget::onSliderBChanged);
    connect(sliderC, &QSlider::valueChanged, this, &GraphWidget::onSliderCChanged);
    connect(spinA, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinAChanged);
    connect(spinB, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinBChanged);
    connect(spinC, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GraphWidget::onSpinCChanged);
    connect(logoutBtn, &QPushButton::clicked, this, &GraphWidget::onLogoutClicked);
}

// Слайдеры и спиннеры
void GraphWidget::onSliderAChanged(int v) { if(spinA){spinA->blockSignals(true);spinA->setValue(v/10.0);spinA->blockSignals(false);} updateGraph(); }
void GraphWidget::onSliderBChanged(int v) { if(spinB){spinB->blockSignals(true);spinB->setValue(v/10.0);spinB->blockSignals(false);} updateGraph(); }
void GraphWidget::onSliderCChanged(int v) { if(spinC){spinC->blockSignals(true);spinC->setValue(v/10.0);spinC->blockSignals(false);} updateGraph(); }
void GraphWidget::onSpinAChanged(double v) { if(sliderA){sliderA->blockSignals(true);sliderA->setValue(int(v*10));sliderA->blockSignals(false);} updateGraph(); }
void GraphWidget::onSpinBChanged(double v) { if(sliderB){sliderB->blockSignals(true);sliderB->setValue(int(v*10));sliderB->blockSignals(false);} updateGraph(); }
void GraphWidget::onSpinCChanged(double v) { if(sliderC){sliderC->blockSignals(true);sliderC->setValue(int(v*10));sliderC->blockSignals(false);} updateGraph(); }

void GraphWidget::updateGraph()
{
    if (!spinA || !spinB || !spinC) return;
    currentA = spinA->value();
    currentB = spinB->value();
    currentC = spinC->value();
    requestServerUpdate(currentA, currentB, currentC);
}

void GraphWidget::requestServerUpdate(double a, double b, double c)
{
    QString req = QString("%1;%2;%3").arg(a,0,'f',2).arg(b,0,'f',2).arg(c,0,'f',2);
    ClientSingleton::instance().sendRequest(req);
}

void GraphWidget::onServerResponse(const QString &response)
{
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
        if (pt[1].isNull()) continue;

        if (x < 0)           pointsBranch1.append(QPointF(x,y));
        else if (x < PI)     pointsBranch2.append(QPointF(x,y));
        else                 pointsBranch3.append(QPointF(x,y));
    }

    canvas->setPoints(pointsBranch1, pointsBranch2, pointsBranch3);
    fillTable(currentA, currentB, currentC);
}

void GraphWidget::fillTable(double, double, double)
{
    if (!table) return;

    // Выбираем 21 точку с равномерным шагом по всем данным
    QVector<QPointF> all;
    all.append(pointsBranch1);
    all.append(pointsBranch2);
    all.append(pointsBranch3);
    std::sort(all.begin(), all.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });

    int n = all.size();
    table->setRowCount(21);

    for (int i = 0; i < 21 && i < n; i++) {
        int idx = i * (n - 1) / 20; // равномерно распределяем индексы
        double x = all[idx].x();
        double y = all[idx].y();
        table->setItem(i, 0, new QTableWidgetItem(QString::number(x, 'f', 2)));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(y, 'f', 4)));
    }
}

void GraphWidget::onLogoutClicked() { emit logout(); }
void GraphWidget::setUserLogin(const QString &login)
{
    userLogin = login;
    if (userLabel) userLabel->setText("Пользователь: " + login);
}