#include "editavgmeteoform.h"
#include "ui_editavgmeteoform.h"

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
