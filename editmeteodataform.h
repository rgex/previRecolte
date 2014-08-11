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
    void setMainWindowPointer(MainWindow* pointer);
    void setId(int id);
    void setYear(int year);
    int getId();
    int getYear();
    void displayTable();

private slots:
    void on_saveBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::editMeteoDataForm *ui;
    MainWindow* mainWindow;
    int year;
    int siteId;
};

#endif // EDITMETEODATAFORM_H
