#ifndef OSCOPE_H
#define OSCOPE_H

#include <QWidget>
#include <QtGui>
#include <QPushButton>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>


//static int m_data_width=10;
//#define M_DATA_WIDTH 1024


class OSource
{
public:
  enum Type {Undefined, Signal, Control, Sensor, Setpoint, Bool, Number};
  virtual ~OSource() {};
  virtual unsigned int size() const = 0;
  virtual Type type(const QString & name) const = 0;
  virtual bool valid(const QString & name, unsigned int tick) = 0;
  virtual bool DI(const QString & name, unsigned int tick) = 0;
  virtual float AI(const QString & name, unsigned int tick) = 0;
};

class OFileSourceDyno {
public:
  typedef struct
  {
    QString date;
    QVector<double> x_dyno;
    QVector<double> y_dyno;
    QwtPlotCurve *dyno_plot_curve;
    bool show;
    int id;
    bool show_recount;
    QwtPlotCurve *dyno_plot_curve_recount;
    QVector<double> x_dyno_rec;
    QVector<double> y_dyno_rec;
    int point;
  } DynoCurve;
  QList<DynoCurve> dynoCurveList;
  QFile *m_fd;
  OFileSourceDyno();
  ~OFileSourceDyno();
  QList<OFileSourceDyno::DynoCurve> openDyno(const QString & filename);
};

class OFileSource : public OSource
{
    //Q_OBJECT
  typedef struct
  {
    QString name;
    OSource::Type type;
    int offset;
  } Curve;
  QFile *m_fd;


  void addCurve(const QString & type, const QString & name, int *offset);
  OSource::Type str2type(const QString & type);

public:
//  typedef struct
//  {
//    QVector<int> x_dyno;
//    QVector<int> y_dyno;
//  } DynoCurve;
//  QList<DynoCurve> dynoCurveList;
  QMap<QString, Curve*> m_curves;
  unsigned int m_size;
  unsigned int m_packsize;
  unsigned int m_offset;

//signals:
  //void signalDynoCurve(QList<OFileSource::DynoCurve> dynoCurveList);
  OFileSource();
  virtual ~OFileSource();
  bool open(const QString & filename);
  void close();
  virtual unsigned int size() const;
  virtual OSource::Type type(const QString & name) const;
  virtual bool valid(const QString & name, unsigned int tick);
  virtual bool DI(const QString & name, int unsigned tick);
  virtual float AI(const QString & name, int unsigned tick);
};

class FileArray
{
  int m_fd;
public:
  FileArray(const QString & fn);
  ~FileArray();

  void addValue(float value);
  float value(int offs);
};
class QwtPlotCurve;
class QwtPlot;
class Osc : public QObject
{
  Q_OBJECT

  typedef struct
  {
    QString id;
    QString unit;
    float max;
    int dec;

    QList<float> values;
    //FileArray *fa;
    QString data;

    bool showValue;
    bool hidden;
    QwtPlotCurve* curve_plot;
  } SCurve;
public:
    typedef struct
    {
        QString id;
        int number;
        double x[1024];
        double y[1024];
        QVector<double> new_x;
        QVector<double> new_y;
        QVector<double> axis_x;
        QVector<double> axis_y;
        QVector<double> axis_sum_point;
        double x_end;
        double y_end;
        bool show;
        int j_point;
        int offsetCurve;
        QwtPlotCurve* curve;
        QwtPlotCurve *plot_curve_apr1;
        QwtPlotCurve *plot_curve_apr2;
        QwtPlotCurve *plot_curve_apr3;
        QwtPlotCurve *plot_curve_apr4;
        bool approx;
        QVector<double> approx_x;
        QVector<double> approx_y;
        QVector<double> min_x;
        QVector<double> min_y;
        QVector<double> max_x;
        QVector<double> max_y;
        QVector<double> min_i_n;
        QVector<double> max_i_n;
        QVector<double> x_degree;
        QVector<double> y_degree;
        QwtPlotCurve *plot_curve_degree;
    } NewPointCurve;
private:
  QList<OFileSourceDyno::DynoCurve> dynoCurveList;
  QMap<QString, SCurve> m_curve;
  QList<QString> m_curvelist;
  OSource *m_src;
  QwtPlot *m_plot;
  QwtSymbol *m_symbol;
  QMap<QString, NewPointCurve> mapPointCurve;


  int count;
  int m_data_offset;
  //int m_data_offset_x;
  int m_data_width;
  int m_zoom; // miliseconds in pixel
  double x_end;
  double y_end;
  QTime m_started;
  int m_offset; // pixels
  int m_nvalues;
  QTimer m_timer;
//  QMap<QString, float> m_curValues;
  bool stateCheckBox;
private:
  QPolygonF graph_points;
  QList <NewPointCurve> listNewPointCurves;
  int curvePeriod;
signals:
  void getCurves(QList <Osc::NewPointCurve> *listNewPoitnCurves);
  void signalSetData(double min_y_1, double min_x_1, double min_y_2, double min_x_2,
                     double max_y_1, double max_x_1, double max_y_2, double max_x_2);
public slots:
  //void updateCurve(QList<Osc::NewPointCurve> *curve, int index);
  void updateCurve(bool show, int index);
  void updateCurveOffset(int index, double offset);
  void updateApprox(int index, bool approx);
  void approximation(int index);
  int setCurvePeriod(int value);
  void dumpExcel();
  void updateCurveDyno(int index, bool show);
  void updateCurveDynoRecount(int index, bool show);
  void setStateCheckBox(bool state);
public:
  void setDynoCurve(QList<OFileSourceDyno::DynoCurve> dynoCurveList);
  Osc(QObject *parent = 0, OSource *src=0);

  bool addCurveDefaultValue(QString id, QString unit, float max, int dec=0,
                            bool showValue=false, QString data="", QString color="");

  void setSrc(OSource *src);
  float value(SCurve & curve, int tick);
  void pointCurve(SCurve &curve);
  void paintCurves();
  void attach(QwtPlot* plot);
  void init();
  void setWidth(int val);
  void setDataOffset(int val);
  void setCurveParametersOsc(QwtPlotCurve *curve_plot);
  QString getName(int val);
  void addNewCurve(int count);
  void initCurve(NewPointCurve newPointCurves);
  void rebuildCurve(NewPointCurve *curve);
  void searchMinMax(NewPointCurve curve);
  void paintDynoCurve();
  void dynoCurveRecount();
  void calculationCurveDegree();
  QList<OFileSourceDyno::DynoCurve> getDynoCurve();

};

//class AbstractData {
//public:
//   virtual int size()=0;
//   virtual float val(int pos)=0;
//   virtual float ord(int pos)=0;
//};

//class MemData:  public AbstractData {

//public:
//    MemData(OSource *src=0);
//    ~MemData();
//    int m_data_width;
//   //size_t m_data_width;
//private:
//   //double pointArray[m_data_width][2]; // 1024 нужно будет заменить на m_data_width
//    QVector<double> pointArray_x;
//    QVector<double> pointArray_y;
//   OSource *m_src;
//   Osc *m_osc;
//public:
//   //OFileSource *ofs;
//   void setSrcAbstr(OSource *src);
//   void recoadData();
//   float value(QString data, int tick);
//   virtual int size() {return m_data_width; };
//   virtual float val(int pos) {return pointArray_x[pos]; };
//   virtual float ord(int pos) {return pointArray_y[pos]; };
//};

//class Fragment: public AbstractData {
//public:
//  Fragment(AbstractData&);
//  ~Fragment();

//// void fragment(Fragement x, int offset) {
////   base=base.x;
////   ofset=offset+ x.offset;
////  }

//  void findeCurve();
//  virtual int size(){ return cut_size;}
//  virtual float val(int pos) { base.val(offset+pos); }
//  virtual float ord(int pos) { base.ord(offset+pos); }

//protected:
//  AbstractData& base;
//  int offset;
//  int cut_size;
//  int count;
//};

//class Scaled: public AbstractData {
//public:
//    Scaled();
//    ~Scaled();
//    virtual int size(){ return Base.size(); }
//    virtual float val(int pos) { Base.val(pos); }
//    virtual float ord(int pos) { Base.ord(pos)*scaleX; }
//protected:
//    AbstractData& Base;
//    float scaleX;
//};

//class Shift: public AbstractData {
//public:
//    Shift();
//    ~Shift();
//    virtual int size(){ return Base.size();}
//    virtual float val(int pos) { Base.val(pos); }
//    virtual float ord(int pos) { Base.ord(pos)-scaleX; }
//protected:
//    AbstractData& Base;
//    float scaleX;
//};

//class QwtPaint:  QwtSeriesData<QPointF> {
//public:
//    QwtPaint();
//    ~QwtPaint();
//    QPointF sample(size_t i) const override;
//private:
//    AbstractData&  dataref;
//};

#endif // OSCOPE_H
