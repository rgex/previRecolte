#include "edityearmeteoform.h"
#include "ui_edityearmeteoform.h"
#include <QFileDialog>
#include <QStandardPaths>
int year;
int siteId;

editYearMeteoForm::editYearMeteoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editYearMeteoForm)
{
    ui->setupUi(this);

    //set the webView background to transparent
    QPalette palette = ui->meteoChartWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    ui->meteoChartWebView->page()->setPalette(palette);
    ui->meteoChartWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
}

editYearMeteoForm::~editYearMeteoForm()
{
    delete ui;
}

void editYearMeteoForm::setWebViewHtml(QString html)
{
    ui->meteoChartWebView->setHtml(html);
}

void editYearMeteoForm::setMainWindowPointer(MainWindow* pointer)
{
    mainWindow = pointer;
}

void editYearMeteoForm::on_editDeleteYearButton_clicked()
{
    this->mainWindow->deleteYearFromSite(siteId,year);
}

void editYearMeteoForm::setYear(int value)
{
    this->year = value;
}

void editYearMeteoForm::setSiteId(int value)
{
    this->siteId = value;
}

void editYearMeteoForm::setEditYearChartTitleLabel(QString text)
{
    ui->editYearChartTitleLabel->setText(text);
}

void editYearMeteoForm::on_exportYearBtn_clicked()
{
    Meteo* meteo = this->mainWindow->getMeteo(siteId,year);
    qDebug() << meteo->exportMeteoAsCsv2();
    QString filename = QFileDialog::getSaveFileName(0, QObject::tr("Enregistrer un fichier"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QObject::tr("Fichier csv (*.csv)"));

    qDebug() << "filename : " << filename;

}
