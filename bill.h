#ifndef BILL_H
#define BILL_H
#include <QString>
#include <QStringList>

class Bill
{
public:
    Bill(QString billName, double totalVal, double curHeld=0.0) : name(billName), total(totalVal), held(curHeld) {};
    QString getName() const { return name; };
    double getTotal() const { return total; };
    double getHeld() const { return held; };
    QString getSAmmt() const;
private:
    QString name;
    double total;
    double held;
};

#endif // BILL_H
