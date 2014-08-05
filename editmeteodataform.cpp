#include "editmeteodataform.h"
#include "ui_editmeteodataform.h"
#include <QTableWidgetItem>

editMeteoDataForm::editMeteoDataForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editMeteoDataForm)
{
    ui->setupUi(this);

    /*

        Test populate tableView.

    */
    ui->editMeteoTableWidget->setRowCount(10);
    ui->editMeteoTableWidget->setColumnCount(5);

    ui->editMeteoTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("ID")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("date")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("t° moyenne")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("t° minimale")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(QString("t° maximale")));
    ui->editMeteoTableWidget->setItem(2, 3, new QTableWidgetItem(QString("test")));
}

editMeteoDataForm::~editMeteoDataForm()
{
    delete ui;
}
