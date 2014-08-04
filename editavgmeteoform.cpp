#include "editavgmeteoform.h"
#include "ui_editavgmeteoform.h"
#include "mainwindow.h"


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
