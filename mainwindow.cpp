#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "varietesananas.h"
#include <QMessageBox>
#include <QUrl>
#include <QStringList>
#include <QList>
#include <QStandardPaths>
#include "htmlchartmaker.h"
#include "site.h"
#include "sitesdatabaseinterface.h"
#include <QTreeView>
#include <QAbstractListModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include "meteo.h"
#include "meteodatabaseinterface.h"
#include "editavgmeteoform.h"
#include "edityearmeteoform.h"
#include "editmeteodataform.h"
#include "importcsv.h"
#include "helpform.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->supprimerImageBtn->setHidden(true);
    this->hideDatePrevisions();
    this->hideEditVarieteInputs();
    this->sitesSelectionModelIsInitialized = false;

    //set the webView background to transparent
    QPalette palette = ui->ajouterSiteWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    ui->ajouterSiteWebView->page()->setPalette(palette);
    ui->ajouterSiteWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
    ui->ajouterSiteWebView->setHtml("<br/><br/><br/><br/><br/><br/> <center>Ajoutez un fichier avec des données météo pour faire apparaitre le graphique</center>");

    QPalette palette2 = ui->IntervalleWebView->palette();
    palette2.setBrush(QPalette::Base, Qt::transparent);
    ui->IntervalleWebView->page()->setPalette(palette2);
    ui->IntervalleWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);
    ui->IntervalleWebView->setHtml("");

    ui->previsionsDateEdit->setDate(QDate::currentDate());
    ui->intervalleProgressBar->setHidden(true);
    ui->previsionsDebugPlainTextEdit->setHidden(true);


    ui->dateTifLabel->setHidden(true);
    ui->dateTifInputLabel->setHidden(true);
}

void MainWindow::initWindow()
{
    this->vDbi.setStoragePaths(this->appStoragePath, this->imageStoragePath, this->dbPath);
    this->sDbi.setStoragePaths(this->appStoragePath, this->dbPath);
    this->mDbi.setStoragePaths(this->appStoragePath, this->dbPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ajouterVarieteBtn_clicked()
{
    if(ui->varieteNomInput->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez entrer le nom de la variété"));
    }
    else
    {
        VarietesAnanas* variete = new VarietesAnanas(appStoragePath, imageStoragePath);
        variete->setImagePath(ui->imagePathLabel->text());
        variete->setNom(ui->varieteNomInput->text());
        variete->setTBase1(ui->varieteTBase1Input->value());
        variete->setTFloraison(ui->varieteTFloraisonInput->value());
        variete->setTBase2(ui->varieteTBase2Input->value());
        variete->setTRecolte(ui->varieteTRecolteInput->value());
        variete->saveImage();

        this->vDbi.saveVariete(variete);

        QMessageBox msgBox;
        msgBox.information(this, QString("information"), QString("La variété a été ajoutée à la base de donnée."));
        this->emptyAjouterUneVarieteFields();
    }
}

void MainWindow::on_ajouterImageBtn_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Ouvrir un fichier"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QObject::tr("Fichiers image(*.jpg *.gif *png)"));
    qDebug() << "fileName is : " << fileName;

    if(false == fileName.isEmpty())
    {
        QPixmap pixMap = QPixmap(fileName);
        float imageRatio = (float)pixMap.width() / (float)pixMap.height() ;
        qDebug() << "image ratio : " << imageRatio;
        float newWidth = imageRatio * ui->ajouterImageLabel->height();
        qDebug() << "new ajouterImageLabel width is : " << (int)newWidth;
        ui->ajouterImageLabel->setFixedWidth((int)newWidth);
        ui->ajouterImageLabel->setPixmap(pixMap);
        ui->imagePathLabel->setText(fileName);
        ui->ajouterImageBtn->setText("Modifier l'image");
        ui->supprimerImageBtn->setVisible(true);
    }
}

void MainWindow::on_supprimerImageBtn_clicked()
{
    ui->imagePathLabel->setText("");
    QPixmap pixMap = QPixmap("");
    ui->ajouterImageLabel->setPixmap(pixMap);
    ui->supprimerImageBtn->setVisible(false);
    ui->ajouterImageBtn->setText("Ajouter une image");
}

void MainWindow::on_varietesTabView_currentChanged(int index)
{
    qDebug() << "new variete tab index is : " << index;
    if(index == 1)
    {
        this->reloadVarieteListView();
    }
}

void MainWindow::hideDatePrevisions()
{
    ui->dateFloraisonLabel->setHidden(true);
    ui->dateFloraisonLabelInput->setHidden(true);
    ui->dateRecolteLabel->setHidden(true);
    ui->dateRecolteLabelInput->setHidden(true);
}

void MainWindow::showDatePrevisions()
{
    ui->dateFloraisonLabel->setHidden(false);
    ui->dateFloraisonLabelInput->setHidden(false);
    ui->dateRecolteLabel->setHidden(false);
    ui->dateRecolteLabelInput->setHidden(false);
}

void MainWindow::hideEditVarieteInputs()
{
    ui->editAjouterImageLabel->setHidden(true);
    ui->editAjouterImageVarieteBtn->setHidden(true);
    ui->editAjouterVarieteBtn->setHidden(true);
    ui->editImagePathLabel->setHidden(true);
    ui->editNomLabel->setHidden(true);
    ui->editSupprimerImageVariete->setHidden(true);
    ui->editTBase1Label->setHidden(true);
    ui->editTBase2Label->setHidden(true);
    ui->editTFloraisonLabel->setHidden(true);
    ui->editTRecolteLabel->setHidden(true);
    ui->editVarieteNomInput->setHidden(true);
    ui->editVarieteTBase1Input->setHidden(true);
    ui->editVarieteTBase2Input->setHidden(true);
    ui->editVarieteTFloraisonInput->setHidden(true);
    ui->editVarieteTRecolteInput->setHidden(true);
    ui->supprimerVarieteBtn->setHidden(true);

    ui->editVarieteKeyLabel->setHidden(true); //hidden for ever
    ui->editVarieteNewImageNameLabel->setHidden(true); //hidden for ever
}

void MainWindow::showEditVarieteInputs()
{
    ui->editAjouterImageLabel->setHidden(false);
    ui->editAjouterImageVarieteBtn->setHidden(false);
    ui->editAjouterVarieteBtn->setHidden(false);
    ui->editImagePathLabel->setHidden(false);
    ui->editNomLabel->setHidden(false);
    ui->editSupprimerImageVariete->setHidden(false);
    ui->editTBase1Label->setHidden(false);
    ui->editTBase2Label->setHidden(false);
    ui->editTFloraisonLabel->setHidden(false);
    ui->editTRecolteLabel->setHidden(false);
    ui->editVarieteNomInput->setHidden(false);
    ui->editVarieteTBase1Input->setHidden(false);
    ui->editVarieteTBase2Input->setHidden(false);
    ui->editVarieteTFloraisonInput->setHidden(false);
    ui->editVarieteTRecolteInput->setHidden(false);
    ui->supprimerVarieteBtn->setHidden(false);
}


/*
 *
 * Getters and setters
 *
 */

void MainWindow::setAppStoragePath(QString path)
{
    this->appStoragePath = path;
}

void MainWindow::setImageStoragePath(QString path)
{
    this->imageStoragePath = path;
}

void MainWindow::setDbPath(QString path)
{
    this->dbPath = path;
}

QString MainWindow::getAppStoragePath()
{
    return this->appStoragePath;
}

QString MainWindow::getImageStoragePath()
{
    return this->imageStoragePath;
}

QString MainWindow::getDbPath()
{
    return this->dbPath;
}

void MainWindow::on_actionQuitter_triggered()
{
    QApplication::quit();
}

void MainWindow::on_varietesListWidget_itemSelectionChanged()
{
    this->showEditVarieteInputs();
    QString key = ui->varietesListWidget->currentItem()->data(10000).toString();

    VarietesAnanas* variete = vDbi.getVarieteWithId(key.toInt());

    ui->editVarieteKeyLabel->setText(key);
    ui->editVarieteNomInput->setText(variete->getNom());
    ui->editVarieteTBase1Input->setValue(variete->getTBase1());
    ui->editVarieteTBase2Input->setValue(variete->getTBase2());
    ui->editVarieteTFloraisonInput->setValue(variete->getTFloraison());
    ui->editVarieteTRecolteInput->setValue(variete->getTRecolte());
    ui->editVarieteNewImageNameLabel->setText(variete->getNewImageName());
    ui->editImagePathLabel->setText("");

    QString imageName = variete->getNewImageName();

    QString imagePath = imageStoragePath + "/" + imageName + ".img";
    if(false == imageName.isEmpty() && QFile(imagePath).exists())
    {
        QPixmap pixMap = QPixmap(imagePath);
        float imageRatio = (float)pixMap.width() / (float)pixMap.height() ;
        qDebug() << "image ratio : " << imageRatio;
        float newWidth = imageRatio * ui->editAjouterImageLabel->height();
        qDebug() << "new ajouterImageLabel width is : " << (int)newWidth;
        ui->editAjouterImageLabel->setFixedWidth((int)newWidth);
        ui->editAjouterImageLabel->setPixmap(pixMap);
        ui->editAjouterImageVarieteBtn->setText("Modifier l'image");
        ui->editSupprimerImageVariete->setVisible(true);
    }
    else
    {
        QPixmap pixMap = QPixmap("");
        ui->editAjouterImageLabel->setPixmap(pixMap);
        ui->editAjouterImageVarieteBtn->setText("Ajouter une image");
        ui->editSupprimerImageVariete->setVisible(false);
    }
}

void MainWindow::on_editAjouterVarieteBtn_clicked()
{
    if(ui->editVarieteNomInput->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez entrer le nom de la variété"));
    }
    else
    {
        QString key = ui->editVarieteKeyLabel->text();
        VarietesAnanas* variete = new VarietesAnanas(appStoragePath, imageStoragePath);
        variete->setNom(ui->editVarieteNomInput->text());
        variete->setTBase1(ui->editVarieteTBase1Input->value());
        variete->setTFloraison(ui->editVarieteTFloraisonInput->value());
        variete->setTBase2(ui->editVarieteTBase2Input->value());
        variete->setTRecolte(ui->editVarieteTRecolteInput->value());
        variete->setId(key.toInt());
        variete->setNewImageName(ui->editVarieteNewImageNameLabel->text());

        if(ui->editImagePathLabel->text().length() > 1)
        {
            variete->setImagePath(ui->editImagePathLabel->text());
            variete->saveImage(key);
        }
        vDbi.saveVariete(variete);

        QMessageBox msgBox;
        msgBox.information(this, QString("information"), QString("Les informations ont été enregistrés dans la base de données."));
        this->reloadVarieteListView();
    }
}

void MainWindow::reloadVarieteListView()
{
    //clear varietesListWidget
    ui->varietesListWidget->clear();

    //reload varietesTabView
    QList<VarietesAnanas*> varieteList = this->vDbi.getAllvarietes();

    //get current selected if there is one.
    int currentSelectedItem = 0;
    if(false == ui->editVarieteKeyLabel->text().isEmpty())
        currentSelectedItem = ui->editVarieteKeyLabel->text().toInt();

    foreach(VarietesAnanas *variete, varieteList)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(variete->getNom());
        item->setData(10000, QString::number(variete->getId()));
        qDebug() << "Saved data : " << item->data(10000);
        qDebug() << "Variete nom : " << variete->getNom();
        ui->varietesListWidget->addItem(item);
        if(currentSelectedItem == variete->getId())
            ui->varietesListWidget->setCurrentItem(item);
    }
}

void MainWindow::on_editAjouterImageVarieteBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Ouvrir un fichier"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QObject::tr("Fichiers image(*.jpg *.gif *png)"));
    qDebug() << "fileName is : " << fileName;

    if(false == fileName.isEmpty())
    {
        QPixmap pixMap = QPixmap(fileName);
        float imageRatio = (float)pixMap.width() / (float)pixMap.height();
        qDebug() << "image ratio : " << imageRatio;
        float newWidth = imageRatio * ui->editAjouterImageLabel->height();
        qDebug() << "new ajouterImageLabel width is : " << (int)newWidth;
        ui->editAjouterImageLabel->setFixedWidth((int)newWidth);
        ui->editAjouterImageLabel->setPixmap(pixMap);
        ui->editImagePathLabel->setText(fileName);
        ui->editAjouterImageVarieteBtn->setText("Modifier l'image");
        ui->editSupprimerImageVariete->setVisible(true);
    }
}

void MainWindow::on_editSupprimerImageVariete_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cette image?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QString imagePath = imageStoragePath + "/" + ui->editVarieteNewImageNameLabel->text() + ".img";

        if(true == QFile(imagePath).exists())
        {
            QFile(imagePath).remove();
        }

        ui->editVarieteNewImageNameLabel->setText("");
        QPixmap pixMap = QPixmap("");
        ui->editAjouterImageLabel->setPixmap(pixMap);
        ui->editAjouterImageVarieteBtn->setText("Ajouter une image");
        ui->editSupprimerImageVariete->setHidden(true);

    }
}

void MainWindow::emptyAjouterUneVarieteFields()
{
    QPixmap pixMap = QPixmap("");
    ui->ajouterImageLabel->setPixmap(pixMap);
    ui->imagePathLabel->setText("");
    ui->varieteNomInput->setText("");
    ui->varieteTBase1Input->setValue(0);
    ui->varieteTFloraisonInput->setValue(0);
    ui->varieteTBase2Input->setValue(0);
    ui->varieteTRecolteInput->setValue(0);
    ui->ajouterImageBtn->setText("Ajouter une image");
    ui->supprimerImageBtn->setHidden(true);
}

void MainWindow::on_supprimerVarieteBtn_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cette variété?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        this->vDbi.deleteVariete(ui->editVarieteKeyLabel->text().toInt());
        this->reloadVarieteListView();
        this->hideEditVarieteInputs();
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}

void MainWindow::on_ajouterSiteOpenMeteoFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Ouvrir un fichier"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QObject::tr("Fichiers csv(*.csv)"));
    ui->ajouterSiteMeteoFilePath->setText(fileName);
    if(false == fileName.isEmpty())
    {
        QString html = "";
        ImportCsv importCsv;
        this->importTempList = importCsv.importFile(fileName);

        HtmlChartMaker htmlChartMaker;
        html = htmlChartMaker.generateHtmlChartWithTempData(this->importTempList);
        ui->ajouterSiteWebView->setZoomFactor(1);
        ui->ajouterSiteWebView->setHtml(html);
    }
}

void MainWindow::on_ajouterSiteSauvegarderBtn_clicked()
{
    HtmlChartMaker htmlChartMaker;
    if(ui->ajouterSiteNomInput->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez entrer le nom du site"));
    }
    else
    {
        //add site
        HtmlChartMaker htmlChartMaker;
        QStringList yearList = htmlChartMaker.getYearsWithTempData(this->importTempList);
        Site* site = new Site();
        site->setNom(ui->ajouterSiteNomInput->text());
        site->setYears(yearList);
        this->sDbi.saveSite(site);

        //add meteo
        if(this->importTempList.count() > 0)
        {

            foreach (QString year, yearList)
            {
                qDebug() << "year: " << year;
                Meteo* meteo = new Meteo();
                meteo->setYear(year.toInt());
                meteo->addMeteoWithQMaps(htmlChartMaker.calculateMinDayTemp(this->importTempList),
                                         htmlChartMaker.calculateDayTempAverage(this->importTempList),
                                         htmlChartMaker.calculateMaxDayTemp(this->importTempList),
                                         htmlChartMaker.calculateDayPluviometry(this->importTempList)
                                         );

                qDebug() << "meteo csv : " << meteo->exportMeteoAsCsv();
                meteo->setSiteId(this->sDbi.lastInsertedRowId());
                this->mDbi.saveMeteo(meteo);
            }
        }

        QList<QStringList> emptyTempList;
        this->importTempList = emptyTempList;

        QMessageBox msgBox;
        msgBox.warning(this, QString("Succès"), QString("Le site a été ajouté à la base de données."));
        ui->ajouterSiteNomInput->setText("");
        ui->ajouterSiteMeteoFilePath->setText("");
        ui->ajouterSiteWebView->setHtml("<br/><br/><br/><br/><br/><br/> <center>Ajoutez un fichier avec des données météo pour faire apparaitre le graphique</center>");
    }
}

void MainWindow::refreshSitesTreeView()
{
    QStandardItemModel* model = new QStandardItemModel;
    QList<Site*> siteList =  this->sDbi.getAllSites();

    QStringList headerList;
    headerList.append("Sites");
    model->setHorizontalHeaderLabels(headerList);

    foreach(Site *site, siteList)
    {
        QStandardItem* item = new QStandardItem(site->getNom());
        QStringList yearList = site->getYears();
        item->setData(QString::number(site->getId()),10000); //set key
        item->setData(0,10001); //set year (0) is equivalent for no year
        item->setData(site->getYearsCsv(),10002); //set years CSV

        foreach (QString year, yearList)
        {
            if(0 != year.toInt())
            {
                QStandardItem* subItem = new QStandardItem(year);
                subItem->setFlags(item->flags() & ~Qt::ItemIsEditable); //set Item as being not editable
                subItem->setData(QString::number(site->getId()),10000); //set key
                subItem->setData(year,10001); //set year
                item->appendRow(subItem);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); //set Item as being not editable
            }
        }
        model->appendRow(item);
    }
    if(false == this->sitesSelectionModelIsInitialized)
    {
        /*
        this->sitesSelectionModel = new QItemSelectionModel(model);
        this->sitesSelectionModel->selectedRows()
        this->sitesSelectionModelIsInitialized = true;
        */

        ui->sitesTreeView->setModel(model);
        //ui->sitesTreeView->setSelectionModel(this->sitesSelectionModel);
    }
    else
    {
        this->sitesSelectionModel = ui->sitesTreeView->selectionModel();
    }

    //ui->sitesTreeView->setSelectionModel();
}

void MainWindow::on_sitesTabWidget_currentChanged(int index)
{
    qDebug() << "new site tab index is : " << index;

    if(index == 1)
    {
        this->refreshSitesTreeView();
        //editAvgMeteoForm* uiEditAvgMeteo = new editAvgMeteoForm();
        //ui->EditSiteScrollArea->setWidget(uiEditAvgMeteo);
    }
}

void MainWindow::on_sitesTreeView_entered(const QModelIndex &index)
{
    //qDebug() << "entered : " << index;
}

void MainWindow::on_sitesTreeView_activated(const QModelIndex &index)
{
    //qDebug() << "activated : " << index;
}

void MainWindow::on_sitesTreeView_clicked(const QModelIndex &index)
{
    QString key = index.data(10000).toString();
    int year = index.data(10001).toInt();
    QString yearsCsv = index.data(10002).toString();
    QString name = index.data(0).toString();

    if(0 == year)
    {
        editAvgMeteoForm* uiEditAvgMeteo = new editAvgMeteoForm();
        uiEditAvgMeteo->setEditSiteNameTextEditText(name);
        uiEditAvgMeteo->setMainWindowPointer(this);
        uiEditAvgMeteo->setSiteId(key.toInt());
        uiEditAvgMeteo->generateGraph(key.toInt());
        uiEditAvgMeteo->setEditSiteChartTitleLabel("Températures moyennes des années (" + yearsCsv + ")");

        ui->EditSiteScrollArea->setWidget(uiEditAvgMeteo);
        qDebug() << "key  : " << key;
        qDebug() << "year : " << QString::number(year);
        qDebug() << "name : " << name;
    }
    else
    {
        editYearMeteoForm* uiEditYearMeteo = new editYearMeteoForm();
        uiEditYearMeteo->setMainWindowPointer(this);
        uiEditYearMeteo->setSiteId(key.toInt());
        uiEditYearMeteo->setYear(year);
        uiEditYearMeteo->setEditYearChartTitleLabel("Température moyenne pour l'année " + QString::number(year));
        ui->EditSiteScrollArea->setWidget(uiEditYearMeteo);

        HtmlChartMaker htmlChartMaker;
        Meteo* meteo = this->mDbi.getMeteo(key.toInt(), year);

        QString html = htmlChartMaker.generateHtmlChartWithMap(meteo->getMeteo(), year, true);
        uiEditYearMeteo->setWebViewHtml(html);
    }
}

void MainWindow::loadSiteScrollArea(int siteId, int year)
{
    qDebug() << "loadSiteScrollArea";
    editYearMeteoForm* uiEditYearMeteo = new editYearMeteoForm();
    uiEditYearMeteo->setMainWindowPointer(this);
    uiEditYearMeteo->setSiteId(siteId);
    uiEditYearMeteo->setYear(year);
    uiEditYearMeteo->setEditYearChartTitleLabel("Température moyenne pour l'année " + QString::number(year));
    ui->EditSiteScrollArea->setWidget(uiEditYearMeteo);

    HtmlChartMaker htmlChartMaker;
    Meteo* meteo = this->mDbi.getMeteo(siteId, year);

    QString html = htmlChartMaker.generateHtmlChartWithMap(meteo->getMeteo(), year, true);
    uiEditYearMeteo->setWebViewHtml(html);
}

void MainWindow::on_sitesTreeView_pressed(const QModelIndex &index)
{
    //qDebug() << "pressed : " << index;
}

void MainWindow::on_EditSiteSaveButton_clicked()
{

}

void MainWindow::on_leftTabWidget_currentChanged(int index)
{

    if(index == 3 || index == 2) // previsions || intervalle TIF/Récolte
    {
        QString selectedVarieteId = ui->previsionVarieteSelect->itemData(ui->previsionVarieteSelect->currentIndex()).toString();
        QString selectedSiteId = ui->previsionSiteSelect->itemData(ui->previsionSiteSelect->currentIndex()).toString();

        ui->previsionVarieteSelect->clear();
        ui->previsionSiteSelect->clear();
        ui->intervallesVarieteSelect->clear();
        ui->intervallesSiteSelect->clear();

        QList<Site*> siteList = this->sDbi.getAllSites();
        ui->previsionSiteSelect->addItem("Sélectionnez","0");
        ui->intervallesSiteSelect->addItem("Sélectionnez","0");

        foreach(Site* site, siteList)
        {
            ui->previsionSiteSelect->addItem(site->getNom(), QString::number(site->getId()));
            ui->intervallesSiteSelect->addItem(site->getNom(), QString::number(site->getId()));
            if(0 == QString::number(site->getId()).compare(selectedSiteId))
            {
                ui->previsionSiteSelect->setCurrentIndex(ui->previsionSiteSelect->count() - 1);
                ui->intervallesSiteSelect->setCurrentIndex(ui->previsionSiteSelect->count() - 1);
            }
        }

        QList<VarietesAnanas*> varieteList = this->vDbi.getAllvarietes();
        ui->previsionVarieteSelect->addItem("Sélectionnez","0");
        ui->intervallesVarieteSelect->addItem("Sélectionnez","0");
        foreach(VarietesAnanas* variete, varieteList)
        {
            ui->previsionVarieteSelect->addItem(variete->getNom(), QString::number(variete->getId()));
            ui->intervallesVarieteSelect->addItem(variete->getNom(), QString::number(variete->getId()));
            if(0 == QString::number(variete->getId()).compare(selectedVarieteId))
            {
                ui->previsionVarieteSelect->setCurrentIndex(ui->previsionVarieteSelect->count() - 1);
                ui->intervallesVarieteSelect->setCurrentIndex(ui->previsionVarieteSelect->count() - 1);
            }
        }
    }
}

void MainWindow::deleteYearFromSite(int siteId, int year)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cette année?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qDebug() << "will delete year : " << year << " for site with id: " << siteId;
        this->mDbi.deleteMeteo(siteId, year);
        this->refreshSitesTreeView();
        delete ui->EditSiteScrollArea->widget();
    }
}

void MainWindow::deleteSite(int siteId)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce site?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qDebug() << "will delete site : " << siteId;
        this->sDbi.deleteSite(siteId);
        this->refreshSitesTreeView();
        delete ui->EditSiteScrollArea->widget();
    }
}

Meteo* MainWindow::getMeteo(int siteId,int year)
{
    qDebug() << "Will export data of year :" << year << " and for site with id:" << siteId;
    Meteo* meteo = this->mDbi.getMeteo(siteId, year);
    return meteo;
}

void MainWindow::displayEditMeteo(int siteId, int year)
{
    editMeteoDataForm* uiEditMeteoDataForm = new editMeteoDataForm();
    uiEditMeteoDataForm->setMainWindowPointer(this);
    uiEditMeteoDataForm->setId(siteId);
    uiEditMeteoDataForm->setYear(year);
    uiEditMeteoDataForm->displayTable();
    ui->EditSiteScrollArea->setWidget(uiEditMeteoDataForm);
}

void MainWindow::on_previsionSiteSelect_currentIndexChanged(int index)
{
    QString selectedModelId = ui->previsionModelSelect->itemData(ui->previsionModelSelect->currentIndex()).toString();

    qDebug() << "new index" << index;
    QString QsSiteId = ui->previsionSiteSelect->itemData(index).toString();
    qDebug() << "siteId :" << QsSiteId.toInt();

    ui->previsionModelSelect->clear();
    QList<Meteo*> meteoList = this->mDbi.getMeteo(QsSiteId.toInt());
    qDebug() << "meteoList.size() :" << meteoList.size();

    if(meteoList.size() > 0)
    {
        ui->previsionModelSelect->addItem("Sélectionnez","0");
        if(meteoList.size() > 1)
        {
            ui->previsionModelSelect->addItem("Moyenne de toutes les années","-1");
        }
        foreach(Meteo* meteo, meteoList)
        {
            ui->previsionModelSelect->addItem(QString::number(meteo->getYear()), meteo->getYear());
        }
    }

    //ui->previsionsDateEdit->da
}

void MainWindow::on_calculateDateRecolteBtn_clicked()
{
    qDebug() << "clicked calculateDateRecolteBtn";
    if(true == ui->previsionVarieteSelect->itemData(ui->previsionVarieteSelect->currentIndex()).isNull()
            || 0 == ui->previsionVarieteSelect->itemData(ui->previsionVarieteSelect->currentIndex()).toString().compare(QString("0")))
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner une variété"));
        return ;
    }
    else if(true == ui->previsionSiteSelect->itemData(ui->previsionSiteSelect->currentIndex()).isNull()
            || 0 == ui->previsionSiteSelect->itemData(ui->previsionSiteSelect->currentIndex()).toString().compare(QString("0")))
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner un site"));
        return ;
    }

    else if(true == ui->previsionModelSelect->itemData(ui->previsionModelSelect->currentIndex()).isNull()
            || 0 == ui->previsionModelSelect->itemData(ui->previsionModelSelect->currentIndex()).toString().compare(QString("0")))
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner un modèle de prévision"));
        return ;
    }
    else if(false == ui->tifRadioBtn->isChecked() && false == ui->floraisonRadioBtn->isChecked() && false == ui->recolteRadioBtn->isChecked())
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner si la date correspond à la floraison ou au TIF."));
        return ;
    }

        HtmlChartMaker htmlChartMaker;
        VarietesAnanas* variete = this->vDbi.getVarieteWithId(ui->previsionVarieteSelect->itemData(ui->previsionVarieteSelect->currentIndex()).toInt());

        Site* site = this->sDbi.getSite(ui->previsionSiteSelect->itemData(ui->previsionSiteSelect->currentIndex()).toInt());
        QStringList years = site->getYears();

        QList<QMap<QString, QStringList> > finalTmpAvg;
        QMap<QString, QStringList> avgOfTempYears;
        int modelYear = 1;

        foreach(QString year, years)
        {
            qDebug() << "get site with ID :" << site->getId() << " and with year :" << year.toInt();
            Meteo* meteo = this->mDbi.getMeteo(site->getId(), year.toInt());
            finalTmpAvg.append(meteo->getMeteo());
        }

        if(0 == ui->previsionModelSelect->itemData(ui->previsionModelSelect->currentIndex()).toString().compare("-1"))
        {

            avgOfTempYears = htmlChartMaker.calculateAvgOfTempYears(finalTmpAvg);
        }
        else
        {
            modelYear = ui->previsionModelSelect->itemData(ui->previsionModelSelect->currentIndex()).toInt();
            Meteo* meteo = this->mDbi.getMeteo(site->getId(), modelYear);
            avgOfTempYears = meteo->getMeteo();
        }

        QDate selectedDate = ui->previsionsDateEdit->date();

        this->clearPrevisionsDebugPlainTextEdit();
        this->ui->previsionsDebugPlainTextEdit->setHidden(false);

        if(true == ui->tifRadioBtn->isChecked())
        {
            ui->previsionsDebugPlainTextEdit->clear();

            QDate dateFloraison = htmlChartMaker.predictDate(selectedDate, modelYear, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 1);
            QDate dateRecolte = htmlChartMaker.predictDate(dateFloraison, modelYear, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 2);

            ui->dateFloraisonLabelInput->setText(dateFloraison.toString("d ") + dateFloraison.toString("MMMM").mid(0,4) + dateFloraison.toString(" yyyy"));
            ui->dateRecolteLabelInput->setText(dateRecolte.toString("d ") + dateRecolte.toString("MMMM").mid(0,4) + dateRecolte.toString(" yyyy"));

            ui->dateFloraisonLabel->setHidden(false);
            ui->dateFloraisonLabelInput->setHidden(false);

            ui->dateRecolteLabel->setHidden(false);
            ui->dateRecolteLabelInput->setHidden(false);

            ui->dateTifLabel->setHidden(true);
            ui->dateTifInputLabel->setHidden(true);
        }

        if(true == ui->floraisonRadioBtn->isChecked())
        {
            ui->previsionsDebugPlainTextEdit->clear();

            QDate dateRecolte = htmlChartMaker.predictDate(selectedDate, modelYear, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 2);

            ui->dateRecolteLabelInput->setText(dateRecolte.toString("d ") + dateRecolte.toString("MMMM").mid(0,4) + dateRecolte.toString(" yyyy"));

            ui->dateRecolteLabel->setHidden(false);
            ui->dateRecolteLabelInput->setHidden(false);

            ui->dateFloraisonLabel->setHidden(true);
            ui->dateFloraisonLabelInput->setHidden(true);

            ui->dateTifLabel->setHidden(true);
            ui->dateTifInputLabel->setHidden(true);
        }

        if(true == ui->recolteRadioBtn->isChecked())
        {
            ui->previsionsDebugPlainTextEdit->clear();

            ui->dateTifLabel->setHidden(false);
            ui->dateTifInputLabel->setHidden(false);

            ui->dateFloraisonLabel->setHidden(true);
            ui->dateFloraisonLabelInput->setHidden(true);

            ui->dateRecolteLabel->setHidden(true);
            ui->dateRecolteLabelInput->setHidden(true);

            QDate dateTif = htmlChartMaker.predictDate(selectedDate, modelYear, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 3);

            ui->dateTifInputLabel->setText(dateTif.toString("d ") + dateTif.toString("MMMM").mid(0,3) + dateTif.toString(" yyyy"));
        }

}

void MainWindow::on_actionAide_triggered()
{
    HelpForm *helpForm = new HelpForm();
    helpForm->show();
    helpForm->raise();
    helpForm->activateWindow();

}

void MainWindow::on_intervallesSubmitBtn_clicked()
{

    if(true == ui->intervallesVarieteSelect->itemData(ui->intervallesVarieteSelect->currentIndex()).isNull()
            || 0 == ui->intervallesVarieteSelect->itemData(ui->intervallesVarieteSelect->currentIndex()).toString().compare(QString("0")))
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner une variété"));
        return ;
    }
    else if(true == ui->intervallesSiteSelect->itemData(ui->intervallesSiteSelect->currentIndex()).isNull()
            || 0 == ui->intervallesSiteSelect->itemData(ui->intervallesSiteSelect->currentIndex()).toString().compare(QString("0")))
    {
        QMessageBox msgBox;
        msgBox.warning(this, QString("Erreur"), QString("Vous devez sélectionner un site"));
        return ;
    }


    QDate currentDate = QDate::fromString("0001-01-01","yyyy-MM-dd");
    HtmlChartMaker htmlChartMaker;

    VarietesAnanas* variete = this->vDbi.getVarieteWithId(ui->intervallesVarieteSelect->itemData(ui->intervallesVarieteSelect->currentIndex()).toInt());

    Site* site = this->sDbi.getSite(ui->intervallesSiteSelect->itemData(ui->intervallesSiteSelect->currentIndex()).toInt());

    ui->intervalleProgressBar->setHidden(false);

    QMap<int, QStringList> weekIntervalles;

    int i = 1;
    while(1 >= currentDate.toString("yyyy").toInt())
    {
        QStringList list;
        ui->intervalleProgressBar->setValue(i);

        QDate floraisonDate = htmlChartMaker.predictDate(currentDate, 1, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 1);

        QDate recolteDate = htmlChartMaker.predictDate(floraisonDate, 1, this, site, variete->getTBase1(), variete->getTFloraison(), variete->getTBase2(), variete->getTRecolte(), 2);

        qDebug() << "week : " << QString::number(i) << "    recolte interval : " << currentDate.daysTo(recolteDate);

        list.append(QString::number(currentDate.daysTo(floraisonDate)));
        list.append(QString::number(currentDate.daysTo(recolteDate)));

        //calculate temperature

        //get meteo
        //htmlChartMaker.

        QList<QMap<QString, QStringList> > finalTmpAvg;
        QMap<QString, QStringList> avgOfTempYears;

        foreach(QString year, site->getYears())
        {
            qDebug() << "get site with ID :" << site->getId() << " and with year :" << year.toInt();
            Meteo* meteo = this->mDbi.getMeteo(site->getId(), year.toInt());
            finalTmpAvg.append(meteo->getMeteo());
        }

        avgOfTempYears = htmlChartMaker.calculateAvgOfTempYears(finalTmpAvg);

        int tempCount = 0;
        float tempSum = 0;
        QDate dayDate = QDate::fromString(currentDate.toString("yyyyMMdd"),"yyyyMMdd");

        for(int i=0; i<7; i++)
        {
            dayDate = dayDate.addDays(i);
            if(avgOfTempYears.contains(dayDate.toString("yyyyMMdd")))
            {
                tempCount++;
                tempSum += avgOfTempYears.value(dayDate.toString("yyyyMMdd")).at(2).toFloat();
            }
        }

        list.append(QString::number(tempSum/tempCount)); //température

        weekIntervalles.insert(i, list);
        currentDate = currentDate.addDays(7);
        i++;
        qApp->processEvents();
    }

    ui->IntervalleWebView->setHtml(htmlChartMaker.generateHtmlWeekIntervalle(weekIntervalles));

    ui->intervalleProgressBar->setHidden(true);

}

void MainWindow::insertTextInPrevisionsDebugPlainTextEdit(QString text)
{
    this->ui->previsionsDebugPlainTextEdit->insertPlainText(text);
}

void MainWindow::clearPrevisionsDebugPlainTextEdit()
{
    this->ui->previsionsDebugPlainTextEdit->clear();
}
