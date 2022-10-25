#ifndef TRANSACTIONITEMPOINTER_H
#define TRANSACTIONITEMPOINTER_H
#include <QStandardItem>
#include <QString>

class TransactionItemPointer
{
public:
    TransactionItemPointer(QString name, QString date, QString ammt, QStandardItem *nameP, QStandardItem *dateP, QStandardItem *ammtP) : transDesc{name}, transDate{date}, transAmmt{ammt}, transDescP{nameP}, transDateP{dateP}, transAmmtP{ammtP} {};
    QString getDesc() { return transDesc; };
    QString getDate() { return transDate; };
    QString getAmmt() { return transAmmt; };
    QStandardItem* getDescPointer() { return transDescP; };
    QStandardItem* getDatePointer() { return transDateP; };
    QStandardItem* getAmmtPointer() { return transAmmtP; };
    QString checkPointer(QStandardItem *p);

private:
    QString transDesc;
    QString transDate;
    QString transAmmt;
    QStandardItem *transDescP;
    QStandardItem *transDateP;
    QStandardItem *transAmmtP;
};

#endif // TRANSACTIONITEMPOINTER_H
