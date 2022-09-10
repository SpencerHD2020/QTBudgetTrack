#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <QString>
#include <QStringList>
#include <QDate>

class Transaction
{
public:
    Transaction(QString des, double ammt, QString date) : description(des), ammount(ammt), date(date) { parseDate(); };
    QString getDescription() const { return description; };
    double getAmmount() { return ammount; };
    QString getSAmmt() const;
    QString getDate() const { return date; };
    int getMonth() { return month; };
    int getDay() { return day; };
    int getYear() { return year; };
    //const QDate getSortDate() { return sortDate; };
    QDate sortDate;
private:
    QString description;
    double ammount;
    QString date;
    void parseDate();
    int month;
    int day;
    int year;
};

#endif // TRANSACTION_H
