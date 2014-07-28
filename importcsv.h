#include <QObject>
#include <QDebug>

#ifndef IMPORTCSV_H
#define IMPORTCSV_H

class ImportCsv : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void importFile();
};
#endif // IMPORTCSV_H
