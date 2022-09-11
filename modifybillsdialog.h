#ifndef MODIFYBILLSDIALOG_H
#define MODIFYBILLSDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QStringList>
#include <QMessageBox>
#include "ui_modifybillsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class modifybillsdialog;
}
QT_END_NAMESPACE

class ModifyBillsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModifyBillsDialog(QStringList bills, bool debug = false, QWidget *parent = nullptr);
    ~ModifyBillsDialog();
    enum USER_OPERATION {
        MODIFY_BILL,
        ADD_Bill
    };
    int getLastUserOperation() { return lastUserOperation; };
    QString getBillName() { return billName; };
    double getAmmt() { return ammount; };

private:
    Ui::modifybillsdialog *ui;
    QStringList userBills {"No Selection"};
    bool DEBUG {false};
    int lastUserOperation {0};
    QString billName {};
    double ammount {0.0};
private slots:
    void onChangeBillButtonClicked();
    void onAddBillButtonClicked();
    void onCancelButtonClicked();
};

#endif // MODIFYBILLSDIALOG_H
