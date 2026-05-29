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

/**
 * @brief Виджет для отрисовки графика математической функции.
 * 
 * Отвечает за визуализацию трёх ветвей функции:
 * - f₁(x) = √(-x) + a   (x < 0)
 * - f₂(x) = arcsin(x/π) + b   (0 ≤ x < π)
 * - f₃(x) = arccos(x/π) - c   (x ≥ π)
 */
class GraphCanvas : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор холста для графика.
     * @param parent Родительский виджет
     */
    explicit GraphCanvas(QWidget *parent = nullptr);

    /**
     * @brief Устанавливает точки для отрисовки трёх ветвей функции.
     * @param branch1 Точки для x < 0 (красный цвет)
     * @param branch2 Точки для 0 ≤ x < π (зелёный цвет)
     * @param branch3 Точки для x ≥ π (синий цвет)
     */
    void setPoints(const QVector<QPointF> &branch1,
                   const QVector<QPointF> &branch2,
                   const QVector<QPointF> &branch3);

protected:
    /**
     * @brief Обработчик события перерисовки виджета.
     * @param event Событие перерисовки
     * 
     * Рисует:
     * - Координатные оси
     * - Сетку с делениями
     * - Подписи чисел и π
     * - Три ветви функции разными цветами
     */
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> m_branch1;  ///< Точки первой ветви (x < 0)
    QVector<QPointF> m_branch2;  ///< Точки второй ветви (0 ≤ x < π)
    QVector<QPointF> m_branch3;  ///< Точки третьей ветви (x ≥ π)
};

/**
 * @brief Главный виджет для управления графиком.
 * 
 * Содержит элементы управления (слайдеры, спинбоксы, таблицу)
 * и взаимодействует с сервером для получения данных.
 */
class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета графика.
     * @param parent Родительский виджет
     */
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    /**
     * @brief Устанавливает логин пользователя для отображения.
     * @param login Имя пользователя
     */
    void setUserLogin(const QString &login);

    /**
     * @brief Обновляет график с текущими параметрами a,b,c.
     * 
     * Отправляет запрос на сервер и перерисовывает холст.
     */
    void updateGraph();

signals:
    /**
     * @brief Сигнал выхода из системы.
     * 
     * Испускается при нажатии кнопки Logout.
     */
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
    QWidget *leftPanel;                ///< Левая панель с элементами управления
    GraphCanvas *canvas;               ///< Холст для отрисовки графика
    QLabel *formulaLabel;              ///< Отображение формулы функции
    QSlider *sliderA, *sliderB, *sliderC;  ///< Слайдеры для параметров
    QDoubleSpinBox *spinA, *spinB, *spinC; ///< Числовые поля для параметров
    QTableWidget *table;               ///< Таблица со значениями x и y
    QPushButton *logoutBtn;            ///< Кнопка выхода
    QLabel *userLabel;                 ///< Метка с именем пользователя

    QVector<QPointF> pointsBranch1;    ///< Кэш точек первой ветви
    QVector<QPointF> pointsBranch2;    ///< Кэш точек второй ветви
    QVector<QPointF> pointsBranch3;    ///< Кэш точек третьей ветви

    double currentA = 1.0, currentB = 1.0, currentC = 1.0;  ///< Текущие параметры
    QString userLogin;                 ///< Логин текущего пользователя

    void setupUI();                    ///< Настройка пользовательского интерфейса
    void setupLeftPanel();             ///< Настройка левой панели управления
    void fillTable(double a, double b, double c);  ///< Заполнение таблицы значений
    void requestServerUpdate(double a, double b, double c);  ///< Запрос данных от сервера
};

#endif // GRAPHWIDGET_H