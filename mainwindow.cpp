#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Test DB Creation
    BudgetData *db = new BudgetData();
    // Connect UI Buttons to SLOTS
    connect(this->ui->addTransButton, &QPushButton::clicked, this, &MainWindow::addTransactionButtonClicked);
    connect(this->ui->modifyTransButton, &QPushButton::clicked, this, &MainWindow::modifyTransactionButtonClicked);
    connect(this->ui->updateCCTotalsButton, &QPushButton::clicked, this, &MainWindow::updateCCTotalsButtonClicked);
    connect(this->ui->modifyBillsButton, &QPushButton::clicked, this, &MainWindow::modifyBillsButtonClicked);
    connect(this->ui->showTransactionsButton, &QPushButton::clicked, this, &MainWindow::showTransactionsButtonClicked);
    connect(this->ui->showCCButton, &QPushButton::clicked, this, &MainWindow::showCreditDebtButtonClicked);
    connect(this->ui->showBillsButton, &QPushButton::clicked, this, &MainWindow::showBillsButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTransactionButtonClicked() {
    if (DEBUG) {
        qDebug() << "Add Transaction Button Clicked";
    }
}

void MainWindow::modifyTransactionButtonClicked() {
    if (DEBUG) {
        qDebug() << "Modify Transaction Button Clicked";
    }
}

void MainWindow::updateCCTotalsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Update CC Totals Button Clicked";
    }
}

void MainWindow::modifyBillsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Modify Bills Button Clicked";
    }
}

void MainWindow::showTransactionsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Transactions Button Clicked";
    }
}

void MainWindow::showCreditDebtButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Credit Debt Button Clicked";
    }
}

void MainWindow::showBillsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Bills Button Clicked";
    }
}












