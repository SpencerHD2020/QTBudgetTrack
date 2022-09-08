#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Instantiate DB Instance
    db = new BudgetData();
    // Connect UI Buttons to SLOTS
    connect(this->ui->addTransButton, &QPushButton::clicked, this, &MainWindow::addTransactionButtonClicked);
    connect(this->ui->modifyTransButton, &QPushButton::clicked, this, &MainWindow::modifyTransactionButtonClicked);
    connect(this->ui->updateCCTotalsButton, &QPushButton::clicked, this, &MainWindow::updateCCTotalsButtonClicked);
    connect(this->ui->modifyBillsButton, &QPushButton::clicked, this, &MainWindow::modifyBillsButtonClicked);
    connect(this->ui->showTransactionsButton, &QPushButton::clicked, this, &MainWindow::showTransactionsButtonClicked);
    connect(this->ui->showCCButton, &QPushButton::clicked, this, &MainWindow::showCreditDebtButtonClicked);
    connect(this->ui->showBillsButton, &QPushButton::clicked, this, &MainWindow::showBillsButtonClicked);
    // Connect DB SIGNALS to SLOT
    connect(db, &BudgetData::dbMutationFailed, this, &MainWindow::dbError);
    connect(db, &BudgetData::transactionsUpdated, this, &MainWindow::updateTransactionsTable);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTransactionButtonClicked() {
    if (DEBUG) {
        qDebug() << "Add Transaction Button Clicked";
    }
    AddTransactionDialog *addTransDialog = new AddTransactionDialog(DEBUG, this);
    int ret = addTransDialog->exec();
    if (ret == QDialog::Accepted) {
        if (DEBUG) {
            qDebug() << "MainWindow: User Accepted add trans dialog!";
        }
        transactionInfoAdded(addTransDialog->getDescription(), addTransDialog->getAmmount(), addTransDialog->getDate());
    }
    else if (DEBUG) {
        qDebug() << "MainWindow: User canceled add trans dialog!";
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



void MainWindow::transactionInfoAdded(QString description, double ammt, QString date) {
    if (DEBUG) {
        qDebug() << "MainWindow: Transaction Info Received!";
        qDebug() << "Description: " << description;
        qDebug() << "Ammount: $" << ammt;
        qDebug() << "Date: " << date;
    }
    // Add to SQL DB
    db->addTransaction(description, ammt, date);
}



void MainWindow::dbError(QString err) {
    QMessageBox errDialog;
    errDialog.critical(0, "Database Error", err);
    errDialog.setFixedSize(500, 200);
}


void MainWindow::updateTransactionsTable() {
    // TODO: Update Transactions Table on UI IFFFFFF Transactions table is currently shown, else do nothing
}









