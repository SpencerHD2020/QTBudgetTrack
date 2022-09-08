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
#include <fstream>
#include <QSqlRecord>

class BudgetData : public QObject
{
    Q_OBJECT
public:
    BudgetData(QWidget *parent = nullptr);
    void addTransaction(QString description, double ammt, QString date);
signals:
    void transactionsUpdated();
    void dbMutationFailed(QString err);
private:
    QString path = "/Users/spencernowlin/BudgetDB/budget.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath {"/users"};
    QString dbBaseName {"budget.db"};
    bool DEBUG {true};

    QString findHomeFolder();
    bool fileExists(QString filePath);
    bool dbTablesExist();
    void initTables();
    void eraseDBFile();
    enum Totals_Update_Types {
        TRANS_ADDED,
        CC_UPDATE
    };
    bool updateTotals(int changeType, double change);
    QString formInsertStatement(QString tableName, QStringList cols, QStringList data);

};

#endif // BUDGETDATA_H
