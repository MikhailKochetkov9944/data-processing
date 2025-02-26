#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "approximation.h"

class Fragment;

class AbstractData
{
public:
   virtual int size() const =0;
   virtual float val(int pos) const =0;
   virtual float ord(int pos) const =0;

   int getIndexMinVal(int ord_start, int ord_end);
   int getIndexMaxVal(int ord_start, int ord_end);
   bool checkValidOrd(int ord_start, int ord_end);
   static QVector<AbstractData*> finder(AbstractData* curve_base);
};

class Fragment: public AbstractData {
public:
  Fragment(AbstractData* dat, int _offset, int _size) {
      Fragment* frag=dynamic_cast<Fragment*>(dat);
      if (frag) {
          base=frag->base;
          offset=frag->offset+_offset;
      } else {
        base=dat;
        offset=_offset;
      }
      count=_size;
  };

//  Fragment(Fragment* frag) {
//     base=frag->base;
//     offset=frag->offset;
//     count=frag->count;
//  }

  Fragment(const Fragment& frag) {
     base=frag.base;
     offset=frag.offset;
     count=frag.count;
  }

//  ~Fragment();
// void fragment(Fragement x, int offset) {
//   base=base.x;
//   ofset=offset+ x.offset;
//  }

  virtual int size() const override { return count; }
  virtual float val(int pos) const override { return base->val(offset+pos); }
  virtual float ord(int pos) const override { return base->ord(offset+pos); }

protected:
  AbstractData* base;
  int offset;  
  int count;
};


class GlissPolyData: public AbstractData {
public:
 virtual float val(int pos) const override { return poly.Y(ord(pos)); };
    bool init(AbstractData* dat, int n) {
    poly.init(n);

    //AbstractData* dat 0  to size()
    for(int i=0; i<dat->size(); i++) {
        poly.in(dat->ord(i),
                dat->val(i));
    }

    if(!poly.solv()) return false;
    poly.commit();
    return true;
   }
//protected:
  GlissPoly poly;
};

class PolyFragment: public GlissPolyData {
public:
    PolyFragment(AbstractData* dat, int n) {
                init(dat, n);
                base=dat;
    }
    virtual int size() const override { return base->size(); }
    virtual float ord(int pos) const override { return base->ord(pos); }
    inline GlissPoly getPoly() { return poly; }
 protected:
     AbstractData* base;

};

class PolyTewmperFragment: public GlissPolyData {
    PolyTewmperFragment(AbstractData* dat, int n, int _count) {
        init(dat, n);
        start=dat->ord(0);
        end=dat->ord(dat->size()-1);
        count = _count;
    };

   PolyTewmperFragment(PolyFragment pf,  float start, float end, int count) {
       poly=pf.poly;
   };
public:
     virtual int size() const override { return count+1; }
     virtual float val(int pos) const override { return poly.Y(ord(pos)); }
     virtual float ord(int pos) const override { return start+((end-start)*pos/count); }
protected:
     float start;
     float end;
     int count;
     GlissPoly poly;
};




class Scaled: public AbstractData {
public:
    Scaled();
    ~Scaled();
    virtual int size() const override { return Base.size(); }
    virtual float val(int pos) const override { Base.val(pos); }
    virtual float ord(int pos) const override { Base.ord(pos)*scaleX; }
protected:
    AbstractData& Base;
    float scaleX;
};

class Shift: public AbstractData {
public:
    Shift();
    ~Shift();
    virtual int size() const override { return Base.size(); }
    virtual float val(int pos) const override { Base.val(pos); }
    virtual float ord(int pos) const override { Base.ord(pos)-scaleX; }
protected:
    AbstractData& Base;
    float scaleX;
};
#endif // ABSTRACTDATA_H
