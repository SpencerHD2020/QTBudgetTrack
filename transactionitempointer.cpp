#include "transactionitempointer.h"


QString TransactionItemPointer::checkPointer(QStandardItem *p) {
    if (p == this->transAmmtP) {
        return "Ammt";
    }
    else if (p == this->transDateP) {
        return "Date";
    }
    else if (p == this->transDescP) {
        return "Desc";
    }
    else {
        return "-1";
    }
}
