#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QRadioButton>
#include <QPushButton>
#include "ui_AddTransactionDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AddTransactionDialog;
}
QT_END_NAMESPACE


class AddTransactionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddTransactionDialog(bool DEBUG = false, QWidget *parent = nullptr);
    ~AddTransactionDialog();
    QString getDescription();
    double getAmmount();
    QString getDate();
    enum RADIO_BUTTONS {
        MONEY_ADDED,
        MONEY_SUBTRACTED
    };

private slots:
    void onContinueButtonClicked();
    void onCancelButtonClicked();
    void radioButtonChecked(int descriptor);
private:
    Ui::AddTransactionDialog *ui;
    QString description;
    double ammount;
    QString date;
    bool DEBUG;
};

#endif // ADDTRANSACTIONDIALOG_H
