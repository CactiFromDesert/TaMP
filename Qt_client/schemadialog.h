#ifndef SCHEMADIALOG_H
#define SCHEMADIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QWidget>

#ifndef UI_STUB
#define UI_STUB 1
#endif

/**
 * @brief Виджет для отображения блок-схемы алгоритма.
 * 
 * В текущей реализации показывает изображение из файла Schema.png.
 * При отсутствии файла выводит сообщение об ошибке.
 */
class FlowchartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FlowchartWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;  ///< Отрисовка (сейчас пустая, используется QLabel)
};

/**
 * @brief Диалоговое окно с блок-схемой вычислительного процесса.
 * 
 * Загружает и отображает изображение Schema.png.
 * При необходимости можно переключиться в режим программной отрисовки.
 */
class SchemaDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SchemaDialog(QWidget *parent = nullptr);
    ~SchemaDialog();

private:
    QPushButton     *closeBtn;      ///< Кнопка закрытия диалога
    FlowchartWidget *canvas;        ///< Холст для отрисовки (запасной вариант)
    QScrollArea     *scrollArea;    ///< Область прокрутки (для больших схем)
    QLabel *imageLabel;             ///< Метка для отображения изображения

    void setupUI();                 ///< Настройка интерфейса

#if UI_STUB
    void enableStubMode(bool enable = true);
    bool m_stubMode = true;
#endif
};

#endif // SCHEMADIALOG_H