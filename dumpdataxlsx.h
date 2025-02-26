#ifndef DUMPDATAXLSX_H
#define DUMPDATAXLSX_H

#include "xlsxwriter.h"
#include <abstractdata.h>

class DumpDataXlsx : public QObject {
    Q_OBJECT
public:
    DumpDataXlsx(QObject *parent = 0) {};
    ~DumpDataXlsx() {};
    int writerDump();
public slots:
    void setResultData(QVector<QVector<AbstractData*>> result);
private:
    QVector<QVector<AbstractData*>> result;
};

#endif // DUMPDATAXLSX_H
