#include "balancingmoment.h"
#include "math.h"
#include <QtCore/qmath.h>
#include <string>




    void BalancingMoment::setData(double m_bm, Storage *storage) {
        m = m_bm;
       // Storage &st = storage;
        if(storage->getVd() != storage->getVd_new()) {
            getDataDisb(storage);
            storage -> setVd();
        }
        mUr = storage -> getMur();
    }
    double BalancingMoment::getDataDisb(Storage *storage) {
        //Дисбаланс конструкции
        vD = storage -> getVd();
        double vDisb = 9.8*vD;
        storage->setVdisb(vDisb);
        return vDisb;
    }
    double BalancingMoment::getDataMoment(Storage *storage) {
        //Момент роторного уравновешивания(Мрот)
        m_rad = m*M_PI/180;
        mRot = mUr*qSin(m_rad)/1000;
        if(mRot<0.1 && mRot>0 || mRot>-2 && mRot<0) mRot =0;
        storage->setMrot(mRot);
        return mRot;

    }

