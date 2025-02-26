#include "curvetree.h"
#include <QCheckBox>

CurveTree::CurveTree(QWidget *parent) : QTreeWidget(parent)
{
    QStringList labels;
    labels << "sh." << "Обозначение"  << "point" << "Смещение" << "apr"<<"min"<<"max"
           <<"min"<<"max"<<"t0"<<"tn";
    setHeaderLabels(labels);
    setColumnWidth(SHOW, 20);
    setColumnWidth(NAME, 85);
    setColumnWidth(POINT, 30);
    setColumnWidth(OFFSET, 80);
    setColumnWidth(APPROX, 15);
    setColumnWidth(MIN_1, 20);
    setColumnWidth(MAX_1, 20);
    setColumnWidth(MIN_2, 20);
    setColumnWidth(MAX_2, 20);
    setColumnWidth(T_0, 20);
    setColumnWidth(T_n, 20);
    setFixedWidth(30+25+120+100+10+250+30);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

//void CurveTree::setCurves(QList <Osc::NewPointCurve> *listNewPointCurves) {
//    qDebug() << "!!!!!!!!!!!curveTree";
//    this -> listNewPointCurves = listNewPointCurves;
//    clear();
//    fillTree();
//}

void CurveTree::getCurves(QVector<QVector<AbstractData*>> result) {
    qDebug()<<"getCurves_1";
    this->result = result;
    qDebug()<<"getCurves_2";
    clear();
    qDebug()<<"getCurves_3";
    fillTree();
    qDebug()<<"getCurves_4";
}


void CurveTree::fillTree() {
    qDebug()<<"fillTree_1";
    qDebug()<<"result->size()"<<result.size();
    for(int i=0; i<result.size(); i++) {
        qDebug()<<"fillTree() i:"<<i;
        //QVector<Fragment> res_frag = result[i].last();
        QString curve_name = "Curve_" + QString::number(i+1);
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item -> setText(NAME, curve_name);
        addTopLevelItem(item);
        //updateItem(item, &res_frag, i);
        updateItem(item, i);

    }
    qDebug()<<"fillTree_2";
}

void CurveTree::updateItem(QTreeWidgetItem *item, int i) {
    qDebug()<<"updateItem()_1";
    QCheckBox *checkBoxShow = new QCheckBox;
    checkBoxShow->setObjectName("CurveShow_" + QString::number(i+1));
    setItemWidget(item, SHOW, checkBoxShow);
    connect(checkBoxShow, SIGNAL(toggled(bool)), this, SLOT(curveStateChanged(bool)));
    qDebug()<<"updateItem()_2";

    QCheckBox *checkBoxApprox = new QCheckBox;
    checkBoxApprox->setObjectName("Approx_" + QString::number(i+1));
    setItemWidget(item, APPROX, checkBoxApprox);
    connect(checkBoxApprox, SIGNAL(toggled(bool)), this, SLOT(curveApprox(bool)));
}

void CurveTree::curveStateChanged(bool show) {
    QCheckBox *showCurve = qobject_cast<QCheckBox*>(sender());
    int index = showCurve->objectName().remove("CurveShow_").toInt();
    emit curveChanged(index, show);
}


void CurveTree::curveApprox(bool approx) {
    QCheckBox *curveApprox = qobject_cast<QCheckBox*>(sender());
    int index = curveApprox->objectName().remove("Approx_").toInt();
    emit signalCurveApprox(index, approx);
}

