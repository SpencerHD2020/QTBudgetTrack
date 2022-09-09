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
#include <QVector>
#include <QDate>
#include <algorithm>
#include "transaction.h"

class BudgetData : public QObject
{
    Q_OBJECT
public:
    BudgetData(QWidget *parent = nullptr);
    void addTransaction(QString description, double ammt, QString date);
    QVector<Transaction> fetchTransactions();
    static bool compareTDate(const Transaction& i, const Transaction& j);
signals:
    void transactionsUpdated();
    void dbMutationFailed(QString err);
private:
    QString path = "/Users/spencernowlin/BudgetDB/budget.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath {"/users"};
    QString dbBaseName {"budget.db"};
    bool DEBUG {true};
    enum Totals_Update_Types {
        TRANS_ADDED,
        CC_UPDATE
    };

    QString findHomeFolder();
    bool fileExists(QString filePath);
    bool dbTablesExist();
    void initTables();
    void eraseDBFile();
    bool updateTotals(int changeType, double change);
    QString formInsertStatement(QString tableName, QStringList cols, QStringList data);
    QVector<Transaction> sortTransactions(QVector<Transaction> trans);

};

#endif // BUDGETDATA_H
