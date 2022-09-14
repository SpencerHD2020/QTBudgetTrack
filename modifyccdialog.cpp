#include "modifyccdialog.h"

modifyccdialog::modifyccdialog(QStringList cards, bool debug, QWidget *parent) : DEBUG(debug), QDialog(parent),
    ui(new Ui::modifyccdialog)
{
    ui->setupUi(this);
    // Loop through cards and add each to userCards
    for (const auto &t : cards) {
        userCards.append(t);
    }
    // Fill the dropdown box on UI
    this->ui->ccSelector->addItems(userCards);
    // Connect UI Buttons to SLOTS
    connect(this->ui->updateCCButton, &QPushButton::clicked, this, &modifyccdialog::onChangeCCButtonClicked);
    connect(this->ui->addCCButton, &QPushButton::clicked, this, &modifyccdialog::onAddCCButtonClicked);
    connect(this->ui->cancelButton, &QPushButton::clicked, this, &modifyccdialog::onCancelButtonClicked);
}

modifyccdialog::~modifyccdialog() {
    delete ui;
}


void modifyccdialog::onChangeCCButtonClicked() {
    // Set last User Operation
    lastUserOperation = MODIFY_CC;
    // Get Current Selection and ensure validity
    int cardChoice {this->ui->ccSelector->currentIndex()};
    if (cardChoice > 0) {
        // Populate Card Name
        ccName = userCards[cardChoice];
        // Get new debt ammount
        ammount = this->ui->ccUpdateAmmountDSP->value();
        // Accept the dialog
        accept();
    }
    else {
        // Show Error Message
        QMessageBox err;
        err.warning(0, "Warning", "Please select a valid card to modify or add a new card.");
        err.setFixedSize(500, 200);
    }
}


void modifyccdialog::onAddCCButtonClicked() {
    // Set last User Operation
    lastUserOperation = ADD_CC;
    // Ensure new card is not already a current card
    QString newCardName {this->ui->newCCLineEdit->text()};
    if (!userCards.contains(newCardName)) {
        // Populate Card Name
        ccName = newCardName;
        // Get ammount of debt
        ammount = this->ui->newCCAmmontDSP->value();
        // Accept Dialog
        accept();
    }
    else {
        // Show Error Message
        QMessageBox err;
        err.warning(0, "Warning", "New card name matches that of an existing card!");
        err.setFixedSize(500, 200);
    }
}

void modifyccdialog::onCancelButtonClicked() {
    if (DEBUG) {
        qDebug() << "[modifyccdialog::onCancelButtonClicked]: CANCEL BUTTON CLICKED!";
    }
    reject();
}
