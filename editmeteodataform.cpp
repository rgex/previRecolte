#include "editmeteodataform.h"
#include "ui_editmeteodataform.h"
#include "meteo.h"
#include "meteodatabaseinterface.h"
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDate>
#include <QMessageBox>

editMeteoDataForm::editMeteoDataForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editMeteoDataForm)
{
    ui->setupUi(this);
}

void editMeteoDataForm::setMainWindowPointer(MainWindow* pointer)
{
    this->mainWindow = pointer;
}

editMeteoDataForm::~editMeteoDataForm()
{
    delete ui;
}

void editMeteoDataForm::setId(int id)
{
    this->siteId = id;
}

void editMeteoDataForm::setYear(int year)
{
    this->year = year;
}

int editMeteoDataForm::getId()
{
    return this->siteId;
}

int editMeteoDataForm::getYear()
{
    return this->year;
}

void editMeteoDataForm::displayTable()
{
    /*
        Test populate tableView.
    */

    MeteoDatabaseInterface* meteoDatabaseInterface = new MeteoDatabaseInterface();
    meteoDatabaseInterface->setStoragePaths("", this->mainWindow->getDbPath());
    Meteo* meteo = meteoDatabaseInterface->getMeteo(this->siteId, this->year);

    QMap<QString, QStringList> meteoMap = meteo->getMeteo();

    if(QDate::isLeapYear(this->year))
        ui->editMeteoTableWidget->setRowCount(366);
    else
        ui->editMeteoTableWidget->setRowCount(365);

    ui->editMeteoTableWidget->setColumnCount(4);

    ui->editMeteoTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Date")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("T° moyenne")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("T° minimale")));
    ui->editMeteoTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("T° maximale")));

    QDateTime date = QDateTime::fromString(QString::number(this->year)+"0101","yyyyMMdd");
    int i = 0;
    while(date.toString("yyyy").toInt() == this->year)
    {
        if(meteoMap.contains(date.toString("yyyyMMdd")) && meteoMap.value(date.toString("yyyyMMdd")).count() >= 4)
        {
            QStringList list = meteoMap.value(date.toString("yyyyMMdd"));
            ui->editMeteoTableWidget->setItem(i, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));
            ui->editMeteoTableWidget->setItem(i, 1, new QTableWidgetItem(list.at(2)));
            ui->editMeteoTableWidget->setItem(i, 2, new QTableWidgetItem(list.at(1)));
            ui->editMeteoTableWidget->setItem(i, 3, new QTableWidgetItem(list.at(3)));
        }
        else
        {
            ui->editMeteoTableWidget->setItem(i, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));
            ui->editMeteoTableWidget->setItem(i, 1, new QTableWidgetItem("0"));
            ui->editMeteoTableWidget->setItem(i, 2, new QTableWidgetItem("0"));
            ui->editMeteoTableWidget->setItem(i, 3, new QTableWidgetItem("0"));
        }

        date = date.addDays(1);
        i++;
    }
}

void editMeteoDataForm::on_saveBtn_clicked()
{
    MeteoDatabaseInterface* meteoDatabaseInterface = new MeteoDatabaseInterface();
    meteoDatabaseInterface->setStoragePaths("", this->mainWindow->getDbPath());
    Meteo* meteo = meteoDatabaseInterface->getMeteo(this->siteId, this->year);

    for(int i = 0; i < ui->editMeteoTableWidget->rowCount(); i++)
    {
        QDate qDate = QDate::fromString(ui->editMeteoTableWidget->item(i, 0)->text(), "yyyy-MM-dd");
        float avgTemp = ui->editMeteoTableWidget->item(i, 1)->text().toFloat();
        float minTemp = ui->editMeteoTableWidget->item(i, 2)->text().toFloat();
        float maxTemp = ui->editMeteoTableWidget->item(i, 3)->text().toFloat();
        meteo->addEntry(qDate.toString("yyyyMMdd"), maxTemp, avgTemp, minTemp, true);
    }
    meteoDatabaseInterface->saveMeteo(meteo);

    QMessageBox msgBox;
    msgBox.information(this, QString("information"), QString("Les modifications ont étés enregistrées."));
}

void editMeteoDataForm::on_cancelBtn_clicked()
{

}
