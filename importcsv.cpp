#include "importcsv.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QRegExp>
#include <QDateTime>

ImportCsv::ImportCsv()
{

}

QList<QStringList> ImportCsv::importFile(QString fileName)
{
    QList<QStringList> tempList;
    if(false == fileName.isEmpty())
    {
        qDebug() << "fileName is : " << fileName;
        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
                QTextStream in(&file);
                while(!in.atEnd())
                {
                    QString line = in.readLine();

                    if(line.contains(";"))
                    {
                        line = line.replace(";","!##!");
                    }
                    else
                    {
                        line = line.replace(",","!##!");
                    }

                    QRegExp ExpDate("^.{0,}([0-9]{4}-[0-9]{2}-[0-9]{2}).{0,}$");
                    QRegExp ExpDate2("^.{0,}([0-9]{1,2}\/[0-9]{1,2}\/[0-9]{4}).{0,}$");

                    QRegExp ExpTemp("^.{0,}([0-9]{1,}[\.]{1}[0-9]{3}).{0,}$");
                    QRegExp ExpTempComma("^.{0,}([0-9]{1,}[\,]{1}[0-9]{3}).{0,}$");
                    QRegExp ExpPluvio("^.{0,}([0-9]{1,}[\.]{1}[0-9]{2})[^0-9]{0,}$");

                    QStringList lineColumns = line.split("!##!");
                    QStringList tempListLine;

                    bool foundDate = false;
                    for(int i= 0; i < lineColumns.size(); i++)
                    {
                        QString column = lineColumns.at(i);
                        if(ExpDate.exactMatch(column))
                        {
                            //qDebug() << "Captured : " << ExpDate.cap(1);
                            QDateTime qDate = QDateTime::fromString(ExpDate.cap(1), "yyyy-MM-dd");
                            QString date = qDate.toString("yyyy-MM-dd hh:mm:ss");
                            tempListLine.append(date);
                            foundDate = true;
                        }
                        if(ExpDate2.exactMatch(column))
                        {
                            //qDebug() << "Captured : " << ExpDate2.cap(1);
                            QDateTime qDate = QDateTime::fromString(ExpDate2.cap(1), "M/d/yyyy");
                            QString date = qDate.toString("yyyy-MM-dd hh:mm:ss");
                            tempListLine.append(date);
                            foundDate = true;
                        }
                        if(ExpTemp.exactMatch(column))
                        {
                            //qDebug() << "Captured2 : " << ExpTemp.cap(1);
                            tempListLine.append(ExpTemp.cap(1));
                        }
                        if(ExpTempComma.exactMatch(column))
                        {
                            //qDebug() << "Captured2 : " << ExpTemp.cap(1);
                            tempListLine.append(ExpTemp.cap(1).replace(",","."));
                        }
                        if(ExpPluvio.exactMatch(column))
                        {
                            //qDebug() << "Captured3 : " << ExpPluvio.cap(1);
                            tempListLine.append(ExpPluvio.cap(1));
                        }
                    }
                    if(true == foundDate)
                        tempList.append(tempListLine);
                }
        }
    }
    return tempList;
}
