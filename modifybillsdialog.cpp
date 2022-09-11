#include "modifybillsdialog.h"

ModifyBillsDialog::ModifyBillsDialog(QStringList bills, bool debug, QWidget *parent) : DEBUG(debug), QDialog(parent),
    ui(new Ui::modifybillsdialog)
{
    ui->setupUi(this);
    // Loop through bills and add each item to userBills
    for (const auto &t : bills) {
        userBills.append(t);
    }
    // Fill the Dropdown box
    this->ui->billSelector->addItems(userBills);
    // Connect UI Buttons to SLOTS
    connect(this->ui->modifyBillPushButton, &QPushButton::clicked, this, &ModifyBillsDialog::onChangeBillButtonClicked);
    connect(this->ui->addBillButton, &QPushButton::clicked, this, &ModifyBillsDialog::onAddBillButtonClicked);
    connect(this->ui->cancelButton, &QPushButton::clicked, this, &ModifyBillsDialog::onCancelButtonClicked);

}

ModifyBillsDialog::~ModifyBillsDialog() {
    delete ui;
}


void ModifyBillsDialog::onChangeBillButtonClicked() {
    // Set Last User Operation
    lastUserOperation = MODIFY_BILL;
    // Get Current Selection and ensure validity
    int billChoice {this->ui->billSelector->currentIndex()};
    if (billChoice > 0) {
        // Populate bill name
        billName = userBills[billChoice];
        // Get new Cost Associated to Bill
        ammount = this->ui->newAmmtDSB->value();
        // Accept Dialog
        accept();
    }
    else {
        // Show Error Message or do nothing?
        QMessageBox err;
        err.warning(0, "Warning", "Please select a valid bill to modify or add a new bill.");
        err.setFixedSize(500, 200);
    }
}

void ModifyBillsDialog::onAddBillButtonClicked() {
    // Set Last User Operation
    lastUserOperation = ADD_Bill;
    // Ensure new bill is not already a current bill!
    QString newBillName {this->ui->newBillName->text()};
    if (!userBills.contains(newBillName)) {
        // Populate bill name
        billName = newBillName;
        // Get cost associated to bill
        ammount = this->ui->newBillAmmtDSB->value();
        // Accept Dialog
        accept();
    }
    else {
        // Show error message
        QMessageBox err;
        err.warning(0, "Warning", "New Bill Description matches that of an existing bill!");
        err.setFixedSize(500, 200);
    }
}

void ModifyBillsDialog::onCancelButtonClicked() {
    if (DEBUG) {
        qDebug() << "[ModifyBillsDialog::onCancelButtonClicked]: CANCEL BUTTON CLICKED!";
    }
    reject();
}
