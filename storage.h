#ifndef DATAINPUT_H
#define DATAINPUT_H
#include <QObject>
class Storage : public QObject {
    Q_OBJECT
    private:
        double vD_new, mUr_new, a_new, c_new, p_new, k_new, r_new, fi_new, f_new;
        double vDisb, mRot;
        double vD = 340;
        double mUr = 75132.1312;
        double a = 2500;
        double c = 2500;
        double p = 3000;
        double k = 3752;
        double r = 1200;
        double fi = 35.8;
        double f = 88.07468;
    public:
        double getVd();
        double getMur();
        double getA();
        double getC();
        double getP();
        double getK();
        double getR();
        double getFi();
        double getF();
        double getVdisb();
        double getMrot();
        double getVd_new();
        double getMur_new();
        double getA_new();
        double getC_new();
        double getP_new();
        double getK_new();
        double getR_new();
        double getFi_new();
        double getF_new();
        void setVd();
        void setMur();
        void setA();
        void setC();
        void setP();
        void setK();
        void setR();
        void setFi();
        void setF();
        void setVdisb(double vDisb_new);
        void setMrot(double mRot_new);
    public slots:
        void setVd_new(double vD_di);
        void setMur_new(double mUr_di);
        void setA_new(double a_di);
        void setC_new(double c_di);
        void setP_new(double p_di);
        void setK_new(double k_di);
        void setR_new(double r_di);
        void setFi_new(double fi_di);
        void setF_new(double f_di);

     signals:
        void valueChangedVd(double newVd);
        void valueChangedMur(double newMur);
        void valueChangedA(double newA);
        void valueChangedC(double newC);
        void valueChangedP(double newP);
        void valueChangedK(double newK);
        void valueChangedR(double newR);
        void valueChangedFi(double newFi);
        void valueChangedF(double newF);
};


#endif // DATAINPUT_H
