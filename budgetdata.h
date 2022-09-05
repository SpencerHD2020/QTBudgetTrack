#ifndef BUDGETDATA_H
#define BUDGETDATA_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardPaths>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QIODevice>

class BudgetData : public QObject
{
    Q_OBJECT
public:
    BudgetData(QWidget *parent = nullptr);
private:
    QString path = "/Users/spencernowlin/BudgetDB/budget.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath {"/users"};
    QString dbBaseName {"budget.db"};

    QString findHomeFolder();
    bool fileExists(QString filePath);
    void initTables();

};

#endif // BUDGETDATA_H
