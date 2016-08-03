#include "widget.h"
#include "backend.h"
#include "ui_widget.h"
#include <QApplication>
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QGroupBox>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_series_data.h>
#include <qwt_plot_renderer.h>
#include<qwt_plot_spectrogram.h>

#include "math.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    createContents();
}


void Widget::createContents()
{
    this->setMinimumSize(700, 600);
    this->setWindowTitle("Neuronet Architector");
    fcc = fcalc();
    pfcc = &fcc;
    g = fgenc(pfcc);
    qtw = new QTabWidget;
    qtw->addTab(new QWidget, "Neuronet Architecture\0");
    qtw->addTab(new QWidget, "Neuronet Configuration\0");
    qtw->addTab(new QWidget, "Teaching Set\0");
    qtw->addTab(new QWidget, "Plot\0");
    qtw->addTab(new QWidget, "Data");
        //0 Neuronet Architecture
        bNewNN = new QPushButton("New", qtw->widget(0));
        bNewNN->setGeometry(2, 2, b_width, b_height);
        bImportNN = new QPushButton("Import", qtw->widget(0));
        bImportNN->setGeometry(2, b_height+7, b_width, b_height);
        toolBar = new QToolBar();
        btnExport = new QToolButton( toolBar );
        btnExport->setText( "Export plot" );
        toolBar->addWidget( btnExport );
        connect(btnExport, SIGNAL(clicked()), this, SLOT(exportPlot()));
        QToolButton * gen_buttons[4];
        for(int i=0; i<4;i++)
        {
            gen_buttons[i] = new QToolButton(toolBar);
            toolBar->addWidget(gen_buttons[i]);
        }
        gen_buttons[0]->setText("Import population from txt");

//
        gen_buttons[2]->setText("...");
        gen_buttons[3]->setText("About");


        //1 Neuronet Configuration
        l1 = new QVBoxLayout(qtw->widget(1));
        pte = new QPlainTextEdit();
        l1->addWidget(pte);

        //3 Plot
        l3 = new QVBoxLayout(qtw->widget(3));
        hblfunc_inf = new QHBoxLayout(qtw->widget(3));
        QLabel * lF = new QLabel();
        lF->setText("f = ");
        lF->setFixedWidth(45);
        hblfunc_inf->addWidget(lF);
        tFunc = new QTextEdit;
        tFunc->setFixedHeight(b_height);
        hblfunc_inf->addWidget(tFunc);

        bDrawPlot = new QPushButton;
        bDrawPlot->setText("=");
        bDrawPlot->setMaximumWidth(23);
        connect(bDrawPlot, SIGNAL(clicked()), this, SLOT(plot_f()));
        hblfunc_inf->addWidget(bDrawPlot);

//-------------------------------------------------------
        hblout = new QHBoxLayout(qtw->widget(3));

        QLabel* vbls = new QLabel();
        vbls->setText("variables ");
        vbls->setFixedWidth(45);
        hblout->addWidget(vbls);
        tVars = new QTextEdit();
        tVars->setFixedHeight(b_height);
        hblout->addWidget(tVars);
        QLabel* ldx = new QLabel();
        ldx->setText("dx = ");
        hblout->addWidget(ldx);
        tdx = new QTextEdit();
        tdx->setFixedHeight(b_height);
        tdx->setMaximumWidth(50);
        tdx->setText("0.1");
        hblout->addWidget(tdx);

        QLabel * lL = new QLabel();
        lL->setText("limits ");
        hblout->addWidget(lL);
        tLimits = new QTextEdit;
        tLimits->setFixedHeight(b_height);
        hblout->addWidget(tLimits);

        cbreplot = new QCheckBox();
        cbreplot->setChecked(true);
        cbreplot->setText("redraw");
        hblout->addWidget(cbreplot);
        bFout = new QPushButton();
        bFout->setText("->");
        connect(bFout, SIGNAL(clicked()), this, SLOT(calc_f()));
        hblout->addWidget(bFout);
        lout = new QLabel();
        hblout->addWidget(lout);
 //--------------------------------------------
       QHBoxLayout * gbl_gen = new QHBoxLayout();
       bGenRand = new QPushButton();
       bGenRand->setText("Generate random");
       connect(bGenRand, SIGNAL( clicked()), SLOT(gen_rand_p()) );
       bGenNext = new QPushButton();
       bGenNext->setText("Next epoch");
       connect(bGenNext, SIGNAL( clicked()), SLOT(next_gen()) );
       gbl_gen->addWidget(bGenRand);
       gbl_gen->addWidget(bGenNext);

       QLabel* lgc = new QLabel();
       lgc->setText("generation count = ");
       tgencount = new QTextEdit();
       tgencount->setMaximumWidth(50);
       tgencount->setText("1");
       gbl_gen->addWidget(lgc);
       gbl_gen->addWidget(tgencount);
       lopt = new QLabel();
       lopt->setText(" Opt = ");
       gbl_gen->addWidget(lopt);
        gbl_gen->addStretch(1);
        QGroupBox * box2 = new QGroupBox();
        box2->setLayout(hblfunc_inf);
        box2->setMaximumHeight(50);
        l3->addWidget(box2);

        QGroupBox * box1 = new QGroupBox();
        box1->setLayout(hblout);
        l3->addWidget(box1);
        QGroupBox * box3 = new QGroupBox();
        box3->setLayout(gbl_gen);
        box1->setMaximumHeight(50);
        box3->setMaximumHeight(50);
        l3->addWidget(box3);

//------------------------------------------------------------

        create_plots();
        l3->addWidget(FPlot);

        L0 = new QVBoxLayout;
        L0->addWidget(toolBar);
        qtw->setCurrentIndex(3);
        L0->addWidget(qtw);
        this->setLayout(L0);
}
void Widget::create_plots()
{
    FPlot = new QwtPlot;
    FPlot->setTitle("Objective Function");
    FPlot->setAxisScale(QwtPlot::xBottom, 0, 100);
    FPlot->setAxisScale(QwtPlot::yLeft, 0, 100);
//        FPlot->setAutoFillBackground(true);
//        QPalette p = FPlot->palette();
//        p.setColor(QPalette::Window, Qt::white);
//        FPlot->setPalette(p);

    FCurve2 = new QwtPlotCurve;
    FCurve2->setStyle(QwtPlotCurve::Dots);
    QPen pen2 = QPen(Qt::black, 4.0);
    FCurve2->setRenderThreadCount( 0 );
    FCurve2->setPen(pen2);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::plot_f()
{
    if(initFunc()!=0)
                return;
    try
    {
        int n = fcc.varCount();
        switch(n)
        {
        case 0:
            throw "not a function";
            break;
        case 1:
            curvePlot();
            break;
        case 2:
            contourPlot();
            break;
        default:
            throw "too many variables for plot";
        }

    }
    catch(...)
    {
        QMessageBox::about(this, tr("Error"), "Error  in function");
    }
//    catch(exception& e)
//    {
//        QMessageBox::about(this, tr("Error in plotting"), QString(e.what()));
//    }
}
double Widget::getdx()
{
    QString s = tdx->toPlainText();
    try
    {
    return s.toDouble();
    }
    catch(...)
    {
    tdx->setText("0.1");
    return 0.1;
    }
}
bool Widget::freplot()
{
    return (cbreplot->isChecked())?true:false;
}
void Widget::getlimits(int n, int ab[])
{
    QString s = tLimits->toPlainText();
    if(s=="")
    {
        int i=0;
        while(i<n*2)
        {
            ab[i++]=0;
            if(i<n*2)
            ab[i++]=100;
        }
        return;
    }
    string text = s.toUtf8().constData();
    QList<QString> list = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(list.size()!=(n*2))
        throw "wrong count of limits";
    QList<QString>::iterator i;
    int j;
    for(j=0, i = list.begin(); i != list.end(), j<2*n; ++i, j++)
    ab[j] = (*i).toInt();
    for(j=0;j<n*2;j+=2)
        if(ab[j]>ab[j+1])
        throw "wrong borders of limits";
}
void Widget::curvePlot()
{
    try
    {
        if(freplot())
        FPlot->detachItems();
        FCurve = new QwtPlotCurve;
        FCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
        QPen pen1 = QPen(Qt::blue);
        FCurve->setPen(pen1);
        //FCurve->detach();
        double dx=getdx();
        int n[4];
        // [a, b]
        getlimits(2, n);
        int N=(n[1]-n[0])/dx;
        double x[N+1];
         //
        QPolygonF f_points;
        x[0] = n[0];
        f_points<<QPointF(x[0], fcc.calculate(x[0]));
        for(int i=1; i<N; i++)
        {
            x[i] = x[i-1]+dx;
            f_points<<QPointF(x[i], fcc.calculate(x[i]));
        }

        QString varName(QString::fromStdString(fcc.varName()));
        QString fName("f("+varName+")");
        FPlot->setAxisTitle(QwtPlot::yLeft, fName);
        FPlot->setAxisTitle(QwtPlot::xBottom, varName);
        FPlot->setAxisScale(QwtPlot::xBottom, n[0], n[1]);
        FPlot->setAxisScale(QwtPlot::yLeft, n[2], n[3]);

        FCurve->setSamples(f_points);
        FCurve->attach(FPlot);
        FPlot->replot();
    }
    catch(...)
    {
        QMessageBox::about(this, tr("Error"), "Error  in plotting");
    }
//    catch(exception& e)
//    {
//        QMessageBox::about(this, tr("Error in plotting"), QString(e.what()));
//    }
}
Widget::SpectrogramData::SpectrogramData(fcalc * pfc)
{
fc = pfc;
}
double Widget::SpectrogramData::value( double x, double y ) const
{
    double arr[2];
    arr[0]=x;
    arr[1]=y;
    return fc->calculate(arr);
}
void Widget::contourPlot()
{
    try
    {
        if(freplot())
         FPlot->detachItems();
        list<string> vn = fcc.varNames();
        FPlot->setAxisTitle(QwtPlot::yLeft, QString::fromStdString(vn.back()));
        vn.pop_back();
        FPlot->setAxisTitle(QwtPlot::xBottom, QString::fromStdString(vn.back()));
        FSpectr = new QwtPlotSpectrogram();
        FSpectr->setRenderThreadCount( 0 ); // use system specific thread count
        SpectrogramData *  sd = new SpectrogramData(&fcc);
        int lim[6];
        getlimits(3, lim);
        FSpectr->detach();
        FPlot->setAxisScale(QwtPlot::xBottom, lim[0], lim[1]);
        FPlot->setAxisScale(QwtPlot::yLeft, lim[2], lim[3]);
        sd->setInterval( Qt::XAxis, QwtInterval( lim[0], lim[1]) );
        sd->setInterval( Qt::YAxis, QwtInterval( lim[2], lim[3]) );
        sd->setInterval( Qt::ZAxis, QwtInterval( lim[4], lim[5]) );
        FSpectr->setData( sd);
        FSpectr->setCachePolicy( QwtPlotRasterItem::PaintCache );
        FSpectr->setColorMap( new ColorMap() );
        FSpectr->attach(FPlot);
        FPlot->replot();
    }
//    catch(exception& e)
//    {
//        QMessageBox::about(this, tr("Error in plotting"), QString(e.what()));
//    }
    catch(...)
    {
        QMessageBox::about(this, tr("Error"), "Error  in plotting");
    }

}
int Widget::initFunc()
{
    QString s = tFunc->toPlainText();
    std::string text = s.toUtf8().constData();
    int res = fcc.lexer(text);

    if(res!=0)
    {
        QMessageBox::about(this, tr("Lexer"), "Error in formula.");
        return -1;
    }
    res=fcc.parser();
    if(res!=0)
    {
        QMessageBox::about(this, tr("Parser"), "Error in formula.");
        return -1;
    }
    res = fcc.optimizer();
    if(res!=0)
    {
        QMessageBox::about(this, tr("Optimizer"), "Error in formula.");
        return -1;
    }
    return 0;
}
void Widget::calc_f()
{
    try
    {
        if(initFunc()!=0)
            return;
        int x = fcc.varCount();
        QString s = tVars->toPlainText();
        string text = s.toUtf8().constData();
        QList<QString> list = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if(list.size()!=x)
        throw "ParamDismatch";

        double n[x];
        QList<QString>::iterator i;
        int j;

        for(j=0, i = list.begin(); i != list.end(); ++i, j++)
        n[j] = (*i).toDouble();

        lout->setText(QString::number(fcc.calculate(n)));
    }
    catch(...)
    {
        QMessageBox::about(this, tr("Error"), "Error in calculations");
    }
//    catch(exception& e)
//    {
//        QMessageBox::about(this, tr("Error in calculations"), QString(e.what()));
//    }
}
void Widget::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( FPlot, "Objective function");
}
void Widget::next_gen()
{
    FCurve2->detach();
    FPlot->replot();
    QPolygonF samples = g.gen_next();
    draw_dots(samples);
    lopt->setText(" opt = " + QString::number(g.get_opt()));
}

void Widget::draw_dots(QPolygonF samples)
{
    FCurve2->setSamples(samples);
    FCurve2->attach(FPlot);
    FPlot->replot();
}

void Widget::gen_rand_p()
{
    int count = fcc.varCount()+1;
    int points_count=100;
    double ** p;
    p = new double * [points_count];
    for(int i=0; i< points_count; i++)
        p[i] = new double[count];
    int l[count*2];
    getlimits(count, l);
    QPolygonF samples =  g.gen_0(l, p, points_count);
    draw_dots(samples);
    for(int i=0; i<points_count; i++)
        delete []p[i];
    delete []p;
}

//void Widget::about()
//{
//    QMessageBox::about(this, tr("About Application"),"The <b>Application</b> example demonstrates how to "
//        "write modern GUI applications using Qt, with a menu bar, "
//        "toolbars, and a status bar.");
//}
