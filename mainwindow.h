#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parameter.h"
#include "graph.h"

#include "storage.h"
#include <profile.h>

//#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QSqlQuery>
#include "oscope_old_ver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    Parameter *parameter;
    Graph *graph;
    QSqlDatabase db;
    QSqlQuery *query;
    Storage *storage;
};
#endif // MAINWINDOW_H
