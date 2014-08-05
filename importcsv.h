#include <QObject>
#include <QDebug>

#ifndef IMPORTCSV_H
#define IMPORTCSV_H

class ImportCsv
{

public:
    ImportCsv();
    QList<QStringList> importFile(QString fileName);
};
#endif // IMPORTCSV_H
