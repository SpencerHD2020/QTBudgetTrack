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
    connect(db, &BudgetData::billsUpdated, this, &MainWindow::updateBillsTable);
    connect(db, &BudgetData::ccUpdated, this, &MainWindow::updateCCTable);
    // Connect Transaction Model SIGNALS to SLOTS
    connect(model, &QStandardItemModel::itemChanged, this, &MainWindow::transModelChanged);
    // Populate the Transactions Table on App Start
    activeView = TRANSACTIONS;
    updateTransactionsTable();
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
    QStringList cardNames {db->fetchCCNames()};
    modifyccdialog *modifyCCDialog = new modifyccdialog(cardNames, DEBUG, this);
    int ret = modifyCCDialog->exec();
    if (ret == QDialog::Accepted) {
        if (DEBUG) {
            qDebug() << "MainWindow: User Accepted modify cc dialog";
        }
        // Process Update
        if (modifyCCDialog->getLastUserOperation() == MODIFY_BCC) {
            // User is changing an existing card debt
            db->modifyCC(modifyCCDialog->getCCName(), modifyCCDialog->getAmmt());
        }
        else if (modifyCCDialog->getLastUserOperation() == ADD_BCC) {
            // User created a new card
            db->addCC(modifyCCDialog->getCCName(), modifyCCDialog->getAmmt());
        }
    }
    else if (DEBUG) {
        qDebug() << "MainWindow: User canceled modify cc dialog";
    }
}

void MainWindow::modifyBillsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Modify Bills Button Clicked";
    }
    // Fetch Current Bills - TODO: Also fetch their values and populate those in the dialog!
    QStringList billNames {db->fetchBillNames()};
    ModifyBillsDialog *modifyBillsDialog = new ModifyBillsDialog(billNames, DEBUG, this);
    int ret = modifyBillsDialog->exec();
    if (ret == QDialog::Accepted) {
        if (DEBUG) {
            qDebug() << "MainWindow: User Accepted modify bills dialog";
        }
        // Process Update
        if (modifyBillsDialog->getLastUserOperation() == MODIFY_BCC) {
            // User is changing an existing bill
            db->modifyBill(modifyBillsDialog->getBillName(), modifyBillsDialog->getAmmt());
        }
        else if (modifyBillsDialog->getLastUserOperation() == ADD_BCC) {
            // User created a new bill
            db->addBill(modifyBillsDialog->getBillName(), modifyBillsDialog->getAmmt());
        }
    }
    else if (DEBUG) {
        qDebug() << "MainWindow: User canceled modify bills dialog";
    }
}

void MainWindow::showTransactionsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Transactions Button Clicked";
    }
    if (activeView != TRANSACTIONS) {
        activeView = TRANSACTIONS;
        updateTransactionsTable();
    }
}

void MainWindow::showCreditDebtButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Credit Debt Button Clicked";
    }
    if (activeView != CC) {
        activeView = CC;
        updateCCTable();
    }
}

void MainWindow::showBillsButtonClicked() {
    if (DEBUG) {
        qDebug() << "Show Bills Button Clicked";
    }
    if (activeView != BILLS) {
        activeView = BILLS;
        updateBillsTable();
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


void MainWindow::updateUITotals() {
    QStringList totals {db->fetchTotals()};
    this->ui->acctTotalValue->setText(totals[ACCT_TOTAL]);
    this->ui->acctFixedValue->setText(totals[FIX_TOTAL]);
}


void MainWindow::updateTransactionsTable() {
    // Update Transactions Table on UI if Transactions table is currently shown
    if (activeView == TRANSACTIONS) {
        // Throw away old list
        transPointers.clear();
        transactionList.clear();
        transactionList = db->fetchTransactions();
        if (transactionList.length() > 0) {
            // (Re)Build Table View
            this->ui->tableView->clearSpans();
            // Clear the model
            model->clear();
            this->ui->tableView->setModel(model);

            // Configure column Titles
            model->setHorizontalHeaderItem(0, new QStandardItem("Date"));
            model->setHorizontalHeaderItem(1, new QStandardItem("Description"));
            model->setHorizontalHeaderItem(2, new QStandardItem("Price"));

            // Populate Table Rows
            QList<QStandardItem*> rowData;
            Q_FOREACH(auto const &item, transactionList) {
                rowData.clear();
                rowData << new QStandardItem(item.getDate());
                rowData << new QStandardItem(item.getDescription());
                rowData << new QStandardItem(item.getSAmmt());
                model->appendRow(rowData);
                // Add Data + Pointers to transPointers vector
                transPointers.append(TransactionItemPointer(item.getDescription(), item.getDate(), item.getSAmmt(), rowData[1], rowData[0], rowData[2]));
            }
        }
        else {
            // Clear the UI
            this->ui->tableView->clearSpans();
        }
    }
    // Regardless of if transactions are shown or not, we need to update the totals
    updateUITotals();
}



TransactionItemPointer MainWindow::findTIPFromPointer(QStandardItem *p) {
    bool foundTip {false};
    for (auto tip: transPointers) {
        if (tip.checkPointer(p) != "-1") {
            foundTip = true;
            return tip;
        }
    }
    if (!foundTip) {
        BudgetException *b {new BudgetException("[MainWindow::findTIPFromPointer]: Passed pointer did not match any in Transaction Model!")};
        b->raise();
    }
}



void MainWindow::transModelChanged(QStandardItem *p) {
    if (DEBUG) {
        qDebug() << "Transaction Model Changed!";
    }
    // Connection Tested and working
    try {
        TransactionItemPointer targetTransaction {findTIPFromPointer(p)};
    }
    catch (BudgetException &e) {
        qDebug() << "EXCEPTION OCCURED: " << e.issue();
    }

}


void MainWindow::updateBillsTable() {
    // Update Bills Table on UI if BILLS table is currently shown
    if (activeView == BILLS) {
        // Destroy old list
        billList.clear();
        billList = db->fetchBills();
        if (billList.length() > 0) {
            // (Re)Build Table View
            this->ui->tableView->clearSpans();
            auto model = new QStandardItemModel();
            this->ui->tableView->setModel(model);

            // Configure column Titles
            model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
            model->setHorizontalHeaderItem(1, new QStandardItem("Ammount"));
            model->setHorizontalHeaderItem(2, new QStandardItem("Held"));

            // Populate Table Rows
            QList<QStandardItem*> rowData;
            Q_FOREACH(auto const &item, billList) {
                rowData.clear();
                rowData << new QStandardItem(item.getName());
                rowData << new QStandardItem(item.getSAmmt());
                rowData << new QStandardItem(item.getSAmmt());
                model->appendRow(rowData);
            }
        }
        else {
            // Clear the UI
            this->ui->tableView->clearSpans();
        }
    }
    // Regardless of if bills are shown or not, we need to update the totals
    updateUITotals();
}


void MainWindow::updateCCTable() {
    // Update CC Table on UI if CC table is currently shown
    if (activeView == CC) {
        // Destroy old list
        cardList.clear();
        cardList = db->fetchCCData();
        if (cardList.length() > 0) {
            // (Re)Build Table View
            this->ui->tableView->clearSpans();
            auto model = new QStandardItemModel();
            this->ui->tableView->setModel(model);

            // Configure column Titles
            model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
            model->setHorizontalHeaderItem(1, new QStandardItem("Debt Owed"));

            // Populate Table Rows
            QList<QStandardItem*> rowData;
            Q_FOREACH(auto const &item, cardList) {
                rowData.clear();
                rowData << new QStandardItem(item.getDescription());
                rowData << new QStandardItem(item.getSAmmt());
                model->appendRow(rowData);
            }
        }
        else {
            // Clear the UI
            this->ui->tableView->clearSpans();
        }
    }
    // Regardless of if bills are shown or not, we need to update the totals
    updateUITotals();
}








