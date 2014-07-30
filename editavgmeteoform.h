#ifndef EDITAVGMETEOFORM_H
#define EDITAVGMETEOFORM_H

#include <QWidget>
#include <QString>

namespace Ui {
class editAvgMeteoForm;
}

class editAvgMeteoForm : public QWidget
{
    Q_OBJECT

public:
    explicit editAvgMeteoForm(QWidget *parent = 0);
    ~editAvgMeteoForm();
    void setEditSiteNameTextEditText(QString text);

private:
    Ui::editAvgMeteoForm *ui;
};

#endif // EDITAVGMETEOFORM_H
