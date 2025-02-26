#ifndef BALANCINGMOMENT_H
#define BALANCINGMOMENT_H
#include "storage.h"

    class BalancingMoment {
        private:
             double vD, mUr, vDisb, m_rad, mRot, m;
//             QString st;
        public:
            void setData(double m_bm, Storage *storage);
            double getDataDisb(Storage *storage);
            double getDataMoment(Storage *storage);
    };

#endif // BALANCINGMOMENT_H
