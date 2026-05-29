#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = nullptr);
    QString serverIP() const;

private slots:
    void onConnect();

private:
    QLineEdit *ipEdit;
    QPushButton *connectBtn;
    QLabel *errorLabel;
};

#endif
