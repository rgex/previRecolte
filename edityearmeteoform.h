#ifndef EDITYEARMETEOFORM_H
#define EDITYEARMETEOFORM_H

#include <QWidget>

namespace Ui {
class editYearMeteoForm;
}

class editYearMeteoForm : public QWidget
{
    Q_OBJECT

public:
    explicit editYearMeteoForm(QWidget *parent = 0);
    ~editYearMeteoForm();
    void setWebViewHtml(QString html);

private:
    Ui::editYearMeteoForm *ui;
};

#endif // EDITYEARMETEOFORM_H
