#include "datapaint.h"


DataPaint::DataPaint(const AbstractData &_res_frag, float _offset) : res_frag(_res_frag)
{
    offset = _offset;
    cache_count = res_frag.size();
}

QPointF DataPaint::sample(size_t i) const {
    return QPointF(res_frag.ord(i) - offset, // - res_frag.ord(res_frag.size()),
                   res_frag.val(i));
};

QRectF DataPaint::boundingRect() const
{
#if QWT_VERSION >= 0x060200
  if ( cachedBoundingRect.width() < 0.0 )
     cachedBoundingRect = qwtBoundingRect(*this);
  return cachedBoundingRect;
#else
  if ( d_boundingRect.width() < 0.0 )
     d_boundingRect = qwtBoundingRect(*this);
  return d_boundingRect;
#endif
}
