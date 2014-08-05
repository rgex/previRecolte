#ifndef EDITMETEODATAFORM_H
#define EDITMETEODATAFORM_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class editMeteoDataForm;
}

class editMeteoDataForm : public QWidget
{
    Q_OBJECT

public:
    explicit editMeteoDataForm(QWidget *parent = 0);
    ~editMeteoDataForm();

private:
    Ui::editMeteoDataForm *ui;
    MainWindow* mainWindow;
};

#endif // EDITMETEODATAFORM_H
