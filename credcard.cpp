#include "credcard.h"

CredCard::CredCard(QString cardName, double debt) : DBItem()
{
    setAmmount(debt);
    setDescription(cardName);
}
