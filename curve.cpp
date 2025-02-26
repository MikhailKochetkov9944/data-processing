#include "curve.h"
#include <QDebug>
#include <datapaint.h>
#include <qwt_text.h>
#include "qwtpaintfrag.h"

void Curve::curveResult(AbstractData* data) {
    if (!curvePeriod) curvePeriod = 0;
    for(int i=0; i<curvePeriod; i++) {
        if(i==0) result.append(AbstractData::finder(data));
        else result.append(AbstractData::finder(result[i-1].first()));
    }
    for(int i=0; i<result.size(); i++) {
        QPolygonF polygon_points;
        QwtPlotCurve *curve_plot = new QwtPlotCurve();
        curve_plot->setPen(colorRender());
        curve_plot->setRenderHint(QwtPlotItem::RenderAntialiased, true); // сглаживание
        curve_plot->setTitle("Curve_"+QString::number(i+1));
        DataPaint* curve_sample = new DataPaint(*result[i][5], result[i][5]->ord(result[i][5]->size()));
        curve_plot->setData(curve_sample);
        //curve_plot->setSamples(polygon_points);
        curve_plot->setVisible(false);
        curve_plot->attach(b_plot);
    }
    curveFragApprox();
    b_plot->replot();
    emit setCurves(result);
}

void Curve::curveFragApprox() {
    DataPaint* curve_sample;
    //QwtPaintFrag* curve_sample;
    for(int index=0; index<result.size(); index++) {
        for(int i=1; i<5; i++) {
            //PolyFragment* poly_f = new PolyFragment(result[index][i], 3);
            QwtPlotCurve *curve_plot = new QwtPlotCurve();
            result[index].append(new PolyFragment(result[index][i], 3));
            curve_sample = new DataPaint(*result[index][5+i], result[index][5]->ord(0));
            //curve_sample = new QwtPaintFrag(*result[index]);
            curve_plot->setTitle("CurveFrag_"+QString::number(i+4*(index+1)));
            qDebug()<<"finde_curve()"<<curve_plot->title().text();
            curve_plot->setRenderHint(QwtPlotItem::RenderAntialiased, true);
            curve_plot->setPen(colorRender());
            curve_plot->setData(curve_sample);
            curve_plot->setVisible(false);
            curve_plot->attach(b_plot);
        }
    }
    b_plot->replot();
}

void Curve::updateCurve(int index, bool show) {
   QwtPlotItemList curve=b_plot->itemList(QwtPlotItem::Rtti_PlotCurve);
   QwtPlotCurve* b_curve = nullptr;
   for(QwtPlotItemIterator it=curve.begin(); it != curve.end(); it++) {
       QwtPlotItem* item = *it;
       if(item->title().text().remove("Curve_").toInt() == index) b_curve = (QwtPlotCurve*)item;
   }
   if(b_curve) {
        b_curve->setVisible(show);
        b_curve->attach(b_plot);
        b_plot->replot();
   }
}

void Curve::updateCurvePoly(int index, bool show) {
    QwtPlotCurve* curve_plot_frag = new QwtPlotCurve();
    for(int i=1; i<5; i++) {
        curve_plot_frag = findeCurveOnPlot(i+4*index, "CurveFrag_");
        curve_plot_frag->setVisible(show);
        curve_plot_frag->attach(b_plot);
    }
    b_plot->replot();
}

void Curve::attach(QwtPlot *plot) {
    b_plot=plot;
    if (b_plot)  b_plot ->replot();
}

int Curve::setCurvePeriod(int value) {
    return curvePeriod = value;
}



QColor Curve::colorRender() {
    QColor colors[] = { Qt::cyan, Qt::magenta, Qt::red,
                        Qt::darkRed, Qt::darkCyan, Qt::darkMagenta,
                        Qt::darkGreen, Qt::darkGreen, Qt::darkGray,
                        Qt::darkBlue, Qt::black, Qt::darkYellow  };
    return colors[qrand() % 10];
}

QwtPlotCurve* Curve::findeCurveOnPlot(int index, QString base_name_curve) {
    QwtPlotCurve* qwtPlotCurve = nullptr;
    QwtPlotItemList curve=b_plot->itemList(QwtPlotItem::Rtti_PlotCurve);
    for(QwtPlotItemIterator it=curve.begin(); it != curve.end(); it++) {
        QwtPlotItem* item = *it;
        if(item->title().text().remove(base_name_curve).toInt() == index) qwtPlotCurve = (QwtPlotCurve*)item;
    }
    return qwtPlotCurve;
}

bool Curve::existCurveOnPlot(int index, QString base_name_curve) {
    QwtPlotItemList curve=b_plot->itemList(QwtPlotItem::Rtti_PlotCurve);
    for(QwtPlotItemIterator it=curve.begin(); it != curve.end(); it++) {
        QwtPlotItem* item = *it;
        if(item->title().text().remove(base_name_curve).toInt() == index) return true;
    }
    return false;
}

void Curve::createDumpXlsx() {
    emit signalDumpXlsx(result);
}
