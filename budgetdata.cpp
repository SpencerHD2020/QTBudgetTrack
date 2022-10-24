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

QString BudgetData::formatDoubleAsMoneyString(double val) {
    QString sAmmt {};
    if (val < 0) {
        sAmmt = "-$";
        val *= -1;
    }
    else {
        sAmmt = "$";
    }
    QString stringA {QString::number(val)};
    if (stringA.contains('.')) {
        QStringList ammtSplit {stringA.split(".")};
        sAmmt = sAmmt + ammtSplit[0] + ".";
        if (ammtSplit[1].length() == 1) {
            sAmmt = sAmmt + "0" + ammtSplit[1];
        }
        else {
            sAmmt = sAmmt + ammtSplit[1];
        }
    }
    else {
        sAmmt = sAmmt + stringA + ".00";
    }
    return sAmmt;
}


QStringList BudgetData::fetchTotals() {
    QSqlQuery q;
    db.open();
    bool initQuery {q.exec("SELECT * FROM TOTALS;")};
    if (initQuery) {
        // Define Indices
        QSqlRecord rec {q.record()};
        int tc = rec.indexOf("totalcash");
        int tac = rec.indexOf("totAftcc");
        bool innerQuery {q.next()};
        if (innerQuery) {
            double totalCash {q.value(tc).toDouble()};
            double totalFixed {q.value(tac).toDouble()};
            QStringList values;
            values.append(formatDoubleAsMoneyString(totalCash));
            values.append(formatDoubleAsMoneyString(totalFixed));
            db.close();
            return values;
        }
        else {
            emit dbMutationFailed("DB ERROR: No total values retained!");
            QStringList values {"$0", "$0"};
            db.close();
            return values;
        }
    }
    else {
        emit dbMutationFailed("Failed to Query DB for new totals.");
        QStringList values {"$0", "$0"};
        db.close();
        return values;
    }
}



QStringList BudgetData::fetchBillNames() {
    QSqlQuery q;
    db.open();
    bool billsQueried {q.exec("SELECT * FROM BILLS;")};
    if (billsQueried) {
        QStringList billNames {};
        QSqlRecord rec {q.record()};
        int n = rec.indexOf("name");
        while (q.next()) {
            billNames.append(q.value(n).toString());
        }
        db.close();
        return billNames;
    }
    else {
        db.close();
        emit dbMutationFailed("FAILED to fetch bills from database!");
        return QStringList();
    }
}


QStringList BudgetData::fetchCCNames() {
    QSqlQuery q;
    db.open();
    bool cardsQueried {q.exec("SELECT * FROM CC;")};
    if (cardsQueried) {
        QStringList cardNames {};
        QSqlRecord rec {q.record()};
        int n = rec.indexOf("name");
        while (q.next()) {
            cardNames.append(q.value(n).toString());
        }
        db.close();
        return cardNames;
    }
    else {
        db.close();
        emit dbMutationFailed("FAILED to fetch credit cards from database!");
        return QStringList();
    }
}

void BudgetData::addCC(QString cardName, double ammt) {
    QSqlQuery q;
    db.open();
    QStringList colNames {"name", "owed"};
    QStringList data {("'" + cardName + "'"), QString::number(ammt)};
    QString syn {formInsertStatement("CC", colNames, data)};
    if (q.exec(syn)) {
        // Recalculate TOTALS
        bool totalsUpdated {updateTotals(CC_UPDATE, ammt)};
        db.close();
        if (totalsUpdated) {
            emit ccUpdated();
        }
        else {
            emit dbMutationFailed("Failed to update totals in db");
        }
    }
    else {
        emit dbMutationFailed("FAILED to Add CC to database");
        db.close();
    }
}

void BudgetData::addBill(QString billName, double ammt) {
    QSqlQuery q;
    db.open();
    // NOTE: Need to update totals to reflect new BILL FIRST!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ANOTHER NOTE: For all bill updates should we just add to db first and then recalculate everything instead of having all the switches?
    //          at least for bills and CC, transactions would take too long

    QStringList colNames {"name", "totVal", "curHeld"};
    QStringList data {("'" + billName + "'"), QString::number(ammt), "0"};
    QString syn {formInsertStatement("BILLS", colNames, data)};
    if (q.exec(syn)) {
        // Recalculate TOTALS
        bool totalsUpdated {updateTotals(BILLS_UPDATE, ammt)};
        if (totalsUpdated) {
            db.close();
            emit billsUpdated();
        }
        else {
            db.close();
            emit dbMutationFailed("FAILED to update totals in database!");
        }
    }
    else {
        emit dbMutationFailed("FAILED to Add bill to database!");
        db.close();
    }
}

void BudgetData::modifyBill(QString billName, double ammt) {
    // Find Record
    db.open();
    QSqlQuery q;
    // Update based on name
    QString syntax {"UPDATE BILLS set totVal = " + QString::number(ammt) + " where name = '" + billName + "';"};
    bool billUpdated {q.exec(syntax)};
    db.close();
    if (billUpdated) {
        emit billsUpdated();
    }
    else {
        if (DEBUG) {
            qDebug() << "[BudgetData::modifyBill]: Failed to update bill (" << billName << ") to ammount (" << ammt << ")";
        }
        emit dbMutationFailed("FAILED to update bill value!");
    }
}


QVector<Transaction> BudgetData::fetchTransactions() {
    // NOTE: EVENTUALLY we will have to get only so many results and then paginate this
    // NOTE: We should save this Vector and make a check to this function that would just add the new transaction and find its place in the list!

    // Fetch Transactions from DB
    QSqlQuery q;
    db.open();
    bool transQueried {q.exec("SELECT * FROM TRANS;")};
    if (transQueried) {
        // Init Vector - NOTE: THIS SHOULD go on the heap at some point!
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
        // Need to ensure we have transactions
        if (unsortedTrans.length() > 0) {
            // Sort Vector [newVector]
            QVector<Transaction> sortedTrans {sortTransactions(unsortedTrans)};
            // Clear original Vector

            db.close();
            // Return Sorted Vector
            return sortedTrans;
        }
        else {
            db.close();
            return QVector<Transaction>();
        }
    }
    else {
        db.close();
        emit dbMutationFailed("Failed to fetch Transactions");
        return QVector<Transaction>();
    }
}


QVector<Bill> BudgetData::fetchBills() {
    QSqlQuery q;
    db.open();
    bool billsQueried {q.exec("SELECT * FROM BILLS;")};
    if (billsQueried) {
        // Init Vector
        QVector<Bill> bills;
        // Define Indices
        QSqlRecord rec {q.record()};
        int n {rec.indexOf("name")};
        int t {rec.indexOf("totVal")};
        while (q.next()) {
            bills.append(Bill(q.value(n).toString(), q.value(t).toDouble()));
        }
        db.close();
        if (bills.length() > 0) {
            return bills;
        }
        else {
            return QVector<Bill>();
        }
    }
    else {
        db.close();
        emit dbMutationFailed("Failed to fetch Bills");
        return QVector<Bill>();
    }
}

QVector<CredCard> BudgetData::fetchCCData() {
    QSqlQuery q;
    db.open();
    bool cardsQueried {q.exec("SELECT * FROM CC;")};
    if (cardsQueried) {
        // Init Vector
        QVector<CredCard> cards;
        // Define Indices
        QSqlRecord rec {q.record()};
        int n {rec.indexOf("name")};
        int o {rec.indexOf("owed")};
        while (q.next()) {
            cards.append(CredCard(q.value(n).toString(), q.value(o).toDouble()));
        }
        db.close();
        if (cards.length() > 0) {
            return cards;
        }
        else {
            return QVector<CredCard>();
        }
    }
    else {
        db.close();
        emit dbMutationFailed("Failed to fetch credit card data.");
        return QVector<CredCard>();
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
    else if (changeType == BILLS_UPDATE) {
        // Bills Total Updates
        // IMPORTANT! For the time being I am not implementing Bill hold values but this logic will need to heavily change once that is implemented!
        db.open();
        QSqlQuery q;

        // Calculate new BILLS total from BILLS Table
        double totalBills {0.0};
        q.exec("SELECT * FROM BILLS;");
        // Define indices
        QSqlRecord billsRec {q.record()};
        int tot {billsRec.indexOf("totVal")};
        while (q.next()) {
            totalBills += q.value(tot).toDouble();
        }
        // Need to recalculate all downstream totals in case the bill was changed not added
        q.exec("SELECT * FROM TOTALS;");
        // Define Indices
        QSqlRecord rec {q.record()};
        int tc = rec.indexOf("totalcash");
        int cc = rec.indexOf("ccOwed");

        double totalCash;
        double ccOwed;
        double totAftBills;
        double totAftcc;
        if (q.next()) {
            // There is at least one record
            totalCash = q.value(tc).toDouble();
            ccOwed = q.value(cc).toDouble();
            // Figure new total with bills taken out
            totAftBills = totalCash - totalBills;
            // Figure new total with bills and cc taken out
            totAftcc = totAftBills - ccOwed;
            // Clear current table entry

            // NOTE: We should first insert the new values and then if that succeeds delete the line where totalCash does not match calculated
            //          if implemented, ensure value passed is not $0
            q.exec("DELETE FROM TOTALS;");
        }
        else {
            // Nothing in Table
            totalCash = 0.0;
            ccOwed = 0.0;
            totAftBills = totalCash - totalBills;
            totAftcc = totAftBills;
        }

        // Update in TOTALS table
        QStringList colNames {"totalcash", "bills", "totAftBills", "ccOwed", "totAftcc"};
        QStringList data {QString::number(totalCash), QString::number(totalBills), QString::number(totAftBills), QString::number(ccOwed), QString::number(totAftcc)};
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
        // CC Total Updates
        // IMPORTANT! For the time being I am not implementing Bill hold values but this logic will need to heavily change once that is implemented!
        db.open();
        QSqlQuery q;

        // Calculate new CC total from CC Table
        double totalDebt {0.0};
        q.exec("SELECT * FROM CC;");
        // Define indices
        QSqlRecord ccRec {q.record()};
        int owe {ccRec.indexOf("owed")};
        while (q.next()) {
            totalDebt += q.value(owe).toDouble();
        }
        // Need to recalculate all downstream totals in case the bill was changed not added
        q.exec("SELECT * FROM TOTALS;");
        // Define Indices
        QSqlRecord rec {q.record()};
        int tc = rec.indexOf("totalcash");
        int bill = rec.indexOf("bills");

        double totalCash;
        double bills;
        double totAftBills;
        double totAftcc;
        if (q.next()) {
            // There is at least one record
            totalCash = q.value(tc).toDouble();
            bills = q.value(bill).toDouble();
            // Figure new total with bills taken out
            totAftBills = totalCash - bills;
            // Figure new total with bills and cc taken out
            totAftcc = totAftBills - totalDebt;
            // Clear current table entry

            // NOTE: We should first insert the new values and then if that succeeds delete the line where totalCash does not match calculated
            //          if implemented, ensure value passed is not $0
            q.exec("DELETE FROM TOTALS;");
        }
        else {
            // Nothing in Table
            totalCash = 0.0;
            bills = 0.0;
            totAftBills = 0.0;
            totAftcc = totalCash - totalDebt;
        }

        // Update in TOTALS table
        QStringList colNames {"totalcash", "bills", "totAftBills", "ccOwed", "totAftcc"};
        QStringList data {QString::number(totalCash), QString::number(bills), QString::number(totAftBills), QString::number(totalDebt), QString::number(totAftcc)};
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
    else {
        return false;
    }
}













