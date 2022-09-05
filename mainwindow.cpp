#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Test DB Creation
    BudgetData *db = new BudgetData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

