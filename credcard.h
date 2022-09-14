#ifndef CREDCARD_H
#define CREDCARD_H

#include "dbitem.h"
#include <QString>
#include <QStringList>

class CredCard : public DBItem
{
public:
    CredCard(QString cardName, double debt);

};

#endif // CREDCARD_H
