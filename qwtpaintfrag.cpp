#include "qwtpaintfrag.h"

//QwtPaintFrag::QwtPaintFrag(const QVector<AbstractData> &_res_frag) : res_frag(_res_frag)
//{
//    cache_count = res_frag.size();
//    x_end = res_frag[5].ord(cache_count);
//    y_end = res_frag[5].val(cache_count);
//}

//QPointF QwtPaintFrag::sample(size_t i) const {
//    if(i == cache_count) {

//    }
//    return QPointF(res_frag[5].ord(i),
//                   res_frag[5].val(i));
//};

//QRectF QwtPaintFrag::boundingRect() const
//{
//#if QWT_VERSION >= 0x060200
//  if ( cachedBoundingRect.width() < 0.0 )
//     cachedBoundingRect = qwtBoundingRect(*this);
//  return cachedBoundingRect;
//#else
//  if ( d_boundingRect.width() < 0.0 )
//     d_boundingRect = qwtBoundingRect(*this);
//  return d_boundingRect;
//#endif
//}
