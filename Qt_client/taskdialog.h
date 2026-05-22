#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief Диалоговое окно с описанием задания.
 * 
 * Отображает информацию о:
 * - Формулировке задачи
 * - Участниках подгруппы №2
 * - Математической функции №10
 */
class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор диалога задания.
     * @param parent Родительский виджет
     */
    explicit TaskDialog(QWidget *parent = nullptr);
    ~TaskDialog();

private:
    QLabel *titleLabel;       ///< Заголовок "Задание"
    QLabel *workTitleLabel;   ///< Название работы
    QLabel *groupLabel;       ///< Номер подгруппы
    QLabel *membersLabel;     ///< Список участников
    QLabel *formulaLabel;     ///< Отображение функции с разными цветами для ветвей
    QPushButton *closeBtn;    ///< Кнопка закрытия

    void setupUI();           ///< Настройка интерфейса (тёмная тема, центрирование)
};

#endif // TASKDIALOG_H