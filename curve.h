#ifndef CURVE_H
#define CURVE_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <abstractdata.h>

class Curve : public QObject {
      Q_OBJECT
public:
    Curve(QObject *parent = 0) {};
    ~Curve() {};
    void curveResult(AbstractData* data);
    void curveFragApprox();
    QVector<QVector<AbstractData*>> result;
    QwtPlot *b_plot;
    QVector<PolyFragment> poly;
    void attach(QwtPlot *plot);
    QColor colorRender();
    QwtPlotCurve* findeCurveOnPlot(int index, QString base_name_curve);
    bool existCurveOnPlot(int index, QString base_name_curve);
signals:
    void setCurves(QVector<QVector<AbstractData*>> result);
    void signalDumpXlsx(QVector<QVector<AbstractData*>> result);
public slots:
    void updateCurve(int index, bool show);
    int setCurvePeriod(int value);
    void updateCurvePoly(int index, bool show);
    void createDumpXlsx();
private:
    int curvePeriod;
};

#endif // CURVE_H
