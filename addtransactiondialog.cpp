#include "addtransactiondialog.h"

AddTransactionDialog::AddTransactionDialog(bool DEBUG, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTransactionDialog)
{
    ui->setupUi(this);
    this->DEBUG = DEBUG;
    // Connect UI Components
    connect(this->ui->moneyAddedRB, &QRadioButton::clicked, this, [=]() {
        radioButtonChecked(MONEY_ADDED);
    });
    connect(this->ui->moneySubtractedRB, &QRadioButton::clicked, this, [=]() {
        radioButtonChecked(MONEY_SUBTRACTED);
    });
    connect(this->ui->continueButton, &QPushButton::clicked, this, &AddTransactionDialog::onContinueButtonClicked);
}

AddTransactionDialog::~AddTransactionDialog() {
    delete ui;
}

void AddTransactionDialog::radioButtonChecked(int descriptor) {
    if (descriptor == MONEY_ADDED) {
        // Uncheck Money Subtracted
        this->ui->moneySubtractedRB->setChecked(false);
    }
    else {
        this->ui->moneySubtractedRB->setChecked(false);
    }
}


void AddTransactionDialog::onContinueButtonClicked() {
    // Verify Minimum Input - Radio Buttons
    if (this->ui->moneyAddedRB->isChecked() || this->ui->moneySubtractedRB->isChecked()) {
        // Collect and Process Input
        description = this->ui->descriptionLineEdit->text();
        // Figure total Ammount of transaction
        ammount = (static_cast<double>(this->ui->dollarSpinBox->value()) + (static_cast<double>(this->ui->changeSpinBox->value()) / 100));
        if (this->ui->moneySubtractedRB->isChecked()) {
            // Negate the value
            ammount = ammount * -1.0;
        }
        // Figure the date

        if (DEBUG) {
            qDebug() << "Description: " << description;
            qDebug() << "Ammount: " << ammount;
        }
    }
    else {
        // TODO: Popup message to user requesting they check the proper button
        // NOTE: Could also change this to a ['+', '-'] spinbox
    }
}
