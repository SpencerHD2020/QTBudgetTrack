#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "budgetdata.h"
#include "addtransactiondialog.h"
#include <QPushButton>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QVector>
#include <QStandardItemModel>
#include <QStandardItem>

#include "transaction.h"
#include "bill.h"
#include "credcard.h"
#include "modifybillsdialog.h"
#include "modifyccdialog.h"
#include "transactionitempointer.h"
#include "budgetexception.h"


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
    TransactionItemPointer findTIPFromPointer(QStandardItem *p);
    BudgetData *db = nullptr;
    enum ACTIVE_VIEWS {
        TRANSACTIONS,
        CC,
        BILLS
    };
    enum TOTALS_RETURN {
        ACCT_TOTAL,
        FIX_TOTAL
    };
    enum MOD_BILLS_CC_OPERATION {
        MODIFY_BCC,
        ADD_BCC
    };

    int activeView {};
    // NOTE: Eventually this should go on the heap
    QVector<Transaction> transactionList;
    QVector<TransactionItemPointer> transPointers;
    QStandardItemModel* model = new QStandardItemModel();
    QVector<Bill> billList;
    QVector<CredCard> cardList;


private slots:
    void addTransactionButtonClicked();
    void modifyTransactionButtonClicked();
    void updateCCTotalsButtonClicked();
    void modifyBillsButtonClicked();
    void showTransactionsButtonClicked();
    void showCreditDebtButtonClicked();
    void showBillsButtonClicked();
    void updateUITotals();
    void transModelChanged(QStandardItem *p);
public slots:
    void dbError(QString err);
    void updateTransactionsTable();
    void updateBillsTable();
    void updateCCTable();
};
#endif // MAINWINDOW_H
