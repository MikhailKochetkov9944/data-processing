#include "viewparam.h"
#include <stdlib.h>
#include "ui_parameter.h"
void ViewParam::view(Ui::Parameter *ui) {
    Storage st;
    ui -> label_ui_f -> setText(QString::number(st.getF()));
    ui -> label_ui_vd -> setText(QString::number(st.getVd()));
    ui -> label_ui_m_ur -> setText(QString::number(st.getMur()));
    ui -> label_ui_a -> setText(QString::number(st.getA()));
    ui -> label_ui_c -> setText(QString::number(st.getC()));
    ui -> label_ui_p -> setText(QString::number(st.getP()));
    ui -> label_ui_k -> setText(QString::number(st.getK()));
    ui -> label_ui_r -> setText(QString::number(st.getR()));
    ui -> label_ui_fi -> setText(QString::number(st.getFi()));
}
