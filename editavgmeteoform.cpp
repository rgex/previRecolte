#include "editavgmeteoform.h"
#include "ui_editavgmeteoform.h"

editAvgMeteoForm::editAvgMeteoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editAvgMeteoForm)
{
    ui->setupUi(this);
}

editAvgMeteoForm::~editAvgMeteoForm()
{
    delete ui;
}

void editAvgMeteoForm::setEditSiteNameTextEditText(QString text)
{
    ui->editSiteNameTextEdit->setText(text);
}
