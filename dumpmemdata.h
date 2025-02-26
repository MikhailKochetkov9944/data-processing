#ifndef DUMPMEMDATA_H
#define DUMPMEMDATA_H

#include <abstractdata.h>
#include <QWidget>
#include <QtGui>

class OSourceDump
{
public:
  enum Type {Undefined, Signal, Control, Sensor, Setpoint, Bool, Number};
  virtual ~OSourceDump() {};
  virtual unsigned int size() const = 0;
  virtual Type type(const QString & name) const = 0;
  virtual bool valid(const QString & name, unsigned int tick) = 0;
  virtual bool DI(const QString & name, unsigned int tick) = 0;
  virtual float AI(const QString & name, unsigned int tick) = 0;
};

class OFileSourceDump : public OSourceDump
{
    //Q_OBJECT
  typedef struct
  {
    QString name;
    OSourceDump::Type type;
    int offset;
  } Curve;
  QFile *m_fd;


  void addCurve(const QString & type, const QString & name, int *offset);
  OSourceDump::Type str2type(const QString & type);

public:
  QMap<QString, Curve*> m_curves;
  unsigned int m_size;
  unsigned int m_packsize;
  unsigned int m_offset;
  OFileSourceDump();
  virtual ~OFileSourceDump();
  bool open(const QString & filename);
  void close();
  virtual unsigned int size() const;
  virtual OSourceDump::Type type(const QString & name) const;
  virtual bool valid(const QString & name, unsigned int tick);
  virtual bool DI(const QString & name, int unsigned tick);
  virtual float AI(const QString & name, int unsigned tick);
};

class FileArrayDump
{
  int m_fd;
public:
  FileArrayDump(const QString & fn);
  ~FileArrayDump();

  void addValue(float value);
  float value(int offs);
};

class DumpMemData : public AbstractData
{
public:
    DumpMemData(OSourceDump *src=0);
    ~DumpMemData();
    int m_data_width;
private:
    QVector<double> pointArray_x;
    QVector<double> pointArray_y;
    OSourceDump *m_src;
public:
   //OFileSource *ofs;
   void setSrcAbstr(OSourceDump *src);
   void recordData();
   float value(QString data, int tick);
   virtual int size() const override {return m_data_width; };
   virtual float val(int pos) const override {return pointArray_y[pos]; };
   virtual float ord(int pos) const override {return pointArray_x[pos]; };
};

#endif // DUMPMEMDATA_H
