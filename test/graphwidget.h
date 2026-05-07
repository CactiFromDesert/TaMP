#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

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

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    void setUserLogin(const QString &login);
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
    void onServerResponse(const QString &response);

private:
    QWidget *leftPanel;

    QLabel *formulaLabel;
    QSlider *sliderA;
    QSlider *sliderB;
    QSlider *sliderC;

    QDoubleSpinBox *spinA;
    QDoubleSpinBox *spinB;
    QDoubleSpinBox *spinC;

    QTableWidget *table;
    QPushButton *logoutBtn;
    QLabel *userLabel;

    QVector<QPointF> pointsBranch1;
    QVector<QPointF> pointsBranch2;
    QVector<QPointF> pointsBranch3;

    double currentA = 1.0;
    double currentB = 1.0;
    double currentC = 1.0;

    QString userLogin;

    void setupUI();
    void setupLeftPanel();
    void fillTable(double a, double b, double c);
    void requestServerUpdate(double a, double b, double c);
};

#endif // GRAPHWIDGET_H