#include "parameter.h"
#include "ui_parameter.h"
#include <QtSql>
#include <QDebug>
#include <QSqlQuery>
#include "mainwindow.h"
#include "storage.h"
#include <QObject>
#include "viewparam.h"



Storage storageForm;
Parameter::Parameter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Parameter)

{

    ui->setupUi(this);
    ViewParam viewParam;
    viewParam.view(ui);
}

Parameter::~Parameter()
{
    delete ui;
}


void Parameter::on_pushButton_f_clicked()
{
    QString f = ui -> lineEdit_f -> text();
    ui -> label_ui_f -> setText(f);
    emit signalF(f.toDouble());
}

void Parameter::on_pushButton_vd_clicked()
{
    QString vD = ui -> lineEdit_vd -> text();
    ui -> label_ui_vd -> setText(vD);
    emit signalVd(vD.toDouble());
}

void Parameter::on_pushButton_m_ur_clicked()
{
   QString mUr = ui -> lineEdit_m_ur -> text();
   ui -> label_ui_m_ur -> setText(mUr);
   emit signalMur(mUr.toDouble());
}

void Parameter::on_pushButton_clicked()
{
    emit ToMainWindow();
    this -> close();
}

void Parameter::on_pushButton_a_clicked()
{
    QString a = ui -> lineEdit_a -> text();
    ui -> label_ui_a -> setText(a);
    emit signalA(a.toDouble());
}

void Parameter::on_pushButton_c_clicked()
{
    QString c = ui -> lineEdit_c -> text();
    ui -> label_ui_c -> setText(c);
    emit signalC(c.toDouble());
}

void Parameter::on_pushButton_p_clicked()
{
    QString p = ui -> lineEdit_p -> text();
    ui -> label_ui_p -> setText(p);
    emit signalP(p.toDouble());
}

void Parameter::on_pushButton_k_clicked()
{
    QString k = ui -> lineEdit_k -> text();
    ui -> label_ui_k -> setText(k);
    emit signalK(k.toDouble());
}

void Parameter::on_pushButton_r_clicked()
{
    QString r = ui -> lineEdit_r -> text();
    ui -> label_ui_r -> setText(r);
    emit signalR(r.toDouble());
}

void Parameter::on_pushButton_fi_clicked()
{
    QString fi = ui -> lineEdit_fi -> text();
    ui -> label_ui_fi -> setText(fi);
    emit signalFi(fi.toDouble());
}
