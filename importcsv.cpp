#include "importcsv.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QFile>

void ImportCsv::importFile()
{

    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Ouvrir un fichier"),
                                                    "",
                                                    QObject::tr("Fichiers csv(*.csv)"));
    qDebug() << "fileName is : " << fileName;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while(false == in.atEnd())
        {
            QString line = in.readLine();
            QRegExp ExpDate("^([0-9]{4}-[0-9]{2}-[0-9]{2}) [0-9]{2}:[0-9]{2}:[0-9]{2}$");
            QRegExp ExpTemp("^[0-9]{0,}[\.]{1}[0-9]{0,}$");

            QStringList lineColumns =  line.split(",");

            for(int i=0; i < lineColumns.size(); i++)
            {

                QString column = lineColumns.at(i);
                if(ExpDate.exactMatch(column))
                {
                    qDebug() << "date: " << column;
                    //qDebug() << "date jour: " << ExpDate.cap(1);
                }
                if(ExpTemp.exactMatch(column))
                {
                    qDebug() << "temp: " << column;
                }

            }
        }
    }
}
