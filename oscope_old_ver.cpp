#include "oscope_old_ver.h"
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <QtAlgorithms>
#include "approximation.h"
#include <QAxObject>
#include <QRandomGenerator>


using namespace std;
#define ABS_HEIGHT 20


#pragma pack(push, 1)
  typedef struct
  {
    char valid;
    float value;
    float rawValue;
  } Analog;
  typedef struct
  {
    char valid;
    char value;
  } Digital;
#pragma pack(pop)


//QPointF QwtPaint::sample(size_t i) const {
//    return QPointF(dataref.val(i), dataref.ord(i));
//}

OFileSourceDyno::OFileSourceDyno(): m_fd(0) {}
QList<OFileSourceDyno::DynoCurve> OFileSourceDyno::openDyno(const QString & filename) {
    assert(!m_fd);
    ifstream is(filename.toStdString().c_str());
    char line[1024];
    DynoCurve d_curve;
    dynoCurveList.append(d_curve);
    int index=0;
    QString name;
    while(1)
    {
      is.getline(line, sizeof(line), '\t');
      if(!*line)
        break;
      QStringList strList = QString(line).split(' ');
      if(strList.size()==8) {
         index++;
         DynoCurve d_curve;
         dynoCurveList.append(d_curve);
         dynoCurveList[index-1].date = "day:"+strList[3]+", month:"+strList[4]+", "+strList[5]+", time:"
                 +strList[6]+", year:"+strList[7].remove("\nI");
      }
      if(strList.size()==6) {
         dynoCurveList[index].x_dyno.append(strList[2].toInt());
         dynoCurveList[index].y_dyno.append(strList[5].remove("\nI").toInt());
      }

    }

    dynoCurveList.removeLast();
    return dynoCurveList;
}
OFileSource::OFileSource()
  : m_size(0),
    m_packsize(0),
    m_offset(0),
    m_fd(0)
{}

bool OFileSource:: open(const QString & filename) {
    qDebug() << "Flag_2.0";
    assert(!m_fd);
    ifstream is(filename.toStdString().c_str());
    if(!is.is_open())
      return false;
    char line[1024];
    QString type, name;
    int offset = 0;
    while(1)
    {
      is.getline(line, sizeof(line), '\0');
      qDebug()<<"line ifstream"<<line;
      if(!*line)
        break;
      QStringList strList = QString(line).split(' ');
      qDebug()<<"strList ifstream"<<strList;
      type = strList[0];
      name = strList[1];
      qDebug()<<"type ifstream"<<type;
      qDebug()<<"name ifstream"<<name;
      addCurve(type, name, &offset);
    }
    m_packsize = offset;
    m_offset = is.tellg();
    is.seekg(0, ios::end);
    int size = is.tellg();
    m_size = (size - m_offset) / m_packsize;
    is.close();
    m_fd = new QFile(filename);
    if(!m_fd->open(QIODevice::ReadOnly))
    {
      delete m_fd;
      m_fd = 0;
      return false;
    }
    return true;
}

void OFileSource::close()
{
    qDebug() << "Flag_2.1";
  foreach(Curve *curve, m_curves.values())
    delete curve;
  if(m_fd)
    delete m_fd;
  m_size = 0;
  m_packsize = 0;
  m_offset = 0;
  m_fd = 0;
}

OFileSource::~OFileSource()
{
    qDebug() << "Flag_2.2";
    close();
}

unsigned int OFileSource::size() const
{
    qDebug() << "Flag_2.3";
  return m_size;
}

OSource::Type OFileSource::type(const QString & name) const
{
    qDebug() << "Flag_2.4";
  QMap<QString, Curve*>::const_iterator i = m_curves.find(name);
  if (i == m_curves.end()) return OSource::Undefined;
  Curve *curve = i.value();
  return curve->type;
}

bool OFileSource::valid(const QString & name, unsigned int tick)
{
    qDebug() << "Flag_2.5";
   if (tick >= m_size) return false;
   QMap<QString, Curve*>::const_iterator i = m_curves.find(name);
   if (i == m_curves.end()) return false;
   Curve *curve = i.value();
   unsigned int offset = m_offset + (m_packsize * tick) + curve->offset;
   m_fd->seek(offset);
   switch(curve->type)
   {
     case OSource::Signal:
     case OSource::Control:
       {
         Digital val;
         m_fd->read((char*)&val, sizeof(Digital));
         return val.valid;
       }

     case OSource::Sensor:
     case OSource::Setpoint:
       {
         Analog val;
         m_fd->read((char*)&val, sizeof(Analog));
         return val.valid;
       }
     default:
       return true;
   }
   return false;
}

bool OFileSource::DI(const QString & name, unsigned int tick)
{
    qDebug() << "Flag_2.6";
  assert(tick < m_size);
  QMap<QString, Curve*>::const_iterator i = m_curves.find(name);
  assert(i != m_curves.end());
  Curve *curve = i.value();
  unsigned int offset = m_offset + (m_packsize * tick) + curve->offset;
  m_fd->seek(offset);
  switch(curve->type)
  {
    case OSource::Signal:
    case OSource::Control:
      {
        Digital val;
        m_fd->read((char*)&val, sizeof(Digital));
        return val.value;
      }
    case OSource::Bool:
      {
        char val;
        m_fd->read((char*)&val, sizeof(char));
        return val;
      }
    default:
      assert(0);
  }
  return false;
}


float OFileSource::AI(const QString & name, unsigned int tick){
    qDebug() << "Flag_2.7";
  qDebug()<<" AI " << name << tick;
  assert(tick < m_size);
  QMap<QString, Curve*>::const_iterator i = m_curves.find(name);
  assert(i != m_curves.end());
  Curve *curve = i.value();
  unsigned int offset = m_offset + (m_packsize * tick) + curve->offset;
  m_fd->seek(offset);
//  float b;
  switch(curve->type)
  {
    case OSource::Signal:
    case OSource::Control:
      {
        Digital val;
        m_fd->read((char*)&val, sizeof(Digital));
        return val.value?1:0;
      }

    case OSource::Sensor:
    case OSource::Setpoint:
      {
        Analog val;
        m_fd->read((char*)&val, sizeof(Analog));
        return val.value;
      }
    case OSource::Bool:
      {
        char val;
        m_fd->read((char*)&val, sizeof(char));
        return val?1:0;
      }
    case OSource::Number:
      {
        float val;
        m_fd->read((char*)&val, sizeof(float));
        return val;
      }
    default:
      assert(0);
  }
  return 0;//QNAN;
}

OSource::Type OFileSource::str2type(const QString & type)
{
    qDebug() << "Flag_2.8";
  if(type == "SIGNAL")
    return OSource::Signal;
  else if(type == "CONTROL")
    return OSource::Control;
  else if(type == "SENSOR")
    return OSource::Sensor;
  else if(type == "SETPOINT")
    return OSource::Setpoint;
  else if(type == "BOOL")
    return OSource::Bool;
  else if(type == "NUMBER")
    return OSource::Number;
  else
  {
    QMessageBox::information(0, "Bad dump file", "Wrong type of element - " + type);
    assert(0);
  }
}

void OFileSource::addCurve(const QString & type, const QString & name, int *offset)
{
    qDebug() << "Flag_2.9";
  Curve *curve = new Curve;
  curve->name = name;
  curve->type = str2type(type);
  curve->offset = *offset;
  switch(curve->type)
  {
    case OSource::Signal:
    case OSource::Control:
      *offset += sizeof(Digital);
      break;
    case OSource::Sensor:
    case OSource::Setpoint:
      *offset += sizeof(Analog);
      break;
    case OSource::Bool:
      *offset += sizeof(char);
      break;
    case OSource::Number:
      *offset += sizeof(float);
      break;
    default:
      assert(0);
  }
  m_curves.insert(name, curve);
}

bool Osc::addCurveDefaultValue(QString id, QString unit, float max, int dec,
                                       bool showValue, QString data, QString color) {
    qDebug() << "Flag_2.10";
    SCurve curve;
    curve.id = id;
    curve.unit = unit;
    curve.max = max;
    curve.dec = dec;
    curve.showValue = showValue;
    curve.data = data;
    curve.hidden = false;
    curve.curve_plot = new QwtPlotCurve(); 
    m_curve[id] = curve;
    m_curvelist.append(id);
    return true;
}

void Osc::setSrc(OSource *src){
    qDebug() << "Flag_2.11";
  m_src=src;
  qDebug() << "Src size" << m_src->size();
  //paintCurves();
};

float Osc::value(SCurve & curve, int tick)
{
  if(tick >= m_src->size()) return 0;
  switch(m_src->type(curve.data))
  {
    case OSource::Signal:
    case OSource::Control:
    case OSource::Bool:
      qDebug()<<"DI_curve.data"<<curve.data;
      qDebug()<<"DI_tick"<<tick;
      return m_src->DI(curve.data, tick)?1:0;
    case OSource::Sensor:
    case OSource::Setpoint:
    case OSource::Number:
      qDebug()<<"AI_curve.data"<<curve.data;
      qDebug()<<"AI_tick"<<tick;
      return m_src->AI(curve.data, tick);
    default:
      return 0;
  }
  return 0;
}

/*
class AbstractData{
public:
   virtual int size()=0;
   virtual float val(int pos)=0;
   virtual float ord(int pos)=0;
};

class MemData{
private
   double pointArray[m_data_width][2];
public:
   virtual int size() {return m_data_width;
   virtual float val(int pos) {return pointArray[pos][0]; };
   virtual float ord(int pos) {return pointArray[pos][1]; };
};

class fragment: AbstarctData {

public:
  fragment(AbstarctData)

 fragment(fragement x, offset) {
   base=base.x;
   ofset=offset+ x.offset;
  }

  virtual int size(){ return cut_size;}
  virtual virtual float val(int pos) { Base.val(offset+pos); }
  virtual virtual float ord(int pos) { Base.ord(offset+pos); }

protected:
  AbstractData& Base;
  int offset;
  int cut_size;
};


class Scaled: AbstarctData{
public:
    virtual int size(){ return Base.size();}
    virtual virtual float val(int pos) { Base.val(pos); }
    virtual virtual float ord(int pos) { Base.ord(pos)*scaleX; }
protected:
    AbstractData& Base;
    float scaleX;
};

class Shift: AbstarctData{
public:
    virtual int size(){ return Base.size();}
    virtual virtual float val(int pos) { Base.val(pos); }
    virtual virtual float ord(int pos) { Base.ord(pos)-scaleX; }
protected:
    AbstractData& Base;
    float scaleX;
};

class Seri:  QwtSeriesData< QPointF >{

    sample(i) { return Pointf(dataref.val(i),dataref.ord(i)));};
private:
 AbstarctData&  dataref;
}


GlissPoly* approx(AbstarctData* data);

  GlissPoly* res= new GlissPoly(3);//

  GlissPoly&   poly_4=*res;

  for(int i=listNewPointCurves[index-1].max_i_n[1]-1; i<listNewPointCurves[index-1].new_x.size(); i++) {
      poly_4.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
      qDebug()<<"approximation()_4.1";
  }
  for(int i=0; i<listNewPointCurves[index].min_i_n[0]; i++) {
      poly_4.in(listNewPointCurves[index].new_x[i]+listNewPointCurves[index-1].new_x.last(), listNewPointCurves[index].new_y[i]+150);
      qDebug()<<"approximation()_4.2";
  }
  if(!poly_4.solv()) return 0;
  poly_4.commit();
  return res;
}

*/
void Osc::pointCurve(SCurve & curve) {
    qDebug() << "Flag_2.13";
    graph_points.clear();
  int i, x, n = 0;
  bool flag = false;
  bool flagNewCurve = true;
  qDebug()<<curve.id;

  double pointArray[m_data_width][2];

  for(x=0, i=0;x<m_data_width;x++, i++)
  {
    pointArray[i][0] = value(m_curve["Tm"], m_data_offset+x);
                        //*250)/1000) - 1191.75;
    pointArray[i][1] = value(curve, m_data_offset+x);
    int v = pointArray[i][1];
    if(v&0x8000) pointArray[i][1] -= 65536;

//    qDebug()<<"x-"<< x;
//    qDebug()<<"y-"<< value(curve, (m_data_offset+x)*100);
//    graph_points << QPointF (pointArray[x][0], pointArray[x][1]);
  }

//*****************************************************************
//  GlissPoly poly(3);
//  for(int x=0; x<m_data_width; x++) {
//      poly.in(pointArray[x][0], pointArray[x][1]);
//  }
//  if(!poly.solv()) return;
//  poly.commit();
//  for(int x=0; x<m_data_width; x++) {
//      double y=poly.Y(pointArray[x][0]);
//      pointArrayNew[x][0] = pointArray[x][0];
//      pointArrayNew[x][1] = y;

//      graph_points << QPointF (pointArrayNew[x][0], pointArrayNew[x][1]);
//  }

//*****************************************************************

  //************************************************
  count = 1;
  for(int i=0, j=0; i<m_data_width; i++) {
      if(flagNewCurve) {
          flagNewCurve = false;
          addNewCurve(count);
      }
//      if(!flag && count!=1 && pointArray[i-1][1]*pointArray[i+1][1] < 0) {
//          flag = true;
//      }
      if((pointArray[i-1][1]*pointArray[i+1][1]) <= 0 || flag) { // поиск начала периода
         //if(n == 0)
             flag = true;
             j++;
         if((pointArray[i][1]*pointArray[i+1][1]) <= 0 && pointArray[i-1][1]*pointArray[i+1][1] < 0) {
             n++;
             listNewPointCurves[count-1].axis_x.append(pointArray[i][0]);
             listNewPointCurves[count-1].axis_y.append(pointArray[i][1]);
             listNewPointCurves[count-1].axis_sum_point.append(listNewPointCurves[count-1].new_x.size());
         }
         if(n < 5) { //первый период
//              pointArrayNew[j][0] = pointArray[i][0];
//              pointArrayNew[j][1] = pointArray[i][1];
              listNewPointCurves[count-1].new_x.append(pointArray[i][0]);
              listNewPointCurves[count-1].new_y.append(pointArray[i][1]);
//              mapPointCurve.find(getName(count)).value().x[j]= pointArray[i][0];
//              mapPointCurve.find(getName(count)).value().y[j] = pointArray[i][1];
//              applyPoint(count, j);
         }
         if(n/(5*count) == 1) { // последняя точка в периоде
            n++;
            if(count == 1) {
                listNewPointCurves[count-1].new_x.append(pointArray[i][0]);
                listNewPointCurves[count-1].new_y.append(pointArray[i][1]);
//                mapPointCurve.find(getName(count)).value().x[j]= pointArray[i][0];
//                mapPointCurve.find(getName(count)).value().y[j] = pointArray[i][1];
            } else {
                listNewPointCurves[count-1].new_x.append(pointArray[i][0] - x_end);
                listNewPointCurves[count-1].new_y.append(pointArray[i][1]-(150*(count-1)));
//            mapPointCurve.find(getName(count)).value().x[j]= pointArray[i][0] - x_end;
//            mapPointCurve.find(getName(count)).value().y[j] = pointArray[i][1]-(150*(count-1));
            }
//            mapPointCurve.find(getName(count)).value().j_point = j;
//            applyPoint(count, j);
            initCurve(listNewPointCurves[count-1]);
            searchMinMax(listNewPointCurves[count-1]);
            j=0;
            flag = false;
            flagNewCurve = true;
            count += 1;
            x_end = pointArray[i][0];
            y_end = pointArray[i][1];

//            curve.curve_plot->setSamples(graph_points); // ассоциировать набор точек с кривой
         }
         if(count!=1 && !flagNewCurve) { // последующие периоды
             listNewPointCurves[count-1].new_x.append(pointArray[i][0] - x_end);
             listNewPointCurves[count-1].new_y.append(pointArray[i][1]-(150*(count-1)));
//             mapPointCurve.find(getName(count)).value().x[j]= pointArray[i][0] - x_end; // (x_end*(count-1))
//             mapPointCurve.find(getName(count)).value().y[j] = pointArray[i][1]-(150*(count-1));
 //            applyPoint(count, j);
         }

//              qDebug()<<">>>>>x:"<< pointArray[i][0];
//              qDebug()<<">>>>>y:"<< pointArray[i][1];
//              qDebug()<<">>>>>x_NEW:"<< pointArrayNew[j][0];
//              qDebug()<<">>>>>y_NEW:"<< pointArrayNew[j][1];
//              qDebug()<<">>>>>j:"<< j;
//              qDebug()<<">>>>>n:"<< n;
              //qDebug()<<">>>>>k:"<< k;
      }


        if(n/5 == curvePeriod) break; // кол-во периодов

         }
  listNewPointCurves.removeFirst();
  for(int i=1;i<listNewPointCurves.size()-1; i++) {
      approximation(i);
      qDebug()<<"i"<<i;
  }

  //************************************************
   calculationCurveDegree();
   stateCheckBox = false;
   qDebug()<<"stateCheckBox"<<stateCheckBox;
   emit getCurves(&listNewPointCurves);
}
void Osc::initCurve(NewPointCurve newPointCurves) {
    qDebug()<<"setCurve()";
   QwtPlotCurve *curve = new QwtPlotCurve();
   if(!curve) return;
   curve->setPen(Qt::green);
   QPolygonF polygon;
   for(int i=0; i<newPointCurves.new_x.size(); i++) {
       polygon << QPointF (newPointCurves.new_x[i], newPointCurves.new_y[i]);
   }
   curve->setRenderHint(QwtPlotItem::RenderAntialiased, true); //сглаживание
   //Точки кривой
   QwtSymbol *symbol;
   symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::yellow), QPen(Qt::red), QSize(4, 4));
   curve->setSymbol(symbol);
   curve->setSamples(polygon);
   curve->attach(m_plot);
   m_plot->replot();
   curve->setVisible(false);
   listNewPointCurves[count-1].show=false;
   listNewPointCurves[count-1].curve = curve;
}
void Osc::searchMinMax(NewPointCurve curve) {
    qDebug()<<"searchMinMax()";
    qDebug()<<"curve.axis_sum_point[0]"<<curve.axis_sum_point[0];
    qDebug()<<"curve.axis_sum_point[1]"<<curve.axis_sum_point[1];
    qDebug()<<"curve.axis_sum_point[2]"<<curve.axis_sum_point[2];
    qDebug()<<"curve.axis_sum_point[3]"<<curve.axis_sum_point[3];
    //qDebug()<<"curve.axis_sum_point[4]"<<curve.axis_sum_point[4];
    qDebug()<<"curve.new_x.size()"<<curve.new_x.size();
    curve.min_y.append(curve.new_y[0]);
    curve.min_x.append(curve.new_x[0]);
    curve.min_i_n.append(0);
    for(int i=0; i<curve.axis_sum_point[0]; i++) {
        if(curve.min_y[0] > curve.new_y[i]) {
            curve.min_y[0] = curve.new_y[i];
            curve.min_x[0] = curve.new_x[i];
            curve.min_i_n[0] = i;
//            curve.min_y.replace(0, curve.new_y[i]);
//            curve.min_x.replace(0, curve.new_x[i]);
        }
    }
    qDebug()<<"curve.min_i_n[0]"<<curve.min_i_n[0];
    curve.min_y.append(curve.new_y[0]);
    curve.min_x.append(curve.new_x[0]);
    curve.min_i_n.append(0);
    for(int i=curve.axis_sum_point[1]; i<curve.axis_sum_point[2]; i++) {
        if(curve.min_y[1] > curve.new_y[i]) {
            curve.min_y[1] = curve.new_y[i];
            curve.min_x[1] = curve.new_x[i];
            curve.min_i_n[1] = i;
//            curve.min_y.replace(1, curve.new_y[i]);
//            curve.min_x.replace(1, curve.new_x[i]);
        }
    }
qDebug()<<"curve.min_i_n[1]"<<curve.min_i_n[1];
    curve.max_y.append(curve.new_y[0]);
    curve.max_x.append(curve.new_x[0]);
    curve.max_i_n.append(0);
    for(int i=curve.axis_sum_point[0]; i<curve.axis_sum_point[1]; i++) {
        if(curve.max_y[0] < curve.new_y[i]) {
            curve.max_y[0] = curve.new_y[i];
            curve.max_x[0] = curve.new_x[i];
            curve.max_i_n[0] = i;
//            curve.max_y.replace(0, curve.new_y[i]);
//            curve.max_x.replace(0, curve.new_x[i]);
        }
    }
    qDebug()<<"curve.max_i_n[0]"<<curve.max_i_n[0];
    curve.max_y.append(curve.new_y[0]);
    curve.max_x.append(curve.new_x[0]);
    curve.max_i_n.append(0);
    for(int i=curve.axis_sum_point[2]; i<curve.axis_sum_point[3]; i++) {
        if(curve.max_y[1] < curve.new_y[i]) {
            curve.max_y[1] = curve.new_y[i];
            curve.max_x[1] = curve.new_x[i];
            curve.max_i_n[1] = i;
        }
    }

    qDebug()<<"curve.max_i_n[1]"<<curve.max_i_n[1];

//    if(curve.min_x[0] > curve.min_x[1]) {
//        double y1 = curve.min_y[0];
//        double y2 = curve.min_y[1];
//        double x1 = curve.min_x[0];
//        double x2 = curve.min_x[1];
//        curve.min_y.replace(0, y2);
//        curve.min_y.replace(1, y1);
//        curve.min_x.replace(0, x2);
//        curve.min_x.replace(1, x1);
//    }
//    if(curve.max_x[0] > curve.max_x[1]) {
//        double y1 = curve.max_y[0];
//        double y2 = curve.max_y[1];
//        double x1 = curve.max_x[0];
//        double x2 = curve.max_x[1];
//        curve.max_y.replace(0, y2);
//        curve.max_y.replace(1, y1);
//        curve.max_x.replace(0, x2);
//        curve.max_x.replace(1, x1);
//    }

    for(int i=0; i<curve.min_x.size(); i++) {
        double min_y = curve.min_y[i]+(150*(count-1));
        double max_y = curve.max_y[i]+(150*(count-1));

        listNewPointCurves[count-1].min_y.append(min_y);
        listNewPointCurves[count-1].min_x.append(curve.min_x[i]);
        listNewPointCurves[count-1].max_y.append(max_y);
        listNewPointCurves[count-1].max_x.append(curve.max_x[i]);
        listNewPointCurves[count-1].min_i_n.append(curve.min_i_n[i]);
        listNewPointCurves[count-1].max_i_n.append(curve.max_i_n[i]);
    }

    qDebug()<<"curve.min_x.size()"<<curve.min_x.size();
    qDebug()<<"<<<<<<<<<min_y[0]"<<listNewPointCurves[count-1].min_y[0];
    qDebug()<<"<<<<<<<<<min_y[1]"<<listNewPointCurves[count-1].min_y[1];
    qDebug()<<"<<<<<<<<<max_y[0]"<<listNewPointCurves[count-1].max_y[0];
    qDebug()<<"<<<<<<<<<max_y[1]"<<listNewPointCurves[count-1].max_y[1];
    emit signalSetData(listNewPointCurves[count-1].min_y[0],
            listNewPointCurves[count-1].min_x[0],
            listNewPointCurves[count-1].min_y[1],
            listNewPointCurves[count-1].min_x[1],
            listNewPointCurves[count-1].max_y[0],
            listNewPointCurves[count-1].max_x[0],
            listNewPointCurves[count-1].max_y[1],
            listNewPointCurves[count-1].max_x[1]);
}

//void Osc::applyPoint(int count, int j) {
//    graph_points << QPointF (mapPointCurve.find(getName(count)).value().x[j], mapPointCurve.find(getName(count)).value().y[j]);
//}
void Osc::addNewCurve(int count) {
    qDebug() << "addNewCurve()";
    NewPointCurve newCurve;
    newCurve.id = getName(count);
    newCurve.number = count;
    newCurve.show = false;
    mapPointCurve[newCurve.id] = newCurve;
    listNewPointCurves.append(newCurve);
}
QString Osc::getName(int val) {
    QString name = "Curve_" + QString::number(val);
    return name;
}
//void Osc::setCurves(QList<Osc::NewPointCurve> *curve) {
//    listNewPointCurves = *curve;
//}
//void Osc::updateCurve(QList<Osc::NewPointCurve> *curve, int index) {
void Osc::updateCurve(bool show, int index) {   
        qDebug()<<"stateCheckBox"<<stateCheckBox;
        NewPointCurve *new_curve = &(listNewPointCurves)[index-1];
     if(!stateCheckBox) {
        new_curve->plot_curve_degree->setVisible(false);
        new_curve->show = show;
        new_curve->curve->setVisible(show);
        m_plot->replot();
    } else if(stateCheckBox) {
        new_curve->curve->setVisible(false);
        new_curve->show = show;
        new_curve->plot_curve_degree->setVisible(show);
        m_plot->replot();
    }
}
void Osc::updateCurveOffset(int index, double offset) {
    NewPointCurve *curveOffset = &(listNewPointCurves)[index-1];
    listNewPointCurves[index-1].offsetCurve = offset;
    QPolygonF polygon;
    QwtPlotCurve curve;
    QVector<double> x;
    for(int i=0; i<curveOffset->new_x.size(); i++) {
        x.append(listNewPointCurves[index-1].new_x[i] + offset);
        polygon << QPointF (x[i],
                listNewPointCurves[index-1].new_y[i]);
    }
    qDebug()<<"......offset"<<offset;
    listNewPointCurves[index-1].curve->setSamples(polygon);
    m_plot->replot();
}

//void Osc::approximation(int index, bool approx) { //аппроксимация всего периода
//    QPolygonF polygon;
//    listNewPointCurves[index-1].approx = approx;
//    if(approx) {
//      GlissPoly poly(3);
//      for(int i=0; i<listNewPointCurves[index-1].new_x.size(); i++) {
//          poly.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
//      }
//      if(!poly.solv()) return;
//      poly.commit();
//      for(int i=0; i<listNewPointCurves[index-1].new_x.size(); i++) {
//          double y=poly.Y(listNewPointCurves[index-1].new_x[i]);
//          listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index-1].new_x[i]);
//          listNewPointCurves[index-1].approx_y.append(y);

//          polygon << QPointF (listNewPointCurves[index-1].approx_x[i],
//                  listNewPointCurves[index-1].approx_y[i]);
//      }
//      listNewPointCurves[index-1].curve->setSamples(polygon);
//      m_plot->replot();
//    } else {
//        for(int i=0; i<listNewPointCurves[index-1].new_x.size(); i++) {
//            polygon << QPointF (listNewPointCurves[index-1].new_x[i],
//                listNewPointCurves[index-1].new_y[i]);
//        }
//        listNewPointCurves[index-1].curve->setSamples(polygon);
//        m_plot->replot();
//    }
//}
void Osc::updateApprox(int index, bool approx) {
    listNewPointCurves[index-1].plot_curve_apr1->setVisible(approx);
    listNewPointCurves[index-1].plot_curve_apr2->setVisible(approx);
    listNewPointCurves[index-1].plot_curve_apr3->setVisible(approx);
    listNewPointCurves[index-1].plot_curve_apr4->setVisible(approx);
    m_plot->replot();
}
void Osc::approximation(int index) { // аппроксимация периодов по отдельности
//*****************************************************************
    QPolygonF polygon;
    //if(approx) {
    qDebug()<<"approximation()_1";
        // от min1 до max1
        GlissPoly poly_1(3);
        for(int i=listNewPointCurves[index-1].min_i_n[0]; i<listNewPointCurves[index-1].max_i_n[0]+1; i++) {
            qDebug()<<listNewPointCurves[index-1].max_i_n[0];
            poly_1.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
        }
        if(!poly_1.solv()) return;
        poly_1.commit();
      for(int i=listNewPointCurves[index-1].min_i_n[0]; i<listNewPointCurves[index-1].max_i_n[0]+1; i++) {
          qDebug()<<listNewPointCurves[index-1].max_i_n[0];
          double y=poly_1.Y(listNewPointCurves[index-1].new_x[i]);
          listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index-1].new_x[i]);
          listNewPointCurves[index-1].approx_y.append(y);
  }
      QPolygonF polygon_points_1;
      QwtPlotCurve *curve_plot_1 = new QwtPlotCurve();
      listNewPointCurves[index-1].plot_curve_apr1 = curve_plot_1;
      listNewPointCurves[index-1].plot_curve_apr1->setPen( Qt::blue, 2 );
      listNewPointCurves[index-1].plot_curve_apr1->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // сглаживание
      int size_1 = listNewPointCurves[index-1].approx_x.size();
            for(int i=0; i<size_1; i++) {
              polygon_points_1 << QPointF (listNewPointCurves[index-1].approx_x[i],
                    listNewPointCurves[index-1].approx_y[i]);
            }
           listNewPointCurves[index-1].plot_curve_apr1->setSamples(polygon_points_1);
           listNewPointCurves[index-1].plot_curve_apr1->attach(m_plot);
           qDebug()<<"approximation()_2";
      // от max1 до min2
      GlissPoly poly_2(3);
      for(int i=listNewPointCurves[index-1].max_i_n[0]-1; i<listNewPointCurves[index-1].min_i_n[1]+1; i++) {
          qDebug()<<listNewPointCurves[index-1].max_i_n[0];
          qDebug()<<"i"<<i<<"approximation()_2.0.1";
          poly_2.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
      }
      qDebug()<<"approximation()_2.1";
      if(!poly_2.solv()) return;
      poly_2.commit();
    // newCurev_b_ poly_temper ( poly, data, step=0.1) {
       // left_ord=min(data.ord)
       // right_ord=max(data.ord)
       // diff=right_ord-left_ord;
       // size=diff/step;
       // val(pos ){ return poly_2.Y(ord(pos)); }
      // ord(pos) { return left_ord+pos*step; }
   // };

    // newCurev_b_ poly( poly, data,
    // size() { return data.size() }
    // val(pos ){ return poly_2.Y(ord(pos)); }
    // ord(pos) { return data.ord(pos) }
    //

    for(int i=listNewPointCurves[index-1].max_i_n[0]-1; i<listNewPointCurves[index-1].min_i_n[1]+1; i++) {
        double y=poly_2.Y(listNewPointCurves[index-1].new_x[i]);
        listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index-1].new_x[i]);
        listNewPointCurves[index-1].approx_y.append(y);
}

    qDebug()<<"approximation()_2.2";
    QPolygonF polygon_points_2;
    QwtPlotCurve *curve_plot_2 = new QwtPlotCurve();
    listNewPointCurves[index-1].plot_curve_apr2 = curve_plot_2;
    listNewPointCurves[index-1].plot_curve_apr2->setPen( Qt::red, 2 );
    listNewPointCurves[index-1].plot_curve_apr2->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // сглаживание
    int size_2 = listNewPointCurves[index-1].approx_x.size();
          for(int i=size_1; i<size_2; i++) {
            polygon_points_2 << QPointF (listNewPointCurves[index-1].approx_x[i],
                  listNewPointCurves[index-1].approx_y[i]);
          }
         listNewPointCurves[index-1].plot_curve_apr2->setSamples(polygon_points_2);
         listNewPointCurves[index-1].plot_curve_apr2->attach(m_plot);
         qDebug()<<"approximation()_3";
    // от min2 до max2
    GlissPoly poly_3(3);
    for(int i=listNewPointCurves[index-1].min_i_n[1]-1; i<listNewPointCurves[index-1].max_i_n[1]+1; i++) {
        poly_3.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
    }
    if(!poly_3.solv()) return;
    poly_3.commit();
  for(int i=listNewPointCurves[index-1].min_i_n[1]-1; i<listNewPointCurves[index-1].max_i_n[1]+1; i++) {
      double y=poly_3.Y(listNewPointCurves[index-1].new_x[i]);
      listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index-1].new_x[i]);
      listNewPointCurves[index-1].approx_y.append(y);
  }
  QPolygonF polygon_points_3;
  QwtPlotCurve *curve_plot_3 = new QwtPlotCurve();
  listNewPointCurves[index-1].plot_curve_apr3 = curve_plot_3;
  listNewPointCurves[index-1].plot_curve_apr3->setPen( Qt::black, 2 );
  listNewPointCurves[index-1].plot_curve_apr3->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // сглаживание
  int size_3 = listNewPointCurves[index-1].approx_x.size();
        for(int i=size_2; i<size_3; i++) {
          polygon_points_3 << QPointF (listNewPointCurves[index-1].approx_x[i],
                listNewPointCurves[index-1].approx_y[i]);
        }
       listNewPointCurves[index-1].plot_curve_apr3->setSamples(polygon_points_3);
       listNewPointCurves[index-1].plot_curve_apr3->attach(m_plot);

qDebug()<<"approximation()_4";
  // от max2 до min3
//  double min_y = curve.min_y[i]+(150*(count-1));
//  double max_y = curve.max_y[i]+(150*(count-1));
  GlissPoly poly_4(3);
  for(int i=listNewPointCurves[index-1].max_i_n[1]-1; i<listNewPointCurves[index-1].new_x.size(); i++) {
      poly_4.in(listNewPointCurves[index-1].new_x[i], listNewPointCurves[index-1].new_y[i]);
      qDebug()<<"approximation()_4.1";
  }
  for(int i=0; i<listNewPointCurves[index].min_i_n[0]; i++) {
      poly_4.in(listNewPointCurves[index].new_x[i]+listNewPointCurves[index-1].new_x.last(), listNewPointCurves[index].new_y[i]+150);
      qDebug()<<"approximation()_4.2";
  }
  if(!poly_4.solv()) return;
  poly_4.commit();
for(int i=listNewPointCurves[index-1].max_i_n[1]-1; i<listNewPointCurves[index-1].new_x.size(); i++) {
    double y=poly_4.Y(listNewPointCurves[index-1].new_x[i]);
    listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index-1].new_x[i]);
    listNewPointCurves[index-1].approx_y.append(y);
    qDebug()<<"approximation()_4.3";
}
qDebug()<<"approximation()_4.3.3";
for(int i=0; i<listNewPointCurves[index].min_i_n[0]; i++) {
    qDebug()<<"approximation()_4.4";
    double y=poly_4.Y(listNewPointCurves[index].new_x[i]+listNewPointCurves[index-1].new_x.last());
    qDebug()<<"approximation()_4.5";
    listNewPointCurves[index-1].approx_x.append(listNewPointCurves[index].new_x[i]+listNewPointCurves[index-1].new_x.last());
    qDebug()<<"approximation()_4.6";
    listNewPointCurves[index-1].approx_y.append(y);
}
qDebug()<<"approximation()_5";
QPolygonF polygon_points_4;
QwtPlotCurve *curve_plot_4 = new QwtPlotCurve();
listNewPointCurves[index-1].plot_curve_apr4 = curve_plot_4;
listNewPointCurves[index-1].plot_curve_apr4->setPen( Qt::magenta, 2 );
listNewPointCurves[index-1].plot_curve_apr4->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // сглаживание
//QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
//        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 4, 4 ) );
//    curve_plot_4->setSymbol( symbol );
int size_4 = listNewPointCurves[index-1].approx_x.size();
      for(int i=size_3; i<size_4; i++) {
        polygon_points_4 << QPointF (listNewPointCurves[index-1].approx_x[i],
              listNewPointCurves[index-1].approx_y[i]);
      }
     listNewPointCurves[index-1].plot_curve_apr4->setSamples(polygon_points_4);
     listNewPointCurves[index-1].plot_curve_apr4->attach(m_plot);

     listNewPointCurves[index-1].plot_curve_apr1->setVisible(false);
     listNewPointCurves[index-1].plot_curve_apr2->setVisible(false);
     listNewPointCurves[index-1].plot_curve_apr3->setVisible(false);
     listNewPointCurves[index-1].plot_curve_apr4->setVisible(false);

      m_plot->replot();
//    } else {
//        for(int i=0; i<listNewPointCurves[index-1].new_x.size(); i++) {
//            polygon << QPointF (listNewPointCurves[index-1].new_x[i],
//                     listNewPointCurves[index-1].new_y[i]);
//          }
//            listNewPointCurves[index-1].curve->setSamples(polygon);
//            m_plot->replot();
//          }

//*****************************************************************

}

void Osc::paintCurves() {
    qDebug() << "Flag_2.14";
    foreach(QString id, m_curve.keys()) {
        if(id != "Tm") pointCurve(m_curve[id]);
    }
    if (m_plot) m_plot ->replot();
}

void Osc::attach(QwtPlot *plot) {
    qDebug() << "Flag_2.15";
    foreach(QString id, m_curve.keys()) {
        m_curve[id].curve_plot -> attach(plot);
        setCurveParametersOsc(m_curve[id].curve_plot);
    }
    m_plot=plot;
    if (m_plot)  m_plot ->replot();
    qDebug() << "Flag_2.15.1";
}

Osc::Osc(QObject *parent, OSource *src):QObject(parent) {
    qDebug() << "Flag_2.16";
 m_plot=0;
 //stateCheckBox = false;
 init();
 setSrc(src);
}

void Osc::init() {
    qDebug() << "Flag_2.17";
    addCurveDefaultValue("INV:Pa", "В", 1050, 1, true, "INV:Pa");
    addCurveDefaultValue("Tm",  "c", 160000,  1, true,"TIME");
    //     addCurveDefaultValue("tag", unit, max, dec, showValue, data);
}

void Osc::setWidth(int val) {
    qDebug() << "Flag_2.18";
    m_data_width = val;
    paintCurves();
}

void Osc::setDataOffset(int val) {
    qDebug() << "Flag_2.19";
    m_data_offset = val;
    paintCurves();
}
//void Osc::setDataOffsetX(int val) {
//    qDebug() << "Flag_2.19";
//    m_data_offset_x = val;
//    paintCurves();
//}

void Osc::setCurveParametersOsc(QwtPlotCurve *curve_plot) {
    qDebug() << "Flag_1.5";
    //График
    curve_plot->setPen(Qt::green, 2); //цвет и тощина линий
    curve_plot->setRenderHint(QwtPlotItem::RenderAntialiased, true); //сглаживание
    //Маркеры кривой
    m_symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::yellow), QPen(Qt::red), QSize(4, 4));
    curve_plot->setSymbol(m_symbol);
}

int Osc::setCurvePeriod(int value) {
    return curvePeriod = value;
}

void Osc::dumpExcel() {
    //QAxObject* cell = StatSheet->querySubObject("Cells(QVariant,QVariant)", 1, 1);
        // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
       // cell->setProperty("Value", QVariant("Some value"));

    //qDebug()<<"*****dumpExcel*****";
    QString file("F:\\excelFile.xlsx");
    //QString file("F:/excelFile.xlsx");
    QAxObject *excel = new QAxObject("Excel.Application", this);
    excel->dynamicCall("SetVisible(bool)", false);
    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", file);
    QAxObject *sheets = workbook->querySubObject("Worksheets");

    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 1ая страница(Лист1)
    QAxObject *rows = sheet->querySubObject("Rows"); //строки
    QAxObject *columns = sheet->querySubObject("Columns"); //столбцы

    //Рабочая область
    QAxObject *cell = sheet->querySubObject("Cells(int, int)", 1, 5); // 1-строка, 5-столбец
    cell->dynamicCall("SetValue(const QVariant&)", "Dump параметров");

    QAxObject *text1 = sheet->querySubObject("Cells(int, int)", 2, 5);
    text1->dynamicCall("SetValue(const QVariant&)", "min_x_1");
    QAxObject *text2 = sheet->querySubObject("Cells(int, int)", 2, 6);
    text2->dynamicCall("SetValue(const QVariant&)", "min_y_1");
    QAxObject *text3 = sheet->querySubObject("Cells(int, int)", 2, 7);
    text3->dynamicCall("SetValue(const QVariant&)", "i_1");
    QAxObject *interior1 = text3->querySubObject("Interior");//color
    interior1->setProperty("Color", QColor("lightgreen"));

    QAxObject *text4 = sheet->querySubObject("Cells(int, int)", 2, 8);
    text4->dynamicCall("SetValue(const QVariant&)", "max_x_1");
    QAxObject *text5 = sheet->querySubObject("Cells(int, int)", 2, 9);
    text5->dynamicCall("SetValue(const QVariant&)", "max_y_1");
    QAxObject *text6 = sheet->querySubObject("Cells(int, int)", 2, 10);
    text6->dynamicCall("SetValue(const QVariant&)", "i_2");
    QAxObject *interior2 = text6->querySubObject("Interior");//color
    interior2->setProperty("Color", QColor("lightgreen"));

    QAxObject *text7 = sheet->querySubObject("Cells(int, int)", 2, 11);
    text7->dynamicCall("SetValue(const QVariant&)", "min_x_2");
    QAxObject *text8 = sheet->querySubObject("Cells(int, int)", 2, 12);
    text8->dynamicCall("SetValue(const QVariant&)", "min_y_2");
    QAxObject *text9 = sheet->querySubObject("Cells(int, int)", 2, 13);
    text9->dynamicCall("SetValue(const QVariant&)", "i_3");
    QAxObject *interior3 = text9->querySubObject("Interior");//color
    interior3->setProperty("Color", QColor("lightgreen"));

    QAxObject *text10 = sheet->querySubObject("Cells(int, int)", 2, 14);
    text10->dynamicCall("SetValue(const QVariant&)", "max_x_2");
    QAxObject *text11 = sheet->querySubObject("Cells(int, int)", 2, 15);
    text11->dynamicCall("SetValue(const QVariant&)", "max_y_2");
    QAxObject *text12 = sheet->querySubObject("Cells(int, int)", 2, 16);
    text12->dynamicCall("SetValue(const QVariant&)", "i_4");
    QAxObject *interior4 = text12->querySubObject("Interior");//color
    interior4->setProperty("Color", QColor("lightgreen"));

    //Объединение ячеек
    QAxObject *range = sheet->querySubObject("Range(const QVariant&)", QVariant(QString("E1:L1")));
    range->dynamicCall("Select()"); //выделяем
    range->dynamicCall("MergeCells", TRUE);

    //Выравнивание по центру
    QAxObject *centr = sheet->querySubObject("Range(const QVariant&)", QVariant(QString("E1:L1")));
    centr->dynamicCall("Select()");
    centr->dynamicCall("HorizontalAlignment", -4108);

    qDebug()<<"listNewPointCurves.size()"<<listNewPointCurves.size();
    for(int i=0; i<listNewPointCurves.size()-1; i++) {
        QAxObject *dataCurveName = sheet->querySubObject("Cells(int, int)", 3+i, 4);
        dataCurveName -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].id);
        QAxObject *data_min_x1 = sheet->querySubObject("Cells(int, int)", 3+i, 5);
        data_min_x1 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].min_x[0]);
        QAxObject *data_min_y1 = sheet->querySubObject("Cells(int, int)", 3+i, 6);
        data_min_y1 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].min_y[0]);
        QAxObject *data_axis_point1 = sheet->querySubObject("Cells(int, int)", 3+i, 7);
        data_axis_point1 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].axis_sum_point[0]);
        QAxObject *interior1 = data_axis_point1->querySubObject("Interior");//color
        interior1->setProperty("Color", QColor("lightgreen"));

        QAxObject *data_max_x1 = sheet->querySubObject("Cells(int, int)", 3+i, 8);
        data_max_x1 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].max_x[0]);
        QAxObject *data_max_y1 = sheet->querySubObject("Cells(int, int)", 3+i, 9);
        data_max_y1 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].max_y[0]);
        QAxObject *data_axis_point2 = sheet->querySubObject("Cells(int, int)", 3+i, 10);
        data_axis_point2 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].axis_sum_point[1]);
        QAxObject *interior2 = data_axis_point2->querySubObject("Interior");//color
        interior2->setProperty("Color", QColor("lightgreen"));

        QAxObject *data_min_x2 = sheet->querySubObject("Cells(int, int)", 3+i, 11);
        data_min_x2 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].min_x[1]);
        QAxObject *data_min_y2 = sheet->querySubObject("Cells(int, int)", 3+i, 12);
        data_min_y2 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].min_y[1]);
        QAxObject *data_axis_point3 = sheet->querySubObject("Cells(int, int)", 3+i, 13);
        data_axis_point3 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].axis_sum_point[2]);
        QAxObject *interior3 = data_axis_point3->querySubObject("Interior");//color
        interior3->setProperty("Color", QColor("lightgreen"));

        QAxObject *data_max_x2 = sheet->querySubObject("Cells(int, int)", 3+i, 14);
        data_max_x2 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].max_x[1]);
        QAxObject *data_max_y2 = sheet->querySubObject("Cells(int, int)", 3+i, 15);
        data_max_y2 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].max_y[1]);
        QAxObject *data_axis_point4 = sheet->querySubObject("Cells(int, int)", 3+i, 16);
        data_axis_point4 -> dynamicCall("SetValue(const QVariant&)", listNewPointCurves[i].axis_sum_point[3]);
        QAxObject *interior4 = data_axis_point4->querySubObject("Interior");//color
        interior4->setProperty("Color", QColor("lightgreen"));
    }
   //workbook->dynamicCall("Save()");
   workbooks->dynamicCall("Close()");
   delete sheet;
   delete sheets;
   delete workbook;
   delete workbooks;
   delete excel;
   excel->dynamicCall("Quit()");
}
void Osc::setDynoCurve(QList<OFileSourceDyno::DynoCurve> dynoCurveList) {
    this -> dynoCurveList = dynoCurveList;
    for(int i=0; i<dynoCurveList.size(); i++) {
//        qDebug()<<"Osc dyno curve date----"<<dynoCurveList[i].date;
//        qDebug()<<"Osc dyno curve x_dyno----"<<dynoCurveList[i].x_dyno.last();
//        qDebug()<<"Osc dyno curve y_dyno----"<<dynoCurveList[i].y_dyno.last();
    }
    paintDynoCurve();
}
void Osc::paintDynoCurve() {
    for(int index=0; index<dynoCurveList.size(); index++) {
        QPolygonF polygon;
        QwtPlotCurve *curve_plot = new QwtPlotCurve();
        dynoCurveList[index].dyno_plot_curve = curve_plot;
        QColor colors[] = {Qt::cyan, Qt::magenta, Qt::red,
                           Qt::darkRed, Qt::darkCyan, Qt::darkMagenta,
                           Qt::green, Qt::darkGreen, Qt::yellow,
                           Qt::blue};
        dynoCurveList[index].dyno_plot_curve->setPen(colors[qrand() % 10]);
        dynoCurveList[index].dyno_plot_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true); // сглаживание

        double k;
        double max = dynoCurveList[index].x_dyno[0];
        for(int j=0; j<dynoCurveList[index].x_dyno.size(); j++) {
            if(max<dynoCurveList[index].x_dyno[j]) {
                max = dynoCurveList[index].x_dyno[j];
                dynoCurveList[index].point = j;
            }
        }
        k = 180/max;
        //qDebug()<<"max"<<max<<"k"<<k;
        for(int i=0; i<dynoCurveList[index].x_dyno.size(); i++) {
            dynoCurveList[index].x_dyno[i] *= k;
            dynoCurveList[index].y_dyno[i] /= 4500;
          polygon << QPointF (dynoCurveList[index].x_dyno[i],
                dynoCurveList[index].y_dyno[i]);
//          qDebug()<<"X_dyno"<<dynoCurveList[index].x_dyno[i];
//          qDebug()<<"Y_dyno"<<dynoCurveList[index].y_dyno[i];
        }
       dynoCurveList[index].dyno_plot_curve->setSamples(polygon);
       dynoCurveList[index].dyno_plot_curve->attach(m_plot);
       dynoCurveList[index].dyno_plot_curve->setVisible(false);
       dynoCurveList[index].show = false;
    }
   dynoCurveRecount();
   m_plot->replot();
}

void Osc::dynoCurveRecount() {
    for(int index=0; index<dynoCurveList.size(); index++) {
        QPolygonF polygon;
        QwtPlotCurve *curve_plot = new QwtPlotCurve();
        dynoCurveList[index].dyno_plot_curve_recount = curve_plot;
        dynoCurveList[index].dyno_plot_curve_recount->setPen(Qt::black);
        dynoCurveList[index].dyno_plot_curve_recount->setRenderHint(QwtPlotItem::RenderAntialiased, true); // сглаживание

        for(int i=0; i<dynoCurveList[index].x_dyno.size(); i++) {
            if(i>dynoCurveList[index].point) {
                dynoCurveList[index].x_dyno_rec.append((180-dynoCurveList[index].x_dyno[i])+180);
                dynoCurveList[index].y_dyno_rec.append(dynoCurveList[index].y_dyno[i]);
            } else {
                dynoCurveList[index].x_dyno_rec.append(dynoCurveList[index].x_dyno[i]);
                dynoCurveList[index].y_dyno_rec.append(dynoCurveList[index].y_dyno[i]);
            }
            polygon << QPointF (dynoCurveList[index].x_dyno_rec[i],
                  dynoCurveList[index].y_dyno_rec[i]);
        }
        dynoCurveList[index].dyno_plot_curve_recount->setSamples(polygon);
        dynoCurveList[index].dyno_plot_curve_recount->attach(m_plot);
        dynoCurveList[index].dyno_plot_curve_recount->setVisible(false);
        dynoCurveList[index].show_recount = false;
    }
}

QList<OFileSourceDyno::DynoCurve> Osc::getDynoCurve() {
    return dynoCurveList;
}

void Osc::updateCurveDyno(int index, bool show) {
    OFileSourceDyno::DynoCurve *new_curve = &(dynoCurveList)[index];
    new_curve->show = show;
    new_curve->dyno_plot_curve->setVisible(show);
    m_plot->replot();
}

void Osc::updateCurveDynoRecount(int index, bool show) {
    qDebug()<<"updateCurveDynoRecount()"<<"index"<<index<<"show"<<show;
    OFileSourceDyno::DynoCurve *new_curve = &(dynoCurveList)[index];
    new_curve->show_recount = show;
    new_curve->dyno_plot_curve_recount->setVisible(show);
    m_plot->replot();
}

void Osc::calculationCurveDegree() {
    qDebug()<<"calculationCurveDegree()_1";
    for(int index=1; index<listNewPointCurves.size()-1; index++) {
        qDebug()<<"index"<<index;
        for(int i=listNewPointCurves[index].min_i_n[0]; i<listNewPointCurves[index].new_x.size(); i++) {
            qDebug()<<"i"<<i;
            listNewPointCurves[index].x_degree.append(listNewPointCurves[index].new_x[i]);
            listNewPointCurves[index].y_degree.append(listNewPointCurves[index].new_y[i]);
        }
        qDebug()<<"calculationCurveDegree()_1.1";
        for(int i=0; i<listNewPointCurves[index+1].min_i_n[0]; i++) {
            qDebug()<<"i"<<i<<"range"<<listNewPointCurves[index+1].min_i_n[0];
            listNewPointCurves[index].x_degree.append(listNewPointCurves[index+1].new_x[i]+listNewPointCurves[index].new_x.last());
            listNewPointCurves[index].y_degree.append(listNewPointCurves[index+1].new_y[i]+150);
        }
        double n = listNewPointCurves[index].x_degree[0];
        double k=360/(listNewPointCurves[index].x_degree.last()-n);
        QPolygonF polygon_points;
        QwtPlotCurve *curve_plot = new QwtPlotCurve();
        listNewPointCurves[index].plot_curve_degree = curve_plot;
        listNewPointCurves[index].plot_curve_degree->setPen( Qt::magenta, 1 );
        listNewPointCurves[index].plot_curve_degree->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // сглаживание
        qDebug()<<"calculationCurveDegree()_1.2";
        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 2, 2 ) );
            listNewPointCurves[index].plot_curve_degree->setSymbol( symbol );
           // double q = listNewPointCurves[index].y_degree[0];
        for(int i=0; i<listNewPointCurves[index].x_degree.size(); i++) {
            listNewPointCurves[index].x_degree[i] -= n;
            listNewPointCurves[index].x_degree[i] *= k;
            listNewPointCurves[index].y_degree[i] += 150*(index+1);
            listNewPointCurves[index].y_degree[i] /= 100;
            //listNewPointCurves[index].x_degree[i] = (listNewPointCurves[index].x_degree[i]-n)*k;
          polygon_points << QPointF (listNewPointCurves[index].x_degree[i],
                listNewPointCurves[index].y_degree[i]);
        }
        listNewPointCurves[index].plot_curve_degree->setSamples(polygon_points);
        listNewPointCurves[index].plot_curve_degree->attach(m_plot);

        listNewPointCurves[index].plot_curve_degree->setVisible(false);
    }
    m_plot->replot();
    qDebug()<<"calculationCurveDegree()_2";
}

void Osc::setStateCheckBox(bool state) {
    stateCheckBox = state;
}
