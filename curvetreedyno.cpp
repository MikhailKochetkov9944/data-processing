#include "curvetreedyno.h"
#include <QCheckBox>

CurveTreeDyno::CurveTreeDyno(QWidget *parent) : QTreeWidget(parent)
{
    QStringList labels;
    labels << "show" << "name"<<"recount";
    setHeaderLabels(labels);
    setColumnWidth(SHOW, 20);
    setColumnWidth(NAME, 300);
    setColumnWidth(RECOUNT, 50);
    setFixedWidth(565);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void CurveTreeDyno::setCurveDyno(QList<OFileSourceDyno::DynoCurve> dynoCurveList) {
    this -> dynoCurveList = dynoCurveList;
    clear();
    fillTree();
}

void CurveTreeDyno::fillTree() {
    for(int i=0; i<dynoCurveList.count(); i++) {
        dynoCurveList[i].id = i;
        OFileSourceDyno::DynoCurve *newCurve = &dynoCurveList[i];
        QStringList curveName = newCurve->date.split(",");
        //qDebug() << "curveName" << curveName;
        //qDebug() << "i" << i;
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item -> setText(NAME, newCurve->date);
        addTopLevelItem(item);
        updateItem(item, newCurve);
    }
}

void CurveTreeDyno::updateItem(QTreeWidgetItem *item, OFileSourceDyno::DynoCurve *dynoCurve) {
    //qDebug()<<"updateItem()";
        QCheckBox *checkBoxShow = new QCheckBox;
        checkBoxShow->setObjectName(QString::number(dynoCurve->id));
        checkBoxShow->setChecked(dynoCurve->show);
        setItemWidget(item, SHOW, checkBoxShow);
        connect(checkBoxShow, SIGNAL(toggled(bool)), this, SLOT(curveStateChanged(bool)));

        QCheckBox *checkBoxRecount = new QCheckBox;
        checkBoxRecount->setObjectName(QString::number(dynoCurve->id));
        checkBoxRecount->setChecked(dynoCurve->show_recount);
        setItemWidget(item, RECOUNT, checkBoxRecount);
        connect(checkBoxRecount, SIGNAL(toggled(bool)), this, SLOT(curveStateRecount(bool)));
}

void CurveTreeDyno::curveStateChanged(bool show) {
    QCheckBox *showCurve = qobject_cast<QCheckBox*>(sender());
    int index = showCurve->objectName().toInt();
    dynoCurveList[index].show = show;
    emit curveChangedDyno(index, show);
}

void CurveTreeDyno::curveStateRecount(bool show) {
    QCheckBox *showCurve = qobject_cast<QCheckBox*>(sender());
    int index = showCurve->objectName().toInt();
    dynoCurveList[index].show_recount = show;
    emit curveChangedDynoRecount(index, show);
}
