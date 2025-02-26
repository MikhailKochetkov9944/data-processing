#include "storage.h"
#include "math.h"
#include <QtCore/qmath.h>


void Storage::setVd() {
    if(vD_new!=0) {
     vD = vD_new;
    }
}
void Storage::setMur() {
    if(mUr_new!=0) {
     mUr = mUr_new;
    }
}
void Storage::setA() {
    if(a_new!=0) {
     a = a_new;
    }
}
void Storage::setC() {
    if(c_new!=0) {
     c = c_new;
    }
}
void Storage::setP() {
    if(p_new!=0) {
     p = p_new;
    }
}
void Storage::setK() {
    if(k_new!=0) {
     k = k_new;
    }
}
void Storage::setR() {
    if(r_new!=0) {
     r = r_new;
    }
}
void Storage::setFi() {
     if(fi_new!=0) {
        fi = fi_new;
    }
}
void Storage::setF() {
    if(f_new!=0) {
     f = f_new;
    }
}
void Storage::setVdisb(double vDisb_new) {
    vDisb = vDisb_new;
}
void Storage::setMrot(double mRot_new) {
    mRot = mRot_new;
}
void Storage::setVd_new(double vD_di) {
    if(vD_di != vD_new) {
        vD_new = vD_di;
        emit valueChangedVd(vD_di);
    }
}

void Storage::setMur_new(double mUr_di) {
    if(mUr_di != mUr_new) {
        mUr_new = mUr_di;
        emit valueChangedMur(mUr_di);
    }
}
void Storage::setA_new(double a_di) {
    if(a_di != a_new) {
        a_new = a_di;
        emit valueChangedA(a_di);
    }
}
void Storage::setC_new(double c_di) {
    if(c_di != c_new) {
        c_new = c_di;
        emit valueChangedC(c_di);
    }
}
void Storage::setP_new(double p_di) {
    if(p_di != p_new) {
        p_new = p_di;
        emit valueChangedP(p_di);
    }
}
void Storage::setK_new(double k_di) {
    if(k_di != k_new) {
        k_new = k_di;
        emit valueChangedK(k_di);
    }
}
void Storage::setR_new(double r_di) {
    if(r_di != r_new) {
        r_new = r_di;
        emit valueChangedR(r_di);
    }
}
void Storage::setFi_new(double fi_di) {
    if(fi_di != fi_new) {
        fi_new = fi_di;
        emit valueChangedFi(fi_di);
    }
}
void Storage::setF_new(double f_di) {
    if(f_di != f_new) {
        f_new = f_di;
        emit valueChangedF(f_di);
    }
}
double Storage::getVd() {
    return vD;
}
double Storage::getMur() {
    return mUr;
}
double Storage::getA() {
    return a;
}
double Storage::getC() {
    return c;
}
double Storage::getP() {
    return p;
}
double Storage::getK() {
    return k;
}
double Storage::getR() {
    return r;
}
double Storage::getFi() {
    return fi;
}
double Storage::getF() {
    return f;
}
double Storage::getVdisb() {
    return vDisb;
}
double Storage::getMrot() {
    return mRot;
}
double Storage::getVd_new() {
    return vD_new;
}
double Storage::getMur_new() {
    return mUr_new;
}
double Storage::getA_new() {
    return a_new;
}
double Storage::getC_new() {
    return c_new;
}
double Storage::getP_new() {
    return p_new;
}
double Storage::getK_new() {
    return k_new;
}
double Storage::getR_new() {
    return r_new;
}
double Storage::getFi_new() {
    return fi_new;
}
double Storage::getF_new() {
    return f_new;
}
