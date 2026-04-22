#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "func.hpp"

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    void setUserLogin(const QString &login);

    // ✅ ВАЖНО: теперь доступен из MainWindow
    void updateGraph();

signals:
    void logout();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onSliderAChanged(int value);
    void onSliderBChanged(int value);
    void onSliderCChanged(int value);
    void onSpinAChanged(double value);
    void onSpinBChanged(double value);
    void onSpinCChanged(double value);
    void onLogoutClicked();

private:
    // UI
    QWidget *leftPanel;

    QLabel *formulaLabel;
    QLabel *labelA;
    QLabel *labelB;
    QLabel *labelC;

    QSlider *sliderA;
    QSlider *sliderB;
    QSlider *sliderC;

    QDoubleSpinBox *spinA;
    QDoubleSpinBox *spinB;
    QDoubleSpinBox *spinC;

    QTableWidget *table;
    QPushButton *logoutBtn;
    QLabel *userLabel;

    static const int LEFT_PANEL_WIDTH = 280;

    // Data
    QVector<QPointF> pointsBranch1;
    QVector<QPointF> pointsBranch2;
    QVector<QPointF> pointsBranch3;

    double currentA;
    double currentB;
    double currentC;

    QString userLogin;

    // Logic
    void setupUI();
    void setupLeftPanel();
    void updateFormulaLabel();
    void fillTable(double a, double b, double c);
    double calculate(double x, double a, double b, double c) const;

    // Anti-loop flags
    bool blockSliderA = false;
    bool blockSliderB = false;
    bool blockSliderC = false;

    bool blockSpinA = false;
    bool blockSpinB = false;
    bool blockSpinC = false;
};

#endif // GRAPHWIDGET_H
