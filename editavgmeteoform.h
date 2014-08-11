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
    void generateGraph(int siteId);

private slots:
    void on_editDeleteSiteButton_clicked();
    void on_importMeteoDataBtn_clicked();
    void on_editSiteNameTextEdit_textEdited(const QString &arg1);
    void on_EditSiteSaveButton_clicked();
    void on_exportMeteoDataBtn_clicked();

private:
    Ui::editAvgMeteoForm *ui;
    MainWindow* mainWindow;
    int siteId;
};

#endif // EDITAVGMETEOFORM_H
