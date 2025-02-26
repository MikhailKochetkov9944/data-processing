#ifndef CURVETREEDYNO_H
#define CURVETREEDYNO_H

#include <QTreeWidget>
#include <qwt_plot_curve.h>
#include "oscope_old_ver.h"

class CurveTreeDyno : public QTreeWidget
{
    Q_OBJECT
public:
    typedef struct
    {
      QString date;
      QVector<double> x_dyno;
      QVector<double> y_dyno;
      QwtPlotCurve *dyno_plot_curve;
      bool show;
      int id;
      bool show_recount;
      QwtPlotCurve *dyno_plot_curve_recount;
      QVector<double> x_dyno_rec;
      QVector<double> y_dyno_rec;
      int point;
    } DynoCurve;
    explicit CurveTreeDyno(QWidget *parent = 0);
    enum Columns {SHOW, NAME, RECOUNT};
    void setCurveDyno(QList<OFileSourceDyno::DynoCurve> dynoCurveList);
    void fillTree();
    void updateItem(QTreeWidgetItem *item, OFileSourceDyno::DynoCurve *dynoCurve);
private:
    QList<OFileSourceDyno::DynoCurve> dynoCurveList;
signals:
    void curveChangedDyno(int index, bool show);
    void curveChangedDynoRecount(int index, bool show);
public slots:
    void curveStateChanged(bool);
    void curveStateRecount(bool);
};

#endif // CURVETREEDYNO_H
