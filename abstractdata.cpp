#include "abstractdata.h"
#include <QDebug>

int AbstractData::getIndexMinVal(int ord_start, int ord_end) {
    int i=ord_start;
    double val_min = this->val(i);
    double ord_min;
    for(; i<ord_end; i++) {
        if(val_min > this->val(i)) {
            ord_min = i;
        }
    }
    return ord_min;
}

int AbstractData::getIndexMaxVal(int ord_start, int ord_end) {
    int i=ord_start;
    double val_max=this->val(i);
    double ord_max;
        for(; i<ord_end; i++) {
            if(val_max<this->val(i)) {
                ord_max= i;
            }
        }
        return ord_max;
}

bool AbstractData::checkValidOrd(int ord_start, int ord_end) {
    int i=ord_start;
    float ord = this->ord(i);
    i++;
    for(; i<ord_end; i++) {
      float next_ord = this->ord(i);
      if (next_ord<ord) return false;
      ord=next_ord;
    }
    return true;
}

QVector<AbstractData*> AbstractData::finder(AbstractData* curve_base) {
    int n=0;
    QVector<AbstractData*> res_frag;
    QVector<int> p_intersection;
    for(int i=1; i<curve_base->size(); i++) {
        if(curve_base->val(i-1)*curve_base->val(i+1)<= 0) { // поиск начала периода
           if((curve_base->val(i)*curve_base->val(i+1))<= 0 && (curve_base->val(i-1)*curve_base->val(i+1))<0) {
               n++;
               p_intersection.append(i);
               if(n==3) res_frag.append(new Fragment(curve_base, curve_base->getIndexMinVal(p_intersection[0], p_intersection[1]), curve_base->getIndexMaxVal(p_intersection[1], p_intersection[2])));
               if(n==4) res_frag.append(new Fragment(curve_base, curve_base->getIndexMaxVal(p_intersection[1], p_intersection[2]), curve_base->getIndexMinVal(p_intersection[2], p_intersection[3])));
               if(n==5) res_frag.append(new Fragment(curve_base, curve_base->getIndexMinVal(p_intersection[2], p_intersection[3]), curve_base->getIndexMaxVal(p_intersection[3], p_intersection[4])));
           }
           if(n == 7) { // последняя точка в периоде
               res_frag.append(new Fragment(curve_base, curve_base->getIndexMaxVal(p_intersection[3], p_intersection[4]), curve_base->getIndexMinVal(p_intersection[4], p_intersection[5])));
               res_frag.append(new Fragment(curve_base, curve_base->getIndexMinVal(p_intersection[0], p_intersection[1]), curve_base->getIndexMinVal(p_intersection[4], p_intersection[5])));
               res_frag.insert(0, new Fragment(curve_base, (curve_base->getIndexMaxVal(p_intersection[3], p_intersection[4])), curve_base->size()));
               return res_frag;
           }
        }
    }
}
