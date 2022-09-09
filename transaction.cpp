#include "transaction.h"


void Transaction::parseDate()
{
    QStringList dateSplit {date.split('/')};
    month = dateSplit.value(0).toInt();
    day = dateSplit.value(1).toInt();
    year = dateSplit.value(2).toInt();
    sortDate = QDate(year, month, day);
}
