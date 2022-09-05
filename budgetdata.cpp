#include "budgetdata.h"

BudgetData::BudgetData(QWidget *parent)
{
    QString userName {findHomeFolder()};
    dbPath = dbPath + "/" + userName + "BudgetDB";
    // Check if BudgetDB folder exists
    if (!QDir(dbPath).exists()) {
        // Create Folder
        QDir().mkdir(dbPath);
    }
    dbPath = dbPath + "/" + dbBaseName;
    // Check if DB Exists
    if (!fileExists(dbPath)) {
        // Create the DB File
        QFile newFile(dbPath);
        newFile.open( QIODevice::WriteOnly);
        newFile.close();
        initTables();
    }
    // TODO: Check if tables exist
    if (false) {
        // TODO: Clear the file
        initTables();
    }
    else {
        // Set DB Name
        db.setDatabaseName(dbPath);
    }
}

void BudgetData::initTables() {
    qDebug() << "Creating DB Tables";
    db.setDatabaseName(dbPath);
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
    //query.exec("INSERT INTO person (firstname, lastname, age) VALUES (Spencer, Nowlin, 24)");
    db.close();
    qDebug() << "DB Tables Created!";
}

QString BudgetData::findHomeFolder() {
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString homePathCleaned {homePath.first().split(QDir::separator()).last()};
    qDebug() << homePathCleaned;
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
