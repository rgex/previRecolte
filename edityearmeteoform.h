#ifndef EDITYEARMETEOFORM_H
#define EDITYEARMETEOFORM_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class editYearMeteoForm;
}

class editYearMeteoForm : public QWidget
{
    Q_OBJECT

public:
    explicit editYearMeteoForm(QWidget *parent = 0);
    ~editYearMeteoForm();
    void setWebViewHtml(QString html);
    void setMainWindowPointer(MainWindow* pointer);
    void setYear(int value);
    void setSiteId(int value);
    void setEditYearChartTitleLabel(QString text);

private slots:
    void on_editDeleteYearButton_clicked();

    void on_exportYearBtn_clicked();

    void on_editYearDataBtn_clicked();

private:
    Ui::editYearMeteoForm *ui;
    MainWindow* mainWindow;
    int year;
    int siteId;
};

#endif // EDITYEARMETEOFORM_H
