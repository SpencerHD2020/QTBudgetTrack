#include "budgetdata.h"

BudgetData::BudgetData(QWidget *parent)
{
    QString userName {findHomeFolder()};
    dbPath = dbPath + "/" + userName + "/BudgetDB";
    if (DEBUG) {
        qDebug() << "Set DB Path to: " << dbPath;
    }
    // Check if BudgetDB folder exists
    if (!QDir(dbPath).exists()) {
        // Create Folder
        QDir().mkdir(dbPath);
        if (DEBUG) {
            qDebug() << "Path did not exist! Creating folder now";
        }
    }
    dbPath = dbPath + "/" + dbBaseName;
    if (DEBUG) {
        qDebug() << "Set DB ABS Path to: " << dbPath;
    }
    db.setDatabaseName(dbPath);
    // Check if DB Exists
    if (!fileExists(dbPath)) {
        // Create the DB File
        QFile newFile(dbPath);
        newFile.open( QIODevice::WriteOnly);
        newFile.close();
        if (DEBUG) {
            qDebug() << "DB File did not exist! Creating file now";
        }
        initTables();
    }
    // Check if tables exist
    else if (!dbTablesExist()) {
        if (DEBUG) {
            qDebug() << "DB File did not contain all neccessary tables, erasing contents and reinitializing db.";
        }
        // Clear the file
        eraseDBFile();
        // Write Tables
        initTables();
    }
}

bool BudgetData::compareTDate(const Transaction& i, const Transaction& j) {
    return i.sortDate < j.sortDate;
}


QVector<Transaction> BudgetData::sortTransactions(QVector<Transaction> trans) {
    std::sort(trans.begin(), trans.end(), compareTDate);
    if (DEBUG) {
        qDebug() << "[BudgetData::sortTransactions]: First: " << trans.first().getDate();
        qDebug() << trans.last().getDate();
    }
    return trans;
}


QVector<Transaction> BudgetData::fetchTransactions() {
    // Fetch Transactions from DB
    QSqlQuery q;
    db.open();
    bool transQueried {q.exec("SELECT * FROM TRANS;")};
    if (transQueried) {
        // Init Vector
        QVector<Transaction> unsortedTrans;
        // Define Indices
        QSqlRecord rec {q.record()};
        int n = rec.indexOf("name");
        int a = rec.indexOf("ammt");
        int d = rec.indexOf("date");
        // Loop through DB Result building Transaction Instances
        while (q.next()) {
            unsortedTrans.append(Transaction(q.value(n).toString(), q.value(a).toDouble(), q.value(d).toString()));
        }
        // Sort Vector [newVector]
        QVector<Transaction> sortedTrans {sortTransactions(unsortedTrans)};
        // Clear original Vector

        db.close();
        // Return Sorted Vector
    }
    else {
        db.close();
        emit dbMutationFailed("Failed to fetch Transactions");
        return QVector<Transaction>();
    }
}


void BudgetData::eraseDBFile() {
    //QFile dbFile(dbPath);
    //dbFile.resize(0);
    std::ofstream ofs;
    ofs.open(dbPath.toStdString(), std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}


bool BudgetData::dbTablesExist() {
    bool tablesExist {true};
    const QStringList tables {"TOTALS", "CC", "BILLS", "TRANS"};
    try {
        db.open();
        for (const auto& t : tables) {
            QSqlQuery q;
            const QString tempQuery {"SELECT name FROM sqlite_master WHERE type='table' AND name='" + t + "';"};
            q.exec(tempQuery);
            if (!q.next()) {
                if (DEBUG) {
                    qDebug() << t << " table did not exist in DB!";
                }
                tablesExist = false;
            }
        }
    }
    catch (std::exception e) {
        qDebug() << "ERROR while checking if tables exist: " << e.what();
        return false;
        db.close();
    }
    db.close();
    return tablesExist;
}

void BudgetData::initTables() {
    qDebug() << "Creating DB Tables";
    db.open();
    QSqlQuery query;
    // Create TOTALS Table
    query.exec("create table TOTALS "
              "(id integer primary key, "
              "totalcash real DEFAULT 0.0, "
              "bills real DEFAULT 0.0, "
              "totAftBills real DEFAULT 0.0, "
              "ccOwed real DEFAULT 0.0, "
              "totAftcc real DEFAULT 0.0)");
    // Create CC Table
    query.exec("create table CC"
               "(id integer primary key, "
               "name text, "
               "owed real DEFAULT 0.0)");
    // Create BILLS Table
    query.exec("create table BILLS"
               "(id integer primary key, "
               "name text, "
               "totVal real, "
               "curHeld real)");
    // Create TRANS Table
    query.exec("create table TRANS"
               "(id integer primary key, "
               "name text, "
               "ammt real, "
               "processed integer, "
               "date text)");
    //query.exec("INSERT INTO person (firstname, lastname, age) VALUES (Spencer, Nowlin, 24)");
    db.close();
    qDebug() << "DB Tables Created!";
}

QString BudgetData::findHomeFolder() {
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString homePathCleaned {homePath.first().split(QDir::separator()).last()};
    return homePathCleaned;
}

bool BudgetData::fileExists(QString filePath) {
    QFileInfo check_file(filePath);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

QString BudgetData::formInsertStatement(QString tableName, QStringList cols, QStringList data) {
    QString syn {"INSERT INTO " + tableName + " ("};
    int colIndex {0};
    for (const auto& col : cols) {
        syn = syn + col;
        if (colIndex != (cols.length() - 1)) {
            syn = syn + ", ";
        }
        else {
            syn = syn + ") VALUES (";
        }
        colIndex++;
    }
    int datIndex {0};
    for (const auto& dat : data) {
        syn = syn + dat;
        if (datIndex != (data.length() - 1)) {
            syn = syn + ", ";
        }
        else {
            syn = syn + ");";
        }
        datIndex++;
    }
    if (DEBUG) {
        qDebug() << "[BudgetData::formInsertStatement]: INSERT STATEMENT: ";
        qDebug() << syn;
    }
    return syn;
}


void BudgetData::addTransaction(QString description, double ammt, QString date) {
    // Add ammt to total value
    bool totalsUpdated {updateTotals(TRANS_ADDED, ammt)};
    if (totalsUpdated) {
        // Add Entry to Transactions Table
        db.open();
        QSqlQuery q;
        QStringList colNames {"name", "ammt", "processed", "date"};
        QStringList data {("'" + description + "'"), QString::number(ammt), "1", ("'" + date + "'")};
        QString syn {formInsertStatement("TRANS", colNames, data)};
        bool transAdded {q.exec(syn)};
        if (transAdded) {
            // Emit update complete SIGNAL
            emit transactionsUpdated();
        }
        else {
            // Emit Update Failed SIGNAL
            emit dbMutationFailed("Transaction was NOT added to DB.");
        }
        db.close();
    }
    else {
        // Emit Update Failed SIGNAL
        emit dbMutationFailed("Failed to add transaction to total cash in DB.");
    }
}

bool BudgetData::updateTotals(int changeType, double change) {
    if (changeType == TRANS_ADDED) {
        // Transaction Added

        // Find Current Totals
        db.open();
        QSqlQuery q;
        q.exec("SELECT * FROM TOTALS;");
        // Define Indices
        QSqlRecord rec {q.record()};
        int tc = rec.indexOf("totalcash");
        int b = rec.indexOf("bills");
        int cc = rec.indexOf("ccOwed");

        double totalCash;
        double bills;
        double ccOwed;
        double totAftBills;
        double totAftcc;

        if (q.next()) {
            // There is at least one record
            totalCash = q.value(tc).toDouble();
            bills = q.value(b).toDouble();
            ccOwed = q.value(cc).toDouble();
            // Add New Transaction to total
            totalCash += change;
            // Figure new total with bills taken out
            totAftBills = totalCash - bills;
            // Figure new total with bills and cc taken out
            totAftcc = totAftBills - ccOwed;
            // Clear current table entry

            // NOTE: We should first insert the new values and then if that succeeds delete the line where totalCash does not match calculated
            //          if implemented, ensure value passed is not $0
            q.exec("DELETE FROM TOTALS;");
        }
        else {
            // Table is empty
            totalCash = change;
            bills = 0.0;
            ccOwed = 0.0;
            totAftBills = change;
            totAftcc = change;
        }
        // Insert new totals into table
        /*
        QString values {QString::number(totalCash) + ", " + QString::number(bills) + ", " + QString::number(totAftBills) + ", " +
                    QString::number(ccOwed) + ", " + QString::number(totAftcc)};

        QString syn {QString::fromStdString("(INSERT INTO TOTALS ") + QString::fromStdString("(totalcash, bills, totAftBills, ccOwed, totAftcc) ") +
                    QString::fromStdString("VALUES (") + values + QString::fromStdString("));")};
        */
        QStringList colNames {"totalcash", "bills", "totAftBills", "ccOwed", "totAftcc"};
        QStringList data {QString::number(totalCash), QString::number(bills), QString::number(totAftBills), QString::number(ccOwed), QString::number(totAftcc)};
        QString syn {formInsertStatement("TOTALS", colNames, data)};

        bool valInserted {q.exec(syn)};
        db.close();
        if (valInserted) {
            return true;
        }
        else {
            if (DEBUG) {
                qDebug() << "[BudgetData::updateTotals]: Failed to INSERT values into table";
            }
            return false;
        }
    }
    else if (changeType == CC_UPDATE) {
        // Credit Card Total Updates
        // TODO: Implement this switch!
        return false;
    }
    else {
        return false;
    }
}













