#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "varietesananas.h"
#include <QMessageBox>
#include <QUrl>
#include <QStringList>
#include <QList>
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->supprimerImageBtn->setHidden(true);
    this->hideEditVarieteInputs();

    //set the webView background to transparent
    QPalette palette = ui->ajouterSiteWebView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    ui->ajouterSiteWebView->page()->setPalette(palette);
    ui->ajouterSiteWebView->setAttribute(Qt::WA_OpaquePaintEvent, false);

    ui->ajouterSiteWebView->setHtml("<br/><br/><br/><br/><br/><br/> <center>Ajoutez un fichier avec des données météo pour faire apparaitre le graphique</center>");
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

    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Ouvrir un fichier"),
                                                    "",
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
    return appStoragePath;
}

QString MainWindow::getImageStoragePath()
{
    return imageStoragePath;
}

QString MainWindow::getDbPath()
{
    return imageStoragePath;
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

    foreach(VarietesAnanas *variete, varieteList)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(variete->getNom());
        item->setData(10000, QString::number(variete->getId()));
        qDebug() << "Saved data : " << item->data(10000);
        qDebug() << "Variete nom : " << variete->getNom();
        ui->varietesListWidget->addItem(item);
    }
}

void MainWindow::on_editAjouterImageVarieteBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Ouvrir un fichier"),
                                                    "",
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

    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Ouvrir un fichier"),
                                                    "",
                                                    QObject::tr("Fichiers csv(*.csv)"));
    if(false == fileName.isEmpty())
    {
        qDebug() << "fileName is : " << fileName;
        QFile file(fileName);
        QString html = "";

        ui->ajouterSiteMeteoFilePath->setText(fileName);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
                QTextStream in(&file);
                QList<QStringList> tempList;
                while(!in.atEnd())
                {
                    QString line = in.readLine();
                    QRegExp ExpDate("^([0-9]{4}-[0-9]{2}-[0-9]{2}) [0-9]{2}:[0-9]{2}:[0-9]{2}$");
                    QRegExp ExpTemp("^[0-9]{0,}[\.]{1}[0-9]{0,}$");

                    QStringList lineColumns =  line.split(",");

                    QStringList tempListLine;
                    for(int i=0; i < lineColumns.size(); i++)
                    {
                        QString column = lineColumns.at(i);

                        if(ExpDate.exactMatch(column))
                        {
                            //qDebug() << "date: " << column;
                            tempListLine.append(column);
                            //qDebug() << "date jour: " << ExpDate.cap(1);
                        }
                        if(ExpTemp.exactMatch(column))
                        {
                            //qDebug() << "temp: " << column;
                            tempListLine.append(column);
                        }
                    }
                    tempList.append(tempListLine);
                }
                HtmlChartMaker htmlChartMaker;
                html = htmlChartMaker.generateHtmlChartWithTempData(tempList);
                this->importTempList = tempList;
        }

        qDebug() << "html: " << html;
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
        if(this->importTempList.count() > 0)
        {
            HtmlChartMaker htmlChartMaker;
            QStringList yearList = htmlChartMaker.getYearsWithTempData(this->importTempList);

            Site* site = new Site();
            site->setNom(ui->ajouterSiteNomInput->text());
            site->setYears(yearList);
            this->sDbi.saveSite(site);

            foreach (QString year, yearList)
            {
                qDebug() << "year: " << year;
                Meteo* meteo = new Meteo();
                meteo->setYear(year.toInt());
                meteo->addMeteoWithQMaps(htmlChartMaker.calculateMinDayTemp(this->importTempList),
                                         htmlChartMaker.calculateDayTempAverage(this->importTempList),
                                         htmlChartMaker.calculateMaxDayTemp(this->importTempList)
                                         );

                qDebug() << "meteo csv : " << meteo->exportMeteoAsCsv();
                meteo->setSiteId(this->sDbi.lastInsertedRowId());
                this->mDbi.saveMeteo(meteo);
            }

            //add meteo
        }


        QList<QStringList> emptyTempList;
        this->importTempList = emptyTempList;
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

        foreach (QString year, yearList)
        {
            QStandardItem* subItem = new QStandardItem(year);
            subItem->setFlags(item->flags() & ~Qt::ItemIsEditable); //set Item as being not editable
            subItem->setData(QString::number(site->getId()),10000); //set key
            subItem->setData(year,10001); //set year
            item->appendRow(subItem);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); //set Item as being not editable
        }
        model->appendRow(item);
    }
    ui->sitesTreeView->setModel(model);
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
    QString name = index.data(0).toString();

    if(0 == year)
    {
        editAvgMeteoForm* uiEditAvgMeteo = new editAvgMeteoForm();
        uiEditAvgMeteo->setEditSiteNameTextEditText(name);
        ui->EditSiteScrollArea->setWidget(uiEditAvgMeteo);

        qDebug() << "key  : " << key;
        qDebug() << "year : " << QString::number(year);
        qDebug() << "name : " << name;

    }
    else
    {
        editYearMeteoForm* uiEditYearMeteo = new editYearMeteoForm();
        ui->EditSiteScrollArea->setWidget(uiEditYearMeteo);

        HtmlChartMaker htmlChartMaker;
        Meteo* meteo = this->mDbi.getMeteo(key.toInt(), year);

        QString html = htmlChartMaker.generateHtmlChartWithMap(meteo->getMeteo());
        uiEditYearMeteo->setWebViewHtml(html);
    }
}

void MainWindow::on_sitesTreeView_pressed(const QModelIndex &index)
{
    //qDebug() << "pressed : " << index;
}

void MainWindow::on_EditSiteSaveButton_clicked()
{

}
