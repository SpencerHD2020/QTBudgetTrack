#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "budgetdata.h"
#include <QPushButton>
#include <QDebug>

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

private slots:
    void addTransactionButtonClicked();
    void modifyTransactionButtonClicked();
    void updateCCTotalsButtonClicked();
    void modifyBillsButtonClicked();
    void showTransactionsButtonClicked();
    void showCreditDebtButtonClicked();
    void showBillsButtonClicked();
};
#endif // MAINWINDOW_H
