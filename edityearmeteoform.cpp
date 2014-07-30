#include "edityearmeteoform.h"
#include "ui_edityearmeteoform.h"

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
