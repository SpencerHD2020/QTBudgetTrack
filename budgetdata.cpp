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
              "totalcash real, "
              "bills real, "
              "totAftBills real, "
              "ccOwed real, "
              "totAftcc real)");
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
