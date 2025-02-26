#ifndef GRAPH_H
#define GRAPH_H

#include <QDialog>
#include <qwt_plot.h>

#include <qwt_plot_grid.h>

#include <qwt_legend.h>

#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <qwt_plot_magnifier.h>

#include <qwt_plot_panner.h>

#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <QToolBar>
#include <QBoxLayout>
#define LOG_DIR_PATH "/dump"
#include "oscope_old_ver.h"
#include <curvetree.h>
#include <curvetreedyno.h>
#include <dumpmemdata.h>
#include "dumpdataxlsx.h"
#include "curve.h"



namespace Ui {
class Graph;
}

class Graph : public QDialog
{
    Q_OBJECT

public:
    explicit Graph(QWidget *parent = nullptr);
    ~Graph();
public slots:
    void loadingIdealPoints();
public:
    QPolygonF points;
    QwtPlotCurve *curve;
    QwtPlot *a_plot;
private:
    Ui::Graph *ui;
//    QwtPlot *a_plot;
    QwtPlotGrid *grid;
//    QwtPlotCurve *curve;
    QwtSymbol *symbol;
 //   QPolygonF points;
    QToolBar *toolBar;
    QBoxLayout *layoutToolBar;
    QBoxLayout *layoutPlot;
    CurveTree *tree;
    CurveTreeDyno *tree_dyno;
    DumpDataXlsx *dump_data;
    double pointIdealArray[26][2];
    void openFile();
    void openFileDyno();
    void setPlot();
    void setPlotGrid();
    void setCurveParameters();
    void setToolBar();
    Osc *m_osc;
    //MemData *m_mem_data;
    DumpMemData *m_dump_mem_data;
    Curve *curve_paint;
    //OFileSource *m_ofs;
    OFileSource m_src;
    OFileSourceDump m_src_dump;
    OFileSourceDyno *m_src_d;
    QAction *m_actFileOpen;
    QAction *m_actFileClose;
    void opened();
    void closed();    
signals:
    void ToMainWindow();
    void changePointsSignal();
    void signalCurvePeriod(int value);
private slots:
    void on_pushButton_back_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_spinBox_valueChanged(int arg1);
    void on_pushButton_dumpExcel_clicked();
    void on_pushButton_dyno_clicked();
    void on_checkBox_toggled(bool checked);
protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // GRAPH_H
