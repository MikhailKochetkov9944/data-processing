#include "dumpmemdata.h"
#include <fstream>
#include <QMessageBox>

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

OFileSourceDump::OFileSourceDump()
  : m_size(0),
    m_packsize(0),
    m_offset(0),
    m_fd(0)
{}

bool OFileSourceDump:: open(const QString & filename) {
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

void OFileSourceDump::close()
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

OFileSourceDump::~OFileSourceDump()
{
    qDebug() << "Flag_2.2";
    close();
}

unsigned int OFileSourceDump::size() const
{
    qDebug() << "Flag_2.3";
  return m_size;
}

OSourceDump::Type OFileSourceDump::type(const QString & name) const
{
    qDebug() << "Flag_2.4";
  QMap<QString, Curve*>::const_iterator i = m_curves.find(name);
  if (i == m_curves.end()) return OSourceDump::Undefined;
  Curve *curve = i.value();
  return curve->type;
}

bool OFileSourceDump::valid(const QString & name, unsigned int tick)
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
     case OSourceDump::Signal:
     case OSourceDump::Control:
       {
         Digital val;
         m_fd->read((char*)&val, sizeof(Digital));
         return val.valid;
       }

     case OSourceDump::Sensor:
     case OSourceDump::Setpoint:
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

bool OFileSourceDump::DI(const QString & name, unsigned int tick)
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
    case OSourceDump::Signal:
    case OSourceDump::Control:
      {
        Digital val;
        m_fd->read((char*)&val, sizeof(Digital));
        return val.value;
      }
    case OSourceDump::Bool:
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


float OFileSourceDump::AI(const QString & name, unsigned int tick){
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
    case OSourceDump::Signal:
    case OSourceDump::Control:
      {
        Digital val;
        m_fd->read((char*)&val, sizeof(Digital));
        return val.value?1:0;
      }

    case OSourceDump::Sensor:
    case OSourceDump::Setpoint:
      {
        Analog val;
        m_fd->read((char*)&val, sizeof(Analog));
        return val.value;
      }
    case OSourceDump::Bool:
      {
        char val;
        m_fd->read((char*)&val, sizeof(char));
        return val?1:0;
      }
    case OSourceDump::Number:
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

OSourceDump::Type OFileSourceDump::str2type(const QString & type)
{
    qDebug() << "Flag_2.8";
  if(type == "SIGNAL")
    return OSourceDump::Signal;
  else if(type == "CONTROL")
    return OSourceDump::Control;
  else if(type == "SENSOR")
    return OSourceDump::Sensor;
  else if(type == "SETPOINT")
    return OSourceDump::Setpoint;
  else if(type == "BOOL")
    return OSourceDump::Bool;
  else if(type == "NUMBER")
    return OSourceDump::Number;
  else
  {
    QMessageBox::information(0, "Bad dump file", "Wrong type of element - " + type);
    assert(0);
  }
}

void OFileSourceDump::addCurve(const QString & type, const QString & name, int *offset)
{
    qDebug() << "Flag_2.9";
  Curve *curve = new Curve;
  curve->name = name;
  curve->type = str2type(type);
  curve->offset = *offset;
  switch(curve->type)
  {
    case OSourceDump::Signal:
    case OSourceDump::Control:
      *offset += sizeof(Digital);
      break;
    case OSourceDump::Sensor:
    case OSourceDump::Setpoint:
      *offset += sizeof(Analog);
      break;
    case OSourceDump::Bool:
      *offset += sizeof(char);
      break;
    case OSourceDump::Number:
      *offset += sizeof(float);
      break;
    default:
      assert(0);
  }
  m_curves.insert(name, curve);
}


DumpMemData::DumpMemData(OSourceDump *src) {
    setSrcAbstr(src);
}
void DumpMemData::setSrcAbstr(OSourceDump *src) {
    m_src=src;
}

float DumpMemData::value(QString data, int tick)
{
  if(tick >= m_src->size()) return 0;
  switch(m_src->type(data))
  {
    case OSourceDump::Signal:
    case OSourceDump::Control:
    case OSourceDump::Bool:
      return m_src->DI(data, tick)?1:0;
    case OSourceDump::Sensor:
    case OSourceDump::Setpoint:
    case OSourceDump::Number:
      return m_src->AI(data, tick);
    default:
      return 0;
  }
  return 0;
}

void DumpMemData::recordData() {
  int m_data_offset = 10000;
  m_data_width = 1000;
  for(int x=0, i=0;x<m_data_width;x++, i++)
  {
    pointArray_x.append(value("TIME", m_data_offset+x));
    pointArray_y.append(value("INV:Pa", m_data_offset+x));
    int v = pointArray_y[i];
    if(v&0x8000) pointArray_y[i] -= 65536;
    qDebug()<<"x"<<pointArray_x[i]<<"y"<<pointArray_y[i]<<"i-"<<i;
    }
}
