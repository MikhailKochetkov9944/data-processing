#ifndef QWTPAINT_H
#define QWTPAINT_H

#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_symbol.h"
#include "qwt_plot_curve.h"
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <abstractdata.h>


class DataPaint : public QwtSeriesData<QPointF>
{
public:
    //QwtPaint(const QVector<QVector<Fragment>> &_result);
    DataPaint(const AbstractData &_res_frag, float _offset);
    size_t size() const override {return cache_count; }
    QPointF sample(size_t i) const override;
    QRectF boundingRect() const override;
private:
    int cache_count;
    //QVector<QVector<Fragment>> result;
    const AbstractData& res_frag;
    float offset;
    //AbstarctData&  dataref;
};

#endif // QWTPAINT_H
