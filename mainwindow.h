#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "budgetdata.h"
#include "addtransactiondialog.h"
#include <QPushButton>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool DEBUG {true};
    void transactionInfoAdded(QString description, double ammt, QString date);
    BudgetData *db = nullptr;


private slots:
    void addTransactionButtonClicked();
    void modifyTransactionButtonClicked();
    void updateCCTotalsButtonClicked();
    void modifyBillsButtonClicked();
    void showTransactionsButtonClicked();
    void showCreditDebtButtonClicked();
    void showBillsButtonClicked();
public slots:
    void dbError(QString err);
    void updateTransactionsTable();
};
#endif // MAINWINDOW_H
