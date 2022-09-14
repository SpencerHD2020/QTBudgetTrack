#include "dbitem.h"

QString DBItem::getSAmmt() const
{
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
