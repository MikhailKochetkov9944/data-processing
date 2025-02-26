#ifndef QWTPAINTFRAG_H
#define QWTPAINTFRAG_H

#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_symbol.h"
#include "qwt_plot_curve.h"
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <abstractdata.h>

//class QwtPaintFrag : public QwtSeriesData<QPointF>
//{
//public:
//    QwtPaintFrag(const QVector<AbstractData> &_res_frag);
//    size_t size() const override {return cache_count; }
//    QPointF sample(size_t i) const override;
//    QRectF boundingRect() const override;
//private:
//    int cache_count;
//    //const AbstractData& res_frag;
//    const QVector<AbstractData>& res_frag;
//    double x_end;
//    double y_end;
//};

#endif // QWTPAINTFRAG_H
