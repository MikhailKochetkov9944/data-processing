#include "torque.h"
#include "math.h"
#include <QtCore/qmath.h>
#include <string>

void Torque::setData(Storage *storage) {
  if(a==0||c==0||p==0||k==0||r==0||fi==0||f==0) {
    vDisb = storage->getVdisb();
    mRot = storage->getMrot();
    a = storage->getA();
    c = storage->getC();
    p = storage->getP();
    k = storage->getK();
    r = storage->getR();
    fi = storage->getFi();
    f = storage->getF();
  }
  if(storage->getC()!=storage->getC_new() || storage->getC_new()==0 || storage->getP()!=storage->getP()||
          storage->getP_new()==0 ||storage->getK()!=storage->getK_new()|| storage->getK_new()==0||
          storage->getR()!=storage->getR_new()||storage->getR_new()==0) {
   setDataNew(storage);
   param_degree = 180.0/M_PI;
   param_rad = M_PI/180;
   cpk = c*c+p*p-k*k-r*r;
   cp = 2*c*p;
   cc = c*c;
   pp = p*p;
   cz = 2*c;
}

}
void Torque::setDataNew(Storage *storage) {
    vDisb = storage->getVdisb();
    mRot = storage->getMrot();
    storage->setA();
    storage->setC();
    storage->setP();
    storage->setK();
    storage->setR();
    storage->setFi();
    storage->setF();
    a = storage->getA();
    c = storage->getC();
    p = storage->getP();
    k = storage->getK();
    r = storage->getR();
    fi = storage->getFi();
    f = storage->getF();
}
double Torque::getDataTn(double m_tr) {
    m = m_tr;
    //Эффективный крутящий момент с кривошипом(Tн)
    // double a=2500.0, c=2500.0, p = 3000.0, k=3752.0, r = 1200.0, fi=35.8, tN, tf, f = 88.0747;
    // Расчет угла b, радианы
    // b = 180/M_PI*(qAcos((c*c+p*p-k*k-r*r+2*k*r*qCos((m-fi)*M_PI/180))/(2*c*p)));
     m_fi = qCos((m-fi)*param_rad);
     b = qAcos((cpk+2*k*r*m_fi)/cp);
    //Расчет j, мм
    //j = qSqrt(c*c-(2*c*p*qCos(b*M_PI/180.0))+p*p);
     b_cos = qCos(b);
     j = qSqrt(cc-(cp*b_cos)+pp);
    //Расчет угла x, град
    //x = qAcos((c*c+j*j-p*p)/(2*c*j))*180.0/M_PI;
     x = qAcos((cc+j*j-pp)/(cz*j))*param_degree;
    //Расчет угла ro, град
     //ro = qAsin((r*qSin((m-fi)*M_PI/180))/j)*180/M_PI;
     ro = qAsin((r*qSin((m-fi)*param_rad))/j)*param_degree;
    //Расчет угла psi, град
     psi = x - ro;
    //Расчет угла alpha, радианы
    if((b*param_degree+psi-(m-fi))>0) alp = (b*param_degree+psi-(m-fi))*param_rad;
    else alp = (360+(b*param_degree+psi-(m-fi)))*param_rad;
    tf = (((a*r)/(1000*c))*((qSin(alp))/(qSin(b))));
    if(m >= 180 && m <= 360) {
        tN = tf*(f-vDisb/1000)-mRot;
    } else {
        tN = tf*(f*0.75-vDisb/1000)-mRot;
      }
    return tN;
}

