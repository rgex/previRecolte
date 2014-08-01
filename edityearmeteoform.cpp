#include "edityearmeteoform.h"
#include "ui_edityearmeteoform.h"

MainWindow* mainWindow;
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
    mainWindow->deleteYearFromSite(siteId,year);
}

void editYearMeteoForm::setYear(int value)
{
    year = value;
}

void editYearMeteoForm::setSiteId(int value)
{
    siteId = value;
}
