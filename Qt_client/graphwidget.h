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

class GraphCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit GraphCanvas(QWidget *parent = nullptr);

    void setPoints(const QVector<QPointF> &branch1,
                   const QVector<QPointF> &branch2,
                   const QVector<QPointF> &branch3);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> m_branch1;
    QVector<QPointF> m_branch2;
    QVector<QPointF> m_branch3;
};

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
    GraphCanvas *canvas;

    QLabel *formulaLabel;
    QSlider *sliderA, *sliderB, *sliderC;
    QDoubleSpinBox *spinA, *spinB, *spinC;
    QTableWidget *table;
    QPushButton *logoutBtn;
    QLabel *userLabel;

    QVector<QPointF> pointsBranch1;
    QVector<QPointF> pointsBranch2;
    QVector<QPointF> pointsBranch3;

    double currentA = 1.0, currentB = 1.0, currentC = 1.0;
    QString userLogin;

    void setupUI();
    void setupLeftPanel();
    void fillTable(double a, double b, double c);
    void requestServerUpdate(double a, double b, double c);
};

#endif // GRAPHWIDGET_H