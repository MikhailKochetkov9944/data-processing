#include "dumpdataxlsx.h"
#include <QDebug>

//DumpDataXlsx::DumpDataXlsx()
//{

//}

void DumpDataXlsx::setResultData(QVector<QVector<AbstractData*>> result) {
    qDebug()<<"DumpDataXlsx::setResultData()";
    this->result = result;
    writerDump();
}

int DumpDataXlsx::writerDump() {

    lxw_workbook *workbook = workbook_new("dumpxlsx.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, "dump");

    int row = 0;
    int col = 0;
    for(int i=0; i<=3; i++) {
        if(i<2) {
            QString min_x = "min_x_" + QString::number(i+1);
            QString max_x = "max_x_" + QString::number(i+1);
            QString min_y = "min_y_" + QString::number(i+1);
            QString max_y = "max_y_" + QString::number(i+1);
            worksheet_write_string(worksheet, row, col+1+i*14,  min_x.toLocal8Bit().data(), NULL);
            worksheet_write_string(worksheet, row, col+2+i*14,  min_y.toLocal8Bit().data(), NULL);
            worksheet_write_string(worksheet, row, col+8+i*14,  max_x.toLocal8Bit().data(), NULL);
            worksheet_write_string(worksheet, row, col+9+i*14,  max_y.toLocal8Bit().data(), NULL);
        }
       QString i_num = "i_" + QString::number(i+1);
       worksheet_write_string(worksheet, row, col+7+i*7,  i_num.toLocal8Bit().data(), NULL);
    }
    for(int i=3; i>=0;i--) {
        QString curve_name_apr = "K_approx_" + QString::number(i);
        worksheet_write_string(worksheet, row, col+3+(3-i),  curve_name_apr.toLocal8Bit().data(), NULL);
        worksheet_write_string(worksheet, row, col+10+(3-i),  curve_name_apr.toLocal8Bit().data(), NULL);
        worksheet_write_string(worksheet, row, col+17+(3-i),  curve_name_apr.toLocal8Bit().data(), NULL);
        worksheet_write_string(worksheet, row, col+24+(3-i),  curve_name_apr.toLocal8Bit().data(), NULL);
    }

    for(row=0; row<result.size()-1; row++) {
       QString curve_name = "Curve_" + QString::number(row);
       worksheet_write_string(worksheet, row+1, col,  curve_name.toLocal8Bit().data(), NULL);
       worksheet_write_number(worksheet, row+1, col+1, result[row][1]->ord(0), NULL);
       worksheet_write_number(worksheet, row+1, col+2,  result[row][1]->val(0), NULL);
       PolyFragment* poly_1 = dynamic_cast<PolyFragment*>(result[row][6]);
       poly_1->getPoly().getWorksheet(worksheet, row+1, col+3);
       worksheet_write_number(worksheet, row+1, col+7,  result[row][1]->size(), NULL);

       worksheet_write_number(worksheet, row+1, col+8, result[row][1]->ord(result[row][1]->size()), NULL);
       worksheet_write_number(worksheet, row+1, col+9,  result[row][1]->val(result[row][1]->size()), NULL);
       PolyFragment* poly_2 = dynamic_cast<PolyFragment*>(result[row][7]);
       poly_2->getPoly().getWorksheet(worksheet, row+1, col+10);
       worksheet_write_number(worksheet, row+1, col+14,  result[row][2]->size(), NULL);

       worksheet_write_number(worksheet, row+1, col+15, result[row][3]->ord(0), NULL);
       worksheet_write_number(worksheet, row+1, col+16,  result[row][3]->val(0), NULL);
       PolyFragment* poly_3 = dynamic_cast<PolyFragment*>(result[row][8]);
       poly_3->getPoly().getWorksheet(worksheet, row+1, col+17);
       worksheet_write_number(worksheet, row+1, col+21,  result[row][3]->size(), NULL);
       worksheet_write_number(worksheet, row+1, col+22, result[row][3]->ord(result[row][1]->size()), NULL);
       worksheet_write_number(worksheet, row+1, col+23,  result[row][3]->val(result[row][1]->size()), NULL);
       PolyFragment* poly_4 = dynamic_cast<PolyFragment*>(result[row][9]);
       poly_4->getPoly().getWorksheet(worksheet, row+1, col+24);
       worksheet_write_number(worksheet, row+1, col+28,  result[row][4]->size(), NULL);
    }
//    lxw_error error = workbook_close(workbook);
//    if(error) qDebug()<<"Error write xlsx file";
    return workbook_close(workbook);
}
