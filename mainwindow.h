#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "varietedatabaseinterface.h"
#include "sitesdatabaseinterface.h"
#include "meteodatabaseinterface.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setAppStoragePath(QString path);
    void setImageStoragePath(QString path);
    void setDbPath(QString path);
    QString getAppStoragePath();
    QString getImageStoragePath();
    QString getDbPath();
    void reloadVarieteListView();
    void initWindow();
    void emptyAjouterUneVarieteFields();
    void refreshSitesTreeView();
    void hideEditVarieteInputs();
    void showEditVarieteInputs();
    void deleteYearFromSite(int siteId, int year);
    void deleteSite(int siteId);

private slots:
    void on_ajouterVarieteBtn_clicked();
    void on_ajouterImageBtn_clicked();
    void on_supprimerImageBtn_clicked();
    void on_varietesTabView_currentChanged(int index);
    void on_actionQuitter_triggered();
    void on_varietesListWidget_itemSelectionChanged();
    void on_editAjouterVarieteBtn_clicked();
    void on_editAjouterImageVarieteBtn_clicked();
    void on_editSupprimerImageVariete_clicked();
    void on_supprimerVarieteBtn_clicked();
    void on_ajouterSiteOpenMeteoFileBtn_clicked();
    void on_ajouterSiteSauvegarderBtn_clicked();
    void on_sitesTabWidget_currentChanged(int index);
    void on_sitesTreeView_entered(const QModelIndex &index);
    void on_sitesTreeView_activated(const QModelIndex &index);
    void on_sitesTreeView_clicked(const QModelIndex &index);
    void on_sitesTreeView_pressed(const QModelIndex &index);
    void on_EditSiteSaveButton_clicked();
    void on_leftTabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QString appStoragePath;
    QString imageStoragePath;
    QString dbPath;
    VarieteDatabaseInterface vDbi;
    SitesDatabaseInterface sDbi;
    MeteoDatabaseInterface mDbi;
    QList<QStringList> importTempList;

};

#endif // MAINWINDOW_H
