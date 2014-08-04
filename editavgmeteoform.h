#ifndef EDITAVGMETEOFORM_H
#define EDITAVGMETEOFORM_H

#include <QWidget>
#include <QString>
#include "mainwindow.h"

namespace Ui {
class editAvgMeteoForm;
}

class editAvgMeteoForm : public QWidget
{
    Q_OBJECT

public:
    explicit editAvgMeteoForm(QWidget *parent = 0);
    ~editAvgMeteoForm();
    void setEditSiteNameTextEditText(QString text);
    void setWebViewHtml(QString html);
    void setMainWindowPointer(MainWindow* pointer);
    void setSiteId(int value);
    void setEditSiteChartTitleLabel(QString text);

private slots:
    void on_editDeleteSiteButton_clicked();

private:
    Ui::editAvgMeteoForm *ui;
    MainWindow* mainWindow;
    int siteId;
};

#endif // EDITAVGMETEOFORM_H
