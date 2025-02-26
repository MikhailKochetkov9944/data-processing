#include "graph.h"
#include "ui_graph.h"

#include <qwt_plot.h>
#include <qwt_plot_grid.h>

#include <qwt_legend.h>

#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <qwt_plot_magnifier.h>

#include <qwt_plot_panner.h>

#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>

#include "qwt_plot_grid.h"
#include <qwt_symbol.h>

#include <QtSql>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QFileDialog>

#include <abstractdata.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>


Graph::Graph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graph)
{
    ui->setupUi(this);
    m_osc = new Osc(this, &m_src);
    m_dump_mem_data = new DumpMemData(&m_src_dump);
    curve_paint = new Curve();

    m_src_d = new OFileSourceDyno();
    tree = new CurveTree();
    tree_dyno = new CurveTreeDyno();
    dump_data = new DumpDataXlsx();
    ui -> gridLayout -> addWidget(tree);
    ui -> gridLayout -> addWidget(tree_dyno);
    connect(this, SIGNAL(changePointsSignal()), this, SLOT(loadingIdealPoints()));
    //connect(m_osc, SIGNAL(getCurves(QList <Osc::NewPointCurve>*)), tree, SLOT(setCurves(QList <Osc::NewPointCurve>*)));
    connect(tree, SIGNAL(curveChanged(bool, int)), m_osc, SLOT(updateCurve(bool, int)));
    connect(tree, SIGNAL(curveOffsetChanged(int, double)), m_osc, SLOT(updateCurveOffset(int, double)));
    connect(tree, SIGNAL(signalCurveApprox(int, bool)), curve_paint, SLOT(updateCurvePoly(int, bool)));
    connect(this, SIGNAL(signalCurvePeriod(int)), curve_paint, SLOT(setCurvePeriod(int)));
//    connect(m_osc, SIGNAL(signalSetData(double, double, double, double, double, double, double, double)),
//            tree, SLOT(getData(double, double, double, double, double, double, double, double)));
    connect(curve_paint, SIGNAL(signalDumpXlsx(QVector<QVector<AbstractData*>>)), dump_data, SLOT(setResultData(QVector<QVector<AbstractData*>>)));
    connect(tree_dyno, SIGNAL(curveChangedDyno(int, bool)), m_osc, SLOT(updateCurveDyno(int, bool)));
    connect(tree_dyno, SIGNAL(curveChangedDynoRecount(int, bool)), m_osc, SLOT(updateCurveDynoRecount(int, bool)));
    //connect(m_src, SIGNAL(signalDynoCurve(QList<OFileSource::DynoCurve>)), m_osc, SLOT(setDynoCurve(QList<OFileSource::DynoCurve>)));
    //connect(tree, SIGNAL(curveChanged(QList<Osc::NewPointCurve>*, int)), m_osc, SLOT(updateCurve(QList<Osc::NewPointCurve>*, int)));
    //connect(tree, SIGNAL(curveStateChanged(QList<Osc::NewPointCurve>*)), m_osc, SLOT(setCurves(QList<Osc::NewPointCurve>*)));
    connect(curve_paint, SIGNAL(setCurves(QVector<QVector<AbstractData*>>)), tree, SLOT(getCurves(QVector<QVector<AbstractData*>>)));
    connect(tree, SIGNAL(curveChanged(int, bool)), curve_paint, SLOT(updateCurve(int, bool)));
    //Создание поля со шкалами
    setPlot();
    //Включение масштабной сетки
    setPlotGrid();
    //График
    setCurveParameters();
    setToolBar();

}

void Graph::mousePressEvent(QMouseEvent *event) {
    if(event->button()==Qt::RightButton) {
        a_plot->setAxisAutoScale(QwtPlot::xBottom, true);
        a_plot->setAxisAutoScale(QwtPlot::yLeft, true);
        a_plot->replot();

    }
}

void Graph::openFile() {
    QFileDialog fd(this, tr("Open dump file"), "", tr("Dump files (dump.*)"));
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Hidden | QDir::Files);
    if(fd.exec())
    {
      if(m_src_dump.open(fd.selectedFiles()[0]))
      {
        //opened();
        //m_osc->attach(a_plot);
        //m_osc->setSrc(&m_src);
        //m_osc->setWidth(1000);
        //m_osc->setDataOffset(10000);
        m_dump_mem_data->setSrcAbstr(&m_src_dump);
        m_dump_mem_data->recordData();
        m_osc->attach(curve_paint->b_plot);
        curve_paint->curveResult(m_dump_mem_data);
      }
    }
}
void Graph::openFileDyno() {
    QFileDialog fd(this, tr("Open dump file"), "", tr("Dump files (dyn-*)"));
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Hidden | QDir::Files);
    if(fd.exec())
    {
       // m_src.openDyno(fd.selectedFiles()[0]);
        m_osc->setDynoCurve(m_src_d->openDyno(fd.selectedFiles()[0]));
        tree_dyno->setCurveDyno(m_osc->getDynoCurve());
    }
}
void Graph::setToolBar() {
    toolBar = new QToolBar(this);
    layoutToolBar = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layoutToolBar -> setContentsMargins(0, 0, 0, 0);
    layoutToolBar -> addWidget(toolBar);

//    toolBar->setOrientation(Qt::Vertical);
//    layoutToolBar->setDirection(QBoxLayout::LeftToRight);
//    layoutToolBar ->addLayout(buttonLayout);

}
void Graph::setPlot() {
    a_plot = new QwtPlot(this);
    a_plot->setTitle("Graph Tn");
    a_plot->setCanvasBackground(Qt::white);
    a_plot->setAxisTitle(QwtPlot::yLeft, "Tn");
    a_plot->setAxisTitle(QwtPlot::xBottom, "Angle, degree");
    ui->framelLayout->insertWidget(0, a_plot);
    //m_osc->attach(a_plot);
    // приближения/удаления графика
    QwtPlotMagnifier* a_magnifier = new QwtPlotMagnifier(a_plot->canvas());
    // клавиша, активирующая приближение/удаление
    a_magnifier->setMouseButton(Qt::MidButton);
    //перемешение по графику
    QwtPlotPanner* a_panner = new QwtPlotPanner(a_plot->canvas());
    // клавиша, активирующая перемещение
    a_panner->setMouseButton(Qt::LeftButton);
    curve_paint->attach(a_plot);
}
void Graph::setPlotGrid() {
    //Cетка
    grid = new QwtPlotGrid();
    grid ->setMajorPen(QPen(Qt::gray, 2));
    grid -> attach(a_plot);
}
void Graph::setCurveParameters() {
    //График
    curve = new QwtPlotCurve();
    curve->setTitle("Тn");
    curve->setPen(Qt::red, 2); //цвет и тощина линий
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true); //сглаживание
    //Маркеры кривой
    symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::yellow), QPen(Qt::red), QSize(4, 4));
    curve->setSymbol(symbol);
}
void Graph::loadingIdealPoints() {
    points.clear();
    QSqlQuery query;
    for(int i=1; i<26; i++) {
        query.prepare("SELECT angle, tN FROM param WHERE id=:i");
        query.bindValue(":i", i);
        query.exec();
        while (query.next()) {
            pointIdealArray[i-1][0] = query.value(0).toDouble();
            pointIdealArray[i-1][1] = query.value(1).toDouble();
        }
          points << QPointF (pointIdealArray[i-1][0], pointIdealArray[i-1][1]);
    }
    curve->setSamples(points); // ассоциировать набор точек с кривой
    curve->attach(a_plot); //отобразить кривую
    a_plot->replot();
}
Graph::~Graph()
{
    delete ui;
}

void Graph::on_pushButton_back_clicked()
{
    emit ToMainWindow();
    this -> close();
}

void Graph::on_pushButton_clicked()
{
    emit changePointsSignal();
}

void Graph::on_pushButton_2_clicked()
{
    openFile();
}


void Graph::opened() {
    m_actFileOpen->setEnabled(false);
    m_actFileClose->setEnabled(true);
}

void Graph::closed() {
    m_actFileOpen->setEnabled(true);
    m_actFileClose->setEnabled(false);
}

void Graph::on_spinBox_valueChanged(int arg1)
{
    emit signalCurvePeriod(arg1);
}

void Graph::on_pushButton_dumpExcel_clicked()
{
    curve_paint->createDumpXlsx();
}


void Graph::on_pushButton_dyno_clicked()
{
    openFileDyno();
}

void Graph::on_checkBox_toggled(bool checked)
{
   // m_osc->setStateCheckBox(checked);
}
