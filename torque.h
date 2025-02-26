#ifndef TORQUE_H
#define TORQUE_H
#include "storage.h"
class Torque {
    private:
         double vDisb, mRot, m, a, c, p, k, r, fi, f, tf, tN, b, j, x, ro, psi, alp;
         double cpk, cp, m_fi, b_cos, cc, pp, cz, param_degree, param_rad;
    public:

        void setData(Storage *storage);
        void setDataNew(Storage *storage);
        double getDataTn(double m_tr);
        double getDataTest();
};
#endif // TORQUE_H
