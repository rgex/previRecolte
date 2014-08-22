#include "htmlchartmaker.h"
#include <string>
#include <iostream>
#include <sstream>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include "meteodatabaseinterface.h"
#include <QMessageBox>
#include <QFile>

HtmlChartMaker::HtmlChartMaker()
{

}

QDate HtmlChartMaker::predictDate(QDate selectedDate, int modelYear, MainWindow* mainWindow, Site* site, float base1, float floraison, float base2, float recolte, int predictionType)
{
    MeteoDatabaseInterface meteoDatabaseInterface;
    meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

    QStringList years = site->getYears();

    QList<QMap<QString, QStringList> > finalTmpAvg;
    QMap<QString, QStringList> avgOfTempYears;

    foreach(QString year, years)
    {
        qDebug() << "get site with ID :" << site->getId() << " and with year :" << year.toInt();
        Meteo* meteo = meteoDatabaseInterface.getMeteo(site->getId(), year.toInt());
        finalTmpAvg.append(meteo->getMeteo());
    }

    if(1 == modelYear)
    {
        avgOfTempYears = this->calculateAvgOfTempYears(finalTmpAvg);
    }
    else
    {
        qDebug() << "Model year is : " << modelYear;
        QList<QMap<QString, QStringList> > finalTmp;
        Meteo* meteo = meteoDatabaseInterface.getMeteo(site->getId(), modelYear);
        finalTmp.append(meteo->getMeteo());
        avgOfTempYears = this->calculateAvgOfTempYears(finalTmp);
    }


    float tmpFloraison = floraison;
    float tmpSum = 0;
    float lastAdd = 0;
    int exactValues = 0;
    int prognosedValues = 0;
    float tmpRecolte = recolte;
    QString modelYearString = QString::number(modelYear);

    if(1 == modelYearString.length())
        modelYearString = "000" + modelYearString;

    if(2 == predictionType) //recolte prediction
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### Récolte ###\n");
        while(tmpFloraison > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;

            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base2;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base2;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }

        qDebug() << "date de floraison :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;

    }

    if(1 == predictionType)  //floraison prediction
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### Floraison ###\n");
        while(tmpRecolte > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base1;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base1;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }

        qDebug() << "date de récolte :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;
    }

    if(3 == predictionType)  //calculer TIF
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### TIF ###\n");
        while(tmpRecolte > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base2;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base2;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(-1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }
        qDebug() << "date du TIF :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;


        float tmpFloraison = floraison;
        float tmpSum = 0;
        float lastAdd = 0;
        int exactValues = 0;
        int prognosedValues = 0;

        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### 2ème partie ###\n");

        while(tmpFloraison > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base1;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base1;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(-1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }
        qDebug() << "date du TIF :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;

    }

    return selectedDate;
}

QMap<QString, QStringList> HtmlChartMaker::calculateAvgOfTempYears(QList<QMap<QString, QStringList> > temps)
{
    //first get years list
    QStringList yearsList;
    QMap<QString, QStringList> tempsMap;
    QMap<QString, QStringList> avgMap;

    foreach(tempsMap, temps)
    {
        foreach(QStringList temperatures, tempsMap)
        {
            if(temperatures.size() > 0)
            {
                if(false == yearsList.contains(temperatures.at(0).mid(0, 4)))
                {
                    yearsList.append(temperatures.at(0).mid(0, 4));
                }
            }
        }
    }

    QDateTime qDate0 = QDateTime::fromString("0001-01-01","yyyy-MM-dd");

    while(qDate0.toString("yyyy").toInt() < 2)
    {
        //qDebug() << "qDate0.toString(yyyy).toInt() " << qDate0.toString("yyyyMM").totInt();

        QMap<QString, QStringList> tempsMap;
        float avgTempAvg = 0;
        float avgTempMin = 0;
        float avgTempMax = 0;
        float avgPluvioAvg = 0;

        float tempsAvgSum = 0;
        float tempsMinSum = 0;
        float tempsMaxSum = 0;
        float pluvioAvgSum = 0;

        int tempsCount = 0;
        foreach(tempsMap, temps)
        {
            foreach(QString year, yearsList)
            {
                qDebug() << "qmap key : " << qDate0.toString(year + qDate0.toString("MMdd"));
                if(true == tempsMap.contains(qDate0.toString(year + qDate0.toString("MMdd"))))
                {
                    QStringList tmpList = tempsMap.value(qDate0.toString(year + qDate0.toString("MMdd")));
                    if(tmpList.size() >= 4 && tmpList.at(1).toFloat() > 0 && tmpList.at(2).toFloat() > 0 && tmpList.at(3).toFloat() > 0)
                    {
                        tempsAvgSum += tmpList.at(1).toFloat();
                        tempsMinSum += tmpList.at(2).toFloat();
                        tempsMaxSum += tmpList.at(3).toFloat();
                        pluvioAvgSum += tmpList.at(4).toFloat();
                        tempsCount++;
                    }
                }
            }

        }

        if(tempsCount > 0)
        {
            avgTempAvg = tempsAvgSum / (float)tempsCount;
            avgTempMin = tempsMinSum / (float)tempsCount;
            avgTempMax = tempsMaxSum / (float)tempsCount;
            avgPluvioAvg = pluvioAvgSum / (float)tempsCount;

            QStringList resTemps;
            qDebug() << "will insert date : " << qDate0.toString("yyyyMMdd");
            qDebug() << "will insert avg temp : " << QString::number(avgTempAvg);
            resTemps.append(qDate0.toString("yyyyMMdd"));
            resTemps.append(QString::number(avgTempAvg));
            resTemps.append(QString::number(avgTempMin));
            resTemps.append(QString::number(avgTempMax));
            resTemps.append(QString::number(avgPluvioAvg));
            avgMap.insert(qDate0.toString("yyyyMMdd"), resTemps);
        }
        qDate0 = qDate0.addDays(1);
    }

    return avgMap;
}

QMap<QString, float> HtmlChartMaker::calculateDayTempAverage(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempAvgMap;
    QMap<QString, float> dayTempAvgFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            //qDebug() << "nDate : " << qDate.toString("yyyyMMdd");
            QStringList dayTempsList = dayTempAvgMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempAvgMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >= 2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            //qDebug() << "nDate : " << qDate.toString("yyyyMMdd");
            QStringList dayTempsList = dayTempAvgMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempAvgMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QStringList qMapElement = dayTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < qMapElement.size(); i++)
        {
            sum += qMapElement.at(i).toFloat();
        }
        float avg = sum / (float)qMapElement.size();

        //QString qsSum;
        //qDebug() << "day avg : " << qsSum.setNum(avg, 'f', 2);

        dayTempAvgFloatMap.insert(qMapKey, avg);
    }

    return dayTempAvgFloatMap;
}


QMap<QString, float> HtmlChartMaker::calculateWeekTempAverage(QMap<QString, float> dayTempAvgMap)
{
    QMap<QString, QStringList> weekTempAvgMap;
    QMap<QString, float> weekTempAvgFloatMap;

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QString date = QString::number(QDate::fromString(qMapKey, "yyyyMMdd").weekNumber());
        QString tmp;
        tmp = tmp.setNum(dayTempAvgMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = weekTempAvgMap.value(date);
        tmpStringList.append(tmp);
        weekTempAvgMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, weekTempAvgMap.keys())
    {
        QStringList tmpStringList = weekTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            sum += tmpStringList.at(i).toFloat();
        }
        float avg = sum / (float)tmpStringList.size();
        weekTempAvgFloatMap.insert(qMapKey, avg);
    }

    return weekTempAvgFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMonthTempAverage(QMap<QString, float> dayTempAvgMap)
{
    QMap<QString, QStringList> monthTempAvgMap;
    QMap<QString, float> monthTempAvgFloatMap;

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayTempAvgMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthTempAvgMap.value(date);
        tmpStringList.append(tmp);
        monthTempAvgMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthTempAvgMap.keys())
    {
        QStringList tmpStringList = monthTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            sum += tmpStringList.at(i).toFloat();
        }
        float avg = sum / (float)tmpStringList.size();
        monthTempAvgFloatMap.insert(qMapKey, avg);
    }

    return monthTempAvgFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMaxDayTemp(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempMap;
    QMap<QString, float> dayTempMaxFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(3);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >=2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempMap.keys())
    {
        QStringList qMapElement = dayTempMap.value(qMapKey);
        float max = 0;
        for(int i=0; i < qMapElement.size(); i++)
        {
            if(qMapElement.at(i).toFloat() > max)
                max = qMapElement.at(i).toFloat() ;
        }

        dayTempMaxFloatMap.insert(qMapKey, max);
    }

    return dayTempMaxFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMaxMonthTemp(QMap<QString, float> dayMaxTempMap)
{
    QMap<QString, QStringList> monthMaxTempMap;
    QMap<QString, float> monthMaxTempFloatMap;

    foreach(QString qMapKey, dayMaxTempMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayMaxTempMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthMaxTempMap.value(date);
        tmpStringList.append(tmp);
        monthMaxTempMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthMaxTempMap.keys())
    {
        QStringList tmpStringList = monthMaxTempMap.value(qMapKey);
        float max = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            if(tmpStringList.at(i).toFloat() > max)
                max = tmpStringList.at(i).toFloat();
        }
        monthMaxTempFloatMap.insert(qMapKey, max);
    }

    return monthMaxTempFloatMap;
}


QMap<QString, float> HtmlChartMaker::calculateMinDayTemp(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempMap;
    QMap<QString, float> dayTempMinFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(2);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >= 2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempMap.keys())
    {
        QStringList qMapElement = dayTempMap.value(qMapKey);
        float min = 10000;
        for(int i=0; i < qMapElement.size(); i++)
        {
            if(qMapElement.at(i).toFloat() < min)
                min = qMapElement.at(i).toFloat();
        }
        dayTempMinFloatMap.insert(qMapKey, min);
    }
    return dayTempMinFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateDayPluviometry(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayPluvioMap;
    QMap<QString, float> dayPluvioAvgFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 5)
        {
            QString date = temperature.at(0);
            QString pluvio = temperature.at(4);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayPluvioList = dayPluvioMap.value(qDate.toString("yyyyMMdd"));
            dayPluvioList.append(pluvio);
            dayPluvioMap.insert(qDate.toString("yyyyMMdd"), dayPluvioList);
        }
        else if(temperature.size() == 3)
        {
            QString date = temperature.at(0);
            QString pluvio = temperature.at(2);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayPluvioList = dayPluvioMap.value(qDate.toString("yyyyMMdd"));
            dayPluvioList.append(pluvio);
            dayPluvioMap.insert(qDate.toString("yyyyMMdd"), dayPluvioList);
        }
    }

    foreach(QString qMapKey, dayPluvioMap.keys())
    {
        QStringList qMapElement = dayPluvioMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < qMapElement.size(); i++)
        {
            sum += qMapElement.at(i).toFloat();
        }
        float avg = sum / (float)qMapElement.size();
        dayPluvioAvgFloatMap.insert(qMapKey, avg);
    }

    return dayPluvioAvgFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMinMonthTemp(QMap<QString, float> dayMinTempMap)
{
    QMap<QString, QStringList> monthMinTempMap;
    QMap<QString, float> monthMinTempFloatMap;

    foreach(QString qMapKey, dayMinTempMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayMinTempMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthMinTempMap.value(date);
        tmpStringList.append(tmp);
        monthMinTempMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthMinTempMap.keys())
    {
        QStringList tmpStringList = monthMinTempMap.value(qMapKey);
        float min = 10000;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            if(tmpStringList.at(i).toFloat() < min)
                min = tmpStringList.at(i).toFloat();
        }
        monthMinTempFloatMap.insert(qMapKey, min);
    }

    return monthMinTempFloatMap;
}

QStringList HtmlChartMaker::getYearsWithTempData(QList<QStringList> temperatures)
{
    QStringList years;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() > 1)
        {
            QString date = temperature.at(0);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QString QsDate = qDate.toString("yyyy");
            if(false == years.contains(QsDate))
                years.append(QsDate);
        }
        qSort(years.begin(),years.end(),qGreater<QString>());
    }
    return years;
}

QString HtmlChartMaker::generateHtmlChartWithTempData(QList<QStringList> temperatures)
{
    QMap<QString, float> dayMaxTempMap = this->calculateMaxDayTemp(temperatures);
    QMap<QString, float> dayTempAvgMap = this->calculateDayTempAverage(temperatures);
    QMap<QString, float> dayMinTempMap = this->calculateMinDayTemp(temperatures);
    QMap<QString, float> dayPluviometry = this->calculateDayPluviometry(temperatures);

    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap, dayPluviometry);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap)
{
    return this->generateHtmlChartWithMap(tempMap, 0, false);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, bool completeMissingMonth)
{
    return this->generateHtmlChartWithMap(tempMap, 0, completeMissingMonth);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, int year, bool completeMissingMonth)
{

    QMap<QString, float> dayMaxTempMap;
    QMap<QString, float> dayTempAvgMap;
    QMap<QString, float> dayMinTempMap;
    QMap<QString, float> dayPluviometry;

    foreach(QString qMapKey, tempMap.keys())
    {
        QStringList tempList = tempMap.value(qMapKey);
        ///*
        dayMinTempMap.insert(qMapKey, tempList.at(1).toFloat());
        dayTempAvgMap.insert(qMapKey, tempList.at(2).toFloat());
        dayMaxTempMap.insert(qMapKey, tempList.at(3).toFloat());
        if(tempList.size() > 4)
            dayPluviometry.insert(qMapKey, tempList.at(4).toFloat());
        //*/
    }

    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap, dayPluviometry, year, completeMissingMonth);
}

QString HtmlChartMaker::generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap, QMap<QString, float> dayPluviometry)
{
    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap, dayPluviometry, 0, false);
}

QString HtmlChartMaker::generateHtmlWeekIntervalle(QMap<int, QStringList> weekList)
{
    QString dataFloraison = "";
    QString dataRecolte = "";
    QString temperatures ="";

    int i = 0;
    foreach(int qMapKey, weekList.keys())
    {
        QStringList list = weekList.value(qMapKey);

        if(0 == i)
        {
            dataFloraison.append("['semaine " + QString::number(i+1) + "',"); //Floraison
            dataFloraison.append("" + list.at(0) + "]");

            dataRecolte.append("['semaine " + QString::number(i+1) + "',"); //Récolte
            dataRecolte.append("" + list.at(1) + "]");

            temperatures.append("['semaine " + QString::number(i+1) + "',"); //Témpératures
            temperatures.append("" + list.at(2) + "]");
        }
        else
        {
            dataFloraison.append(",['semaine " + QString::number(i+1) + "',"); //Floraison
            dataFloraison.append("" + list.at(0) + "]");

            dataRecolte.append(",['semaine " + QString::number(i+1) + "',"); //Récolte
            dataRecolte.append("" + list.at(1) + "]");

            temperatures.append(",['semaine " + QString::number(i+1) + "',"); //Témpératures
            temperatures.append("" + list.at(2) + "]");
        }

        i++;
    }

    QString html = "";

    QFile mFile(":./html/highcharts-all.js");

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
           qDebug() << "could not open file for read";
    }

    QTextStream in(&mFile);
    QString libText = in.readAll();


    mFile.close();

    html.append("<html>\n");

    html.append("<div id='container' style=''></div>\n");

    html.append("<script type='text/javascript'>\n");
    html.append(libText);
    html.append("</script>\n");

    html.append("<script type='text/javascript'>\n");
    //html.append("$(function () {\n");
    html.append("chart = new Highcharts.Chart({\n");
    html.append("chart: {\n");
    html.append("    type: 'spline',\n");
    html.append("    renderTo: 'container'\n");
    html.append("},\n");
    html.append("title: {\n");
    html.append("    text: 'Intervalle TIF / Floraison / Récolte'\n");
    html.append("},\n");
    html.append("xAxis: {\n");
    html.append("   title: {\n");
    html.append("       text: 'semaine n°'\n");
    html.append("   }\n");
    html.append("},\n");
    html.append("yAxis: [\n");
    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'nombre de jours'\n");
    html.append("   },\n");
    //html.append("   min: 0\n");
    html.append("},\n");

    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'Témpérature'\n");
    html.append("   },\n");
    //html.append("   min: 0,\n");
    html.append("   opposite: true\n");
    html.append("}\n");

    html.append("],\n");
    html.append("exporting: { enabled: false },\n");

    //températures
    html.append("series: [{\n");
    html.append("name: 'Températures',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y}°C'\n");
    html.append("},\n");
    html.append("yAxis: 1,\n");
    html.append("data: [\n");
    html.append(temperatures);
    html.append("]\n");

    //récolte
    html.append("}, {\n");
    html.append("name: 'Récolte',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y} jours'\n");
    html.append("},\n");
    html.append("yAxis: 0,\n");
    html.append("data: [\n");
    html.append(dataRecolte);
    html.append("],\n");

    //floraison
    html.append("}, {\n");
    html.append("   name: 'Floraison',\n");
    html.append("   yAxis: 0,\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y} jours'\n");
    html.append("},\n");
    html.append("   data: [\n");
    html.append(dataFloraison);
    html.append("]\n");

    //End of Script

    html.append("    }]\n");
    html.append("    });\n");
    //html.append("    });\n");
    html.append("</script>\n");

    html.append("</html>\n");

    qDebug() << "HTML : " << html;
    return html;
}

QString HtmlChartMaker::generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap, QMap<QString, float> dayPluviometry, int year, bool completeMissingMonth)
{

    QMap<QString, float> monthMaxTempMap = this->calculateMonthTempAverage(dayMaxTempMap);
    QMap<QString, float> monthTempAvgMap = this->calculateMonthTempAverage(dayTempAvgMap);
    QMap<QString, float> monthMinTempMap = this->calculateMonthTempAverage(dayMinTempMap);
    QMap<QString, float> monthPluviometryMap = this->calculateMonthTempAverage(dayPluviometry);

    if(true == completeMissingMonth && 0 != year)
    for(int i = 1; i <= 12; i++)
    {
        QString newMapKey = QString::number(year);
        if(1 == newMapKey.size())
            newMapKey = "000" + newMapKey;
        QString month = QString::number(i);
        if(1 == month.length())
        {
            month = "0" + month;
        }
        newMapKey = newMapKey + month;

        if(false == monthMaxTempMap.contains(newMapKey))
        {
            monthMaxTempMap.insert(newMapKey, 0);
            monthTempAvgMap.insert(newMapKey, 0);
            monthMinTempMap.insert(newMapKey, 0);
            monthPluviometryMap.insert(newMapKey, 0);
        }
    }

    QString avgValues = "";
    int i = 0;
    foreach(QString qMapKey, monthTempAvgMap.keys())
    {
        QString qsAvg;
        qsAvg = qsAvg.setNum(monthTempAvgMap.value(qMapKey), 'f', 2);
        if(i == 0)
        {
            avgValues.append("[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            avgValues.append(qsAvg + "]");
        }
        else
        {
            avgValues.append(",[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            avgValues.append(qsAvg + "]");
        }
        i++;
    }

    QString maxValues = "";
    i = 0;
    foreach(QString qMapKey, monthMaxTempMap.keys())
    {
        QString qsMax;
        qsMax = qsMax.setNum(monthMaxTempMap.value(qMapKey), 'f', 2);

        if(i == 0)
        {
            maxValues.append("[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            maxValues.append(qsMax + "]");
        }
        else
        {
            maxValues.append(",[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            maxValues.append(qsMax + "]");
        }
        i++;
    }

    QString minValues = "";
    i = 0;
    foreach(QString qMapKey, monthMinTempMap.keys())
    {
        QString qsMin;
        qsMin = qsMin.setNum(monthMinTempMap.value(qMapKey), 'f', 2);

        if(i == 0)
        {
            minValues.append("[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            minValues.append(qsMin + "]");
        }
        else
        {
            minValues.append(",[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            minValues.append(qsMin + "]");
        }
        i++;
    }

    QString pluvValues = "";
    i = 0;
    foreach(QString qMapKey, monthPluviometryMap.keys())
    {
        QString qsPluv;
        qsPluv = qsPluv.setNum(monthPluviometryMap.value(qMapKey), 'f', 2);

        if(i == 0)
        {
            pluvValues.append("[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            pluvValues.append(qsPluv + "]");
        }
        else
        {
            pluvValues.append(",[Date.UTC(" + qMapKey.mid(0,4) + "," + QString::number(qMapKey.mid(4,2).toInt()-1) + ",1),");
            pluvValues.append(qsPluv + "]");
        }
        i++;
    }


    QString html = "";

    QFile mFile(":./html/highcharts-all.js");

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
           qDebug() << "could not open file for read";
    }

    QTextStream in(&mFile);
    QString libText = in.readAll();


    mFile.close();

    html.append("<html>\n");

    html.append("<div id='container' style=''></div>\n");

    html.append("<script type='text/javascript'>\n");
    html.append(libText);
    html.append("</script>\n");

    html.append("<script type='text/javascript'>\n");
    html.append("chart = new Highcharts.Chart({\n");
    html.append("chart: {\n");
    html.append("    type: 'spline',\n");
    html.append("    renderTo: 'container'\n");
    html.append("},\n");
    html.append("title: {\n");
    html.append("    text: 'Températures Moyennes, maximales et minimales'\n");
    html.append("},\n");

    html.append("xAxis: {\n");
    html.append("type: 'datetime',");
    html.append("        dateTimeLabelFormats: { // don't display the dummy year");
    html.append("month: '%e. %b',");
    html.append("year: '%b'");
    html.append("},");

    html.append("   title: {\n");
    html.append("       text: 'Date'\n");
    html.append("   }\n");
    html.append("},\n");
    html.append("yAxis: [\n");
    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'Température (°C)'\n");
    html.append("   },\n");
    html.append("   min: 0\n");
    html.append("},\n");

    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'Pluviométrie'\n");
    html.append("   },\n");
    html.append("   min: 0,\n");
    html.append("   opposite: true\n");
    html.append("}\n");

    html.append("],\n");
    html.append("exporting: { enabled: false },\n");


    html.append("series: [{\n");

    //pluviométrie
    html.append("name: 'Pluviométrie',\n");
    html.append("type: 'column',\n");
    html.append("   color: '#8EA4FF',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: '{point.x:%b}: {point.y}'\n");
    html.append("},\n");
    html.append("yAxis: 1,\n");
    html.append("data: [\n");
    html.append(pluvValues);
    html.append("]\n");


    //T°C moyenne
    html.append("}, {\n");
    html.append("name: 'T°C Moyenne',\n");
    html.append("   color: '#8E8E8E',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: '{point.x:%b}: {point.y} °C'\n");
    html.append("},\n");
    html.append("yAxis: 0,\n");
    html.append("data: [\n");
    html.append(avgValues);
    html.append("]\n");


    //T°C maximale
    html.append("}, {\n");
    html.append("name: 'T°C Maximale',\n");
    html.append("   color: '#FE2E2E',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: '{point.x:%b}: {point.y} °C'\n");
    html.append("},\n");
    html.append("yAxis: 0,\n");
    html.append("data: [\n");
    html.append(maxValues);
    html.append("]\n");

    //T°C minimale
    html.append("}, {\n");
    html.append("   name: 'T°C Minimale',\n");
    html.append("   yAxis: 0,\n");
    html.append("   color: '#2E64FE',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: '{point.x:%b}: {point.y} °C'\n");
    html.append("},\n");
    html.append("   data: [\n");
    html.append(minValues);
    html.append("]\n");

    //End of Script

    html.append("    }]\n");
    html.append("    });\n");
    html.append("</script>\n");

    html.append("</html>\n");

    qDebug() << "HTML : " << html;
    return html;
}
