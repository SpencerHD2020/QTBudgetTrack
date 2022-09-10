#include "transaction.h"


void Transaction::parseDate()
{
    QStringList dateSplit {date.split('/')};
    month = dateSplit.value(0).toInt();
    day = dateSplit.value(1).toInt();
    year = dateSplit.value(2).toInt();
    sortDate = QDate(year, month, day);
}

QString Transaction::getSAmmt() const {
    QString sAmmt {};
    double tempAmmt {ammount};
    if (ammount < 0) {
        sAmmt = "-$";
        tempAmmt *= -1;
    }
    else {
        sAmmt = "$";
    }
    QString stringA {QString::number(tempAmmt)};
    if (stringA.contains('.')) {
        QStringList ammtSplit {stringA.split(".")};
        sAmmt = sAmmt + ammtSplit[0] + ".";
        if (ammtSplit[1].length() == 1) {
            sAmmt = sAmmt + "0" + ammtSplit[1];
        }
        else {
            sAmmt = sAmmt + ammtSplit[1];
        }
    }
    else {
        sAmmt = sAmmt + stringA + ".00";
    }
    return sAmmt;
}
