#ifndef CURVETREE_H
#define CURVETREE_H

#include <QTreeWidget>
#include "oscope_old_ver.h"
#include <QList>
#include <QSpinBox>
#include <QLabel>
#include <abstractdata.h>

class CurveTree : public QTreeWidget
{
    Q_OBJECT
public:
typedef struct {
    double min_y_1;
    double min_x_1;
    double min_y_2;
    double min_x_2;
    double max_x_1;
    double max_y_1;
    double max_x_2;
    double max_y_2;
} CurveData;
    explicit CurveTree(QWidget *parent = 0);
    enum Columns {SHOW, NAME, POINT, OFFSET, APPROX, MIN_1, MAX_1, MIN_2, MAX_2, T_0, T_n};
    QList<CurveData> listCurveData;
private:
    QList<Osc::NewPointCurve> *listNewPointCurves;
    QVector<QVector<AbstractData*>> result;
signals:
    void curveChanged(int index, bool show);
    void curveOffsetChanged(int index, double offset);
    void signalCurveApprox(int index, bool approx);
public slots:
//    void setCurves(QList <Osc::NewPointCurve> *listNewPointCurves);
    void getCurves(QVector<QVector<AbstractData*>> result);
private:
    void fillTree();
    //void updateItem(QTreeWidgetItem *item, Osc::NewPointCurve *listNewPointCurves);
    //void updateItem(QTreeWidgetItem *item, QVector<Fragment> *res_frag, int i);
    void updateItem(QTreeWidgetItem *item, int i);
private slots:
    void curveStateChanged(bool show);
//    void curveOffset(double offset);
    void curveApprox(bool approx);
//    void getData(double min_y1, double min_x1, double min_y2, double min_x2,
//                  double max_y1, double max_x1, double max_y2, double max_x2);
};

#endif // CURVETREE_H
