#include "approximation.h"
#include <qmath.h>
#include <qmatrix.h>
#include <QDebug>

//Approximation::Approximation()
//{

//}

int GlissPoly::count() {
    if (!data) return 0;
    return p[0];
}
double GlissPoly::Y(double x) const {
  double res=0;
  if (!data) return res;
  for(int i=K;i>0;--i) { res+=f[i]; res*=x;}
  res+=f[0];
  return res;
}

lxw_worksheet GlissPoly::getWorksheet(lxw_worksheet *worksheet, int row, int col) {
    for(int i=K; i>=0; i--) {
        worksheet_write_number(worksheet, row, col+(K-i), f[i], NULL);
    }
    return *worksheet;
}

//double GlissPoly::dY(double x) {
//  double res=0;
//  if (!data) return res;
//  for(int i=K;i>1;--i) { res+=i*f[i]; res*=x;}
//  res+=f[1];
//  return res;
//}

//double GlissPoly::X(double y, double eps) {
//  if (!data) return 1/eps;
//  if (fabs(f[1])<eps) return 1/eps;
//  double x_=(K==3)? -f[2]/(3*f[3]) // S - magic 3
//           : (y-f[0])/f[1]; // 0
//  return Xx(y,x_,eps);
//}

//double GlissPoly::Xx(double y, double x_, double eps) {
//  if (!data) return 1/eps;
//  if (fabs(f[1])<eps) return 1/eps;
//  double _x; fail=false;
//  bool p=false;
//  double pdx=0;
//  for(;;x_=_x){
//    double r=Y(x_)-y;
//    double dy=dY(x_);
//    _x=x_-r/dy;
//    double  dx=fabs(x_-_x);
//    if (dx<eps) break;
//    if (p) {
//      if (dx>pdx) {
//        fail=true;
//        return x_;
//      }
//    } else p=true;
//    pdx=dx;
//  }
//  return (x_+_x)/2;
//}

int  GlissPoly::buffer_size() const{
   return (K+1)*(K+7)-1;
};

void GlissPoly::init(int _K){
  delete[] data;
  K=_K;
  if (K<1) { data=0; return; };
  int _SZ=buffer_size();
  data=new double[_SZ];
  p=data;
  g=p+2*K+1;
  f=g+(K+1);
  a=f+(K+1);
  b=a+(K+1);
  sums=b+(K+1);
  // set zero

  for(int i=0; i<_SZ; ++i) {
    data[i]=0;
  };
}

void GlissPoly::in(double x, double y){
// append new one
  if (!data) return;
  //init free coefficients column + powers
  double q=1;
  for(int i=0;i<=K;++i){
    g[i]+= q*y;
    p[i]+= q;
    q*=x;
  }

  // powers
  for(int i=K+1;i<=2*K;++i){
    p[i]+=q;
    q*=x;
  }
}

//void GlissPoly::ex(double x, double y){
//// delete old one
//  if (!data) return;
//  //init free coefficients column + powers
//  double q=1;
//  for(int i=0;i<=K;++i){
//    g[i]-= q*y;
//    p[i]-= q;
//    q*=x;
//  }

//  // powers
//  for(int i=K+1;i<=2*K;++i){
//    p[i]-=q;
//    q*=x;
//  }
//}

bool GlissPoly::solv() {
  if (!data) return false;
  /*  loop place for timings */
  int i, j, k;

  //init square sums matrix
  for(i=0; i<=K; i++){
    b[i]=g[i]; // free coefficients
    for(j=0; j<=K; j++) {
      sums[(K+1)*i+j] = p[i+j];
    }
  }

  //check if there are 0 on main diagonal and exchange rows in that case
  double temp=0;
  for(i=0; i<=K; i++){
    if(sums[(K+1)*i+i]==0){ //?? менее чем по абс. знач
      for(j=0; j<=K; j++) {
        if(j==i) continue;
          if(sums[(K+1)*j+i] !=0 && sums[(K+1)*i + j] !=0) {
             for(k=0; k<=K; k++) {
               temp = sums[(K+1)*j+k];
               sums[(K+1)*j+k] = sums[(K+1)*i+k];
               sums[(K+1)*j+k] = temp;
             }
             temp = b[j];
             b[j] = b[i];
             b[i] = temp;
             break;
          }
      }
    }
  }

  for(k=0; k<=K; k++){
    if(sums[(K+1)*k + k]==0){
      return false;
    }

    for(i=k+1; i<=K; i++){
      // check was here
      double M = sums[(K+1)*i + k] / sums[(K+1)*k + k];
      for(j=k;  j<=K; j++){
        sums[(K+1)*i + j] -= M * sums[(K+1)*k + j];
      }
      b[i] -= M*b[k];
    }
  }

  a[K]=0;
  for(i=K; i>=0; i--){
    double s = 0;
    for(j = i; j<=K; j++){
      s = s + sums[(K+1)*i+j]*a[j];
    }
    a[i] = (b[i] - s) / sums[(K+1)*i+i];
  }
  return true;
}

void GlissPoly::commit(){
  if (!data) return;
  /*  loop end place for timings */
  for(int i=K; i>=0; i--){ // keep prev
    f[i] = a[i];
  }
}

//double GlissPoly::S_line_magic(double *pts) {
//  // only for K=3
//  // qDebug()<<f[0]<<f[1]<<f[2]<<f[3];
//  if (!data) return 0;//NANfalse;
//  double xp=-f[2]/(3*f[3]);
//  double yp=Y(xp);
//  a[1]=f[1]-f[2]*f[2]/(3*f[3]);
//  a[0]=yp-a[1]*xp;
//  double xz=-a[0]/a[1];
//  if (pts) {
//    pts[0]=xp;
//    pts[1]=yp;
//    pts[2]=xz;
//    pts[3]=0;
//    pts[4]=a[1];
//    pts[5]=a[0];
//  }
//  // qDebug()<<xp<<yp<<a[0]<<a[1]<<xz;
//  return xz;
//}

//double GlissPoly::S_line(double x) {
//  if (!data) return 0;//NANfalse;
//  return a[0]+x*a[1];
//}

//double GlissPoly::S_revs(double x) {
//  return (x-a[0])/a[1];
//}
