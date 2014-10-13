#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "varietedatabaseinterface.h"
#include "sitesdatabaseinterface.h"
#include "meteodatabaseinterface.h"
#include <QItemSelectionModel>

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
    void hideDatePrevisions();
    void showDatePrevisions();
    void hideEditVarieteInputs();
    void showEditVarieteInputs();
    void deleteYearFromSite(int siteId, int year);
    void deleteSite(int siteId);
    Meteo* getMeteo(int siteId,int year);
    void displayEditMeteo(int siteId, int year);
    void updateSiteScrollArea(int siteId, int year,QString yearsCsv);
    void loadSiteScrollArea(int siteId, int year);
    void insertTextInPrevisionsDebugPlainTextEdit(QString text);
    void clearPrevisionsDebugPlainTextEdit();

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
    void on_previsionSiteSelect_currentIndexChanged(int index);
    void on_calculateDateRecolteBtn_clicked();

    void on_actionAide_triggered();

    void on_intervallesSubmitBtn_clicked();

    void on_Accueil_AjouterVariete_BTN_clicked();

    void on_Accueil_ModifierSite_BTN_clicked();

    void on_Accueil_CreerSite_BTN_clicked();

    void on_Accueil_ModifierSite_BTN_2_clicked();

    void on_Accueil_CalculerIntervalle_BTN_clicked();

    void on_Accueil_PrevoirDate_BTN_clicked();

private:
    Ui::MainWindow *ui;
    QString appStoragePath;
    QString imageStoragePath;
    QString dbPath;
    VarieteDatabaseInterface vDbi;
    SitesDatabaseInterface sDbi;
    MeteoDatabaseInterface mDbi;
    QList<QStringList> importTempList;
    QItemSelectionModel* sitesSelectionModel;
    bool sitesSelectionModelIsInitialized;

};

#endif // MAINWINDOW_H
