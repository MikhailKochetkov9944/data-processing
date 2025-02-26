#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"
#include <stdlib.h>
#include <string>
#include "balancingmoment.h"
#include "torque.h"

#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QSqlQuery>
#include <QTableView>
#include <QtCore/qmath.h>
#include "storage.h"
#include "parameter.h"


//Storage storage;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    parameter = new Parameter();
    connect(parameter, &Parameter :: ToMainWindow, this, &MainWindow :: show);
    graph = new Graph();
    connect(graph, &Graph::ToMainWindow, this, &MainWindow::show);

    // db
    db = QSqlDatabase :: addDatabase("QSQLITE");
    db.setDatabaseName("./paramDB.db");
    if(db.open()) {
        qDebug("open");
    } else {
        qDebug("error. No open");
    }

    storage = new Storage();
    connect(parameter, SIGNAL(signalF(double)), storage, SLOT(setF_new(double)));
    connect(parameter, SIGNAL(signalVd(double)), storage, SLOT(setVd_new(double)));
    connect(parameter, SIGNAL(signalMur(double)), storage, SLOT(setMur_new(double)));
    connect(parameter, SIGNAL(signalA(double)), storage, SLOT(setA_new(double)));
    connect(parameter, SIGNAL(signalC(double)), storage, SLOT(setC_new(double)));
    connect(parameter, SIGNAL(signalP(double)), storage, SLOT(setP_new(double)));
    connect(parameter, SIGNAL(signalK(double)), storage, SLOT(setK_new(double)));
    connect(parameter, SIGNAL(signalR(double)), storage, SLOT(setR_new(double)));
    connect(parameter, SIGNAL(signalFi(double)), storage, SLOT(setFi_new(double)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
        this->close();
        parameter->show();
}

void MainWindow::on_pushButton_3_clicked()
{

    QString m = ui -> lineEdit -> text(); 
    if(!m.isEmpty() && m.toInt()%15==0) {
    ui -> label_4 -> setText(m);
    double m_d = m.toDouble();
    //Для проверки
      double tN_n;
    QSqlQuery query("SELECT tN FROM param WHERE id=1");
        while (query.next()) {
            tN_n = query.value(0).toDouble();
        }     ui -> label_9 -> setText(QString::number(tN_n));

        // Расчет Mрот и дисбаланса конструкции
        BalancingMoment bm;
        bm.setData(m_d, storage);
        double mRot = bm.getDataMoment(storage);
        ui -> label_5 -> setText(QString::number(mRot));
        Torque tr;
        tr.setData(storage);
        double tN = tr.getDataTn(m_d);
        ui -> label_10 -> setText(QString::number(tN));

        QSqlQuery a_query;
        a_query.prepare("UPDATE param SET mRot = :mRot, tN = :tN WHERE angle = :v");
        a_query.bindValue(":mRot", mRot);
        a_query.bindValue(":tN", tN);
        a_query.bindValue(":v", ui -> lineEdit -> text());
        a_query.exec();
    } else return;
  }




void MainWindow::on_pushButton_4_clicked()
{
    this->close();
    graph->show();
}
