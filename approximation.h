#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include <QObject>
#include "xlsxwriter.h"

//class Approximation
//{
//    Q_OBJECT
//public:
//    Approximation();
//};

class GlissPoly {
protected:
  int K;
  double *data;
  double *p;//[2*K+1]; // zum x^p
  double *g;//[K+1]; // zum y*x^i;
  double *f;//[K+1]; // prev a

  double *a;//[K+1];
  double *b;//[K+1]; // g
  double *sums;//[(K+1)*(K+1)];
  lxw_worksheet *worksheet;
  int buffer_size() const;
public:
  GlissPoly(int _K=1):data(0) { init(_K); }

  /*
  operator =(GlissPoly ref) {
   init(ref.K);
   for(i=0;i<buffer_size();i++){
      data[i]=ref.data[i];
   }
 }
 }  */

  void in(double x, double y);
  void ex(double x, double y);
  void init(int _k);
  int count();
  bool solv();
  void commit();
  bool fail;
  double Y(double x) const;
  double dY(double x);
  double X(double y, double eps=0.00001);
  double Xx(double y, double x_, double eps=0.00001);
  double S_line_magic(double *pts=0);
  double S_line(double x);
  double S_revs(double x);
  lxw_worksheet getWorksheet(lxw_worksheet *worksheet, int row, int col);
  ~GlissPoly() {  delete[] data; }
};

#endif // APPROXIMATION_H
