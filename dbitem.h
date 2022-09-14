#ifndef DBITEM_H
#define DBITEM_H

#include <QString>
#include <QStringList>

class DBItem
{
public:
    DBItem() {};
    QString getDescription() const { return description; };
    void setDescription(QString desc) { description = desc; };
    double getAmmount() const { return ammount; };
    void setAmmount(double ammt) { ammount = ammt; };
    QString getSAmmt() const;
private:
    QString description;
    double ammount;
};

#endif // DBITEM_H
