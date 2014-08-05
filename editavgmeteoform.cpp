#include "editavgmeteoform.h"
#include "ui_editavgmeteoform.h"
#include "mainwindow.h"
#include "htmlchartmaker.h"
#include "site.h"
#include "meteo.h"
#include "sitesdatabaseinterface.h"
#include "meteodatabaseinterface.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

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

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Voulez vous ecraser les données déjà présentes dans la base de donnée?",
                                  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
    {
        //HtmlChartMaker htmlChartMaker;
        //htmlChartMaker.
    }
    else if(reply == QMessageBox::No)
    {

    }
    else if(reply == QMessageBox::Cancel)
    {

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
    qDebug() << "got site" ;
    QStringList years = site->getYears();
    MeteoDatabaseInterface meteoDatabaseInterface;

    QList<QMap<QString, QStringList> > finalTmpAvg;

    foreach(QString year, years)
    {
        Meteo* meteo = meteoDatabaseInterface.getMeteo(siteId, year.toInt());
        finalTmpAvg.append(meteo->getMeteo());
    }


    QString html = htmlChartMaker.generateHtmlChartWithMap(htmlChartMaker.calculateAvgOfTempYears(finalTmpAvg));
    ui->meteoChartWebView->setHtml(html);
}
