#ifndef MODIFYCCDIALOG_H
#define MODIFYCCDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QStringList>
#include <QMessageBox>
#include "ui_modifyccdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui {
    class modifyccdialog;
}
QT_END_NAMESPACE

class modifyccdialog : public QDialog
{
    Q_OBJECT
public:
    explicit modifyccdialog(QStringList cards, bool debug = false, QWidget *parent = nullptr);
    ~modifyccdialog();
    enum USER_OPERATION {
        MODIFY_CC,
        ADD_CC
    };
    int getLastUserOperation() { return lastUserOperation; };
    QString getCCName() { return ccName; };
    double getAmmt() { return ammount; };
private:
    Ui::modifyccdialog *ui;
    QStringList userCards {"No Selection"};
    bool DEBUG {false};
    int lastUserOperation {MODIFY_CC};
    QString ccName {};
    double ammount {0.0};
private slots:
    void onChangeCCButtonClicked();
    void onAddCCButtonClicked();
    void onCancelButtonClicked();
};

#endif // MODIFYCCDIALOG_H
