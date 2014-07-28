#include "varietedatabaseinterface.h"
#include "varietesananas.h"
#include <string>
#include <QDebug>
#include <QtSql/QtSql>


VarieteDatabaseInterface::VarieteDatabaseInterface()
{
}

void VarieteDatabaseInterface::setStoragePaths(QString appStoragePath, QString imageStoragePath, QString dbPath)
{
    this->appStoragePath = appStoragePath;
    this->imageStoragePath = imageStoragePath;

    char* nDbPath = new char [dbPath.size()+1];;
    strcpy(nDbPath, dbPath.toStdString().c_str());

    qDebug() << "db: " << dbPath;

    this->dbPath = nDbPath;

    qDebug() << "db: " << this->dbPath;

}

VarietesAnanas* VarieteDatabaseInterface::getVarieteWithId(int id)
{
    VarietesAnanas* variete = new VarietesAnanas(appStoragePath, imageStoragePath);
    qDebug() << "db: " << this->dbPath;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM varietes WHERE ID = :id");
    query.bindValue(":id", id);
    if(query.exec())
    {
        query.next();
        variete->setId( query.value(0).toInt() );
        variete->setNom( query.value(1).toString() );
        variete->setNewImageName( query.value(2).toString() );
        variete->setTBase1( query.value(3).toFloat() );
        variete->setTFloraison( query.value(4).toFloat() );
        variete->setTBase2( query.value(5).toFloat() );
        variete->setTRecolte( query.value(6).toFloat() );
    }
    else
    {
        qDebug() << "SQL ERROR : " << query.lastError();
    }
    db.close();
    return variete;
}

void VarieteDatabaseInterface::deleteVariete(int id)
{
    qDebug() << "db: " << this->dbPath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QSqlQuery query;
    query.prepare("DELETE FROM varietes WHERE ID = :id;");
    query.bindValue(":id", id);
    if(false == query.exec())
    {
       qDebug() << "SQL ERROR : " << query.lastError();
    }
    db.commit();
    db.close();
}

void VarieteDatabaseInterface::saveVariete(VarietesAnanas* variete)
{
    qDebug() << "db: " << this->dbPath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    if(0 != variete->getId()) //if an id is set we update the variete in the database
    {
        qDebug() << " update variete ";
        QSqlQuery query;
        query.prepare("UPDATE varietes SET nom = :nom, newImageName = :newImageName, tBase1 = :tBase1, tFloraison = :tFloraison, tBase2 = :tBase2, tRecolte = :tRecolte WHERE ID = :id;");
        query.bindValue(":id", variete->getId());
        query.bindValue(":nom", variete->getNom());
        query.bindValue(":newImageName", variete->getNewImageName());
        query.bindValue(":tBase1", variete->getTBase1());
        query.bindValue(":tFloraison", variete->getTFloraison());
        query.bindValue(":tBase2", variete->getTBase2());
        query.bindValue(":tRecolte", variete->getTRecolte());
        if(false == query.exec())
        {
            qDebug() << "SQL ERROR : " << query.lastError();
        }
    }
    else
    {
        qDebug() << " insert variete ";
        QSqlQuery query;
        query.prepare("INSERT INTO varietes(ID,nom,newImageName,tBase1,tFloraison,tBase2,tRecolte) VALUES (NULL,:nom,:newImageName,:tBase1,:tFloraison,:tBase2,:tRecolte);");
        query.bindValue(":nom", variete->getNom());
        query.bindValue(":newImageName", variete->getNewImageName());
        query.bindValue(":tBase1", variete->getTBase1());
        query.bindValue(":tFloraison", variete->getTFloraison());
        query.bindValue(":tBase2", variete->getTBase2());
        query.bindValue(":tRecolte", variete->getTRecolte());
        if(false == query.exec())
        {
            qDebug() << "SQL ERROR : " << query.lastError();
        }
    }
    db.commit();
    db.close();
    /*

    {
        sql = sqlite3_mprintf("INSERT INTO varietes(ID,nom,newImageName,tBase1,tFloraison,tBase2,tRecolte) VALUES(NULL,'%q','%q','%q','%q','%q','%q');",
                              variete->getNom().c_str(),
                              variete->getNewImageName().c_str(),
                              QString::number(variete->getTBase1()).toStdString().c_str(),
                              QString::number(variete->getTFloraison()).toStdString().c_str(),
                              QString::number(variete->getTBase2()).toStdString().c_str(),
                              QString::number(variete->getTRecolte()).toStdString().c_str());
    }

    qDebug() << "sql request : " << sql;


    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
       fprintf(stdout, "a new variete was added in the database\n");
    }
    sqlite3_close(db);
    */
}

QList<VarietesAnanas*> VarieteDatabaseInterface::getAllvarietes()
{
    QList<VarietesAnanas*> varieteList;
    qDebug() << "db: " << this->dbPath;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QSqlQuery query= db.exec("SELECT * FROM varietes");
    while (query.next())
    {
        VarietesAnanas* variete = new VarietesAnanas(this->appStoragePath, this->imageStoragePath);
        variete->setId( query.value(0).toInt() );
        variete->setNom( query.value(1).toString() );
        variete->setNewImageName( query.value(2).toString() );
        variete->setTBase1( query.value(3).toFloat() );
        variete->setTFloraison( query.value(4).toFloat() );
        variete->setTBase2( query.value(5).toFloat() );
        variete->setTRecolte( query.value(6).toFloat() );
        varieteList.append( variete );
    }
    db.close();

    return varieteList;
}

void VarieteDatabaseInterface::setdbPath(QString dbPath)
{
    this->dbPath = dbPath;
}

QString VarieteDatabaseInterface::getDbPath()
{
    return this->dbPath;
}
