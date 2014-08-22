#include "editavgmeteoform.h"
#include "ui_editavgmeteoform.h"
#include "mainwindow.h"
#include "htmlchartmaker.h"
#include "site.h"
#include "meteo.h"
#include "sitesdatabaseinterface.h"
#include "meteodatabaseinterface.h"
#include "importcsv.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QTextStream>

editAvgMeteoForm::editAvgMeteoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editAvgMeteoForm)
{
    ui->setupUi(this);

    //set the webView background to transparent
    QPalette palette = ui->meteoChartWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    ui->meteoChartWebView->page()->setPalette(palette);
    ui->meteoChartWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

editAvgMeteoForm::~editAvgMeteoForm()
{
    delete ui;
}

void editAvgMeteoForm::setEditSiteNameTextEditText(QString text)
{
    ui->editSiteNameTextEdit->setText(text);
}

void editAvgMeteoForm::on_editDeleteSiteButton_clicked()
{
    mainWindow->deleteSite(this->siteId);
}

void editAvgMeteoForm::setWebViewHtml(QString html)
{
    ui->meteoChartWebView->setHtml(html);
}

void editAvgMeteoForm::setMainWindowPointer(MainWindow* pointer)
{
    this->mainWindow = pointer;
}

void editAvgMeteoForm::setSiteId(int value)
{
    this->siteId = value;
}

void editAvgMeteoForm::setEditSiteChartTitleLabel(QString text)
{
    ui->EditSiteCharTitleLabel->setText(text);
}

void editAvgMeteoForm::on_importMeteoDataBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Ouvrir un fichier"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QObject::tr("Fichiers csv(*.csv)"));

    if(false == fileName.isEmpty())
    {

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Voulez vous écraser les données déjà présentes dans la base de donnée?",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            ImportCsv importCsv;
            QList<QStringList> importTempList = importCsv.importFile(fileName);

            //update site
            HtmlChartMaker htmlChartMaker;
            SitesDatabaseInterface sitesDatabaseInterface;
            sitesDatabaseInterface.setStoragePaths(QString(""), mainWindow->getDbPath());

            qDebug() << "will search site with ID" << this->siteId;
            Site* site = sitesDatabaseInterface.getSite(this->siteId);

            QStringList yearList = htmlChartMaker.getYearsWithTempData(importTempList);
            foreach(QString year, yearList)
            {
                site->addYear(year.toInt());
            }
            sitesDatabaseInterface.saveSite(site);

            MeteoDatabaseInterface meteoDatabaseInterface;
            meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

            //add meteo
            if(importTempList.count() > 0)
            {
                foreach (QString year, yearList)
                {
                    qDebug() << "year: " << year;
                    Meteo* meteo = meteoDatabaseInterface.getMeteo(this->siteId, year.toInt());
                    meteo->setYear(year.toInt());
                    meteo->addMeteoWithQMaps(htmlChartMaker.calculateMinDayTemp(importTempList),
                                             htmlChartMaker.calculateDayTempAverage(importTempList),
                                             htmlChartMaker.calculateMaxDayTemp(importTempList),
                                             htmlChartMaker.calculateDayPluviometry(importTempList)
                                             );

                    qDebug() << "meteo csv : " << meteo->exportMeteoAsCsv();
                    meteo->setSiteId(this->siteId);
                    meteoDatabaseInterface.saveMeteo(meteo);
                }
                this->generateGraph(this->siteId);
                this->mainWindow->refreshSitesTreeView();
            }
        }
        else if(reply == QMessageBox::No)
        {
            ImportCsv importCsv;
            QList<QStringList> importTempList = importCsv.importFile(fileName);

            //update site
            HtmlChartMaker htmlChartMaker;
            SitesDatabaseInterface sitesDatabaseInterface;
            sitesDatabaseInterface.setStoragePaths(QString(""), mainWindow->getDbPath());

            qDebug() << "will search site with ID" << this->siteId;
            Site* site = sitesDatabaseInterface.getSite(this->siteId);

            QStringList yearList = htmlChartMaker.getYearsWithTempData(importTempList);
            foreach(QString year, yearList)
            {
                site->addYear(year.toInt());
            }
            sitesDatabaseInterface.saveSite(site);

            MeteoDatabaseInterface meteoDatabaseInterface;
            meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

            //add meteo
            if(importTempList.count() > 0)
            {
                foreach (QString year, yearList)
                {
                    qDebug() << "year: " << year;
                    Meteo* meteo = meteoDatabaseInterface.getMeteo(this->siteId, year.toInt());
                    meteo->setYear(year.toInt());
                    meteo->addMeteoWithQMaps(htmlChartMaker.calculateMinDayTemp(importTempList),
                                             htmlChartMaker.calculateDayTempAverage(importTempList),
                                             htmlChartMaker.calculateMaxDayTemp(importTempList),
                                             htmlChartMaker.calculateDayPluviometry(importTempList),
                                             false
                                             );

                    qDebug() << "meteo csv : " << meteo->exportMeteoAsCsv();
                    meteo->setSiteId(this->siteId);
                    meteoDatabaseInterface.saveMeteo(meteo);
                }
                this->generateGraph(this->siteId);
                this->mainWindow->refreshSitesTreeView();
            }
        }
        else if(reply == QMessageBox::Cancel)
        {
            return;
        }
    }
}

void editAvgMeteoForm::generateGraph(int siteId)
{
    //generate graph
    HtmlChartMaker htmlChartMaker;
    SitesDatabaseInterface sitesDatabaseInterface;
    sitesDatabaseInterface.setStoragePaths(QString(""), mainWindow->getDbPath());

    qDebug() << "will search site with ID" << siteId;
    Site* site = sitesDatabaseInterface.getSite(siteId);
    qDebug() << "got site";
    QStringList years = site->getYears();
    MeteoDatabaseInterface meteoDatabaseInterface;
    meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

    QList<QMap<QString, QStringList> > finalTmpAvg;

    foreach(QString year, years)
    {
        qDebug() << "get site with ID :" << siteId << " and with year :" << year.toInt();
        Meteo* meteo = meteoDatabaseInterface.getMeteo(siteId, year.toInt());
        finalTmpAvg.append(meteo->getMeteo());
    }

    qDebug() << "pt1";
    QMap<QString, QStringList> avgOfTempYears = htmlChartMaker.calculateAvgOfTempYears(finalTmpAvg);
    qDebug() << "pt2";
    QString html = htmlChartMaker.generateHtmlChartWithMap(avgOfTempYears, 1, true);
    qDebug() << html;
    ui->meteoChartWebView->setHtml(html);
}

void editAvgMeteoForm::on_editSiteNameTextEdit_textEdited(const QString &arg1)
{
    ui->EditSiteSaveButton->setDisabled(false);
}

void editAvgMeteoForm::on_EditSiteSaveButton_clicked()
{
    SitesDatabaseInterface sitesDatabaseInterface;
    sitesDatabaseInterface.setStoragePaths(QString(""), mainWindow->getDbPath());
    Site* site = sitesDatabaseInterface.getSite(siteId);
    site->setNom(ui->editSiteNameTextEdit->text());
    sitesDatabaseInterface.saveSite(site);
    ui->EditSiteSaveButton->setDisabled(true);

    QMessageBox msgBox;
    msgBox.information(this, QString("information"), QString("Le nom du site a été mis à jour."));
}

void editAvgMeteoForm::on_exportMeteoDataBtn_clicked()
{
    MeteoDatabaseInterface meteoDatabaseInterface;
    meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

    QList<Meteo*> meteoList = meteoDatabaseInterface.getMeteo(this->siteId);

    //@TODO fix default export filename
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/exportmeteo.csv";

    qDebug() << "default path : " << defaultPath;


    QString filename = QFileDialog::getSaveFileName(this, QObject::tr("Enregistrer un fichier"),
                                                    defaultPath,
                                                    QObject::tr("Fichier csv (*.csv)"));
    filename = filename + ".csv";

    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    QString csv;

    if(false == filename.isEmpty())
    {
        qDebug() << "filename not empty";
        qDebug() << "meteoList size : " << meteoList.size();
        qDebug() << "siteId" << this->siteId;

        int i=0;
        foreach(Meteo* meteo, meteoList)
        {
            if(i < 1)
            {
                csv = meteo->exportMeteoAsCsv2(true);
            }
            else
            {
                csv = meteo->exportMeteoAsCsv2(false);
            }
            qDebug() << csv;
            out << csv;
            qDebug() << "filename : " << filename;
            i++;
        }
    }
}
