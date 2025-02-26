#ifndef PARAMETER_H
#define PARAMETER_H

#include <QDialog>
#include "storage.h"


namespace Ui {
class Parameter;
}

class Parameter : public QDialog
{
    Q_OBJECT

public:
    explicit Parameter(QWidget *parent = nullptr);
    ~Parameter();

private slots:
    void on_pushButton_f_clicked();

    void on_pushButton_vd_clicked();

    void on_pushButton_m_ur_clicked();

    void on_pushButton_clicked();

    void on_pushButton_a_clicked();

    void on_pushButton_c_clicked();

    void on_pushButton_p_clicked();

    void on_pushButton_k_clicked();

    void on_pushButton_r_clicked();

    void on_pushButton_fi_clicked();

private:
    Ui::Parameter *ui;
    Storage *st;

signals:
    void ToMainWindow();
    void signalF(double);
    void signalVd(double);
    void signalMur(double);
    void signalA(double);
    void signalC(double);
    void signalP(double);
    void signalK(double);
    void signalR(double);
    void signalFi(double);

};

#endif // PARAMETER_H
