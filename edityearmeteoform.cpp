#include "edityearmeteoform.h"
#include "ui_edityearmeteoform.h"

editYearMeteoForm::editYearMeteoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editYearMeteoForm)
{
    ui->setupUi(this);
}

editYearMeteoForm::~editYearMeteoForm()
{
    delete ui;
}
