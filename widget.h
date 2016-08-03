#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QTabWidget>
#include <QToolBar>
#include <QLabel>
#include <QCheckBox>
#include <QToolButton>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_series_data.h>
#include <qwt_color_map.h>
#include "backend.h"
#include <QPolygonF>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_renderer.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
//friend class SpectrogramData;
public:
    explicit Widget(QWidget *parent = 0);

    ~Widget();
private slots:
    void plot_f();
    void calc_f();
    void gen_rand_p();
   void exportPlot();
   void next_gen();
    //void about();


private:
    Ui::Widget *ui;

    QTabWidget * qtw;
    QPushButton * bNewNN;
    QPushButton * bImportNN;
    QPushButton * bDrawPlot;
    QPushButton * bSovle;
    QPushButton * bFout;
    QVBoxLayout * L0;
    QVBoxLayout * l1;
    QVBoxLayout * l3;
    QPlainTextEdit * pte;
    QTextEdit * tFunc;
    QTextEdit * tLimits;
    QTextEdit * tVars;
    QTextEdit * tdx;
    QTextEdit * tgencount;
    QToolBar * toolBar;
    QToolButton * btnExport;
    QLabel * lopt;
    QwtPlot * FPlot;
    QwtPlotCurve * FCurve;
    QwtPlotCurve * FCurve2; //dots
    QwtPlotSpectrogram * FSpectr;

    QLabel * lout;
    QHBoxLayout * hblfunc_inf;
    QHBoxLayout * hblout;
    QCheckBox * cbreplot;
    QPushButton * bGenRand;
    QPushButton * bGenNext;

    int tabscount = 5;
    int b_height=25, b_width=60;
    void getlimits(int n, int ab[]);
    fcalc fcc;
    fgenc  g;
    fcalc * pfcc;
    void createContents();
    void createActions();
    int initFunc();
    double getdx();
void draw_dots(QPolygonF samples);
void create_plots();
    void curvePlot();  //graph f(x)
    void contourPlot(); //spectrogram f(x, y)
    //void scatterPlot(); //scatter graph
    bool freplot();

    class SpectrogramData: public QwtRasterData
    {
    public:
        SpectrogramData();
        SpectrogramData(fcalc * pfc);
        virtual double value( double x, double y ) const;
    private:
        fcalc * fc;
    };
    class ColorMap: public QwtLinearColorMap
    {
    public:
        ColorMap():
            QwtLinearColorMap(QColor(0, 204, 204, 0), QColor(255, 107, 0, 0))
        {
            addColorStop( 0.3, QColor(125, 249, 255, 0));
            addColorStop( 0.6, QColor(0, 255, 127, 0) );
            addColorStop( 0.9, QColor(255, 204, 0, 0));
        }
//            QwtLinearColorMap( Qt::darkCyan, Qt::red)
//        {
//            addColorStop( 0.1, Qt::cyan );
//            addColorStop( 0.6, Qt::green );
//            addColorStop( 0.95, Qt::yellow );
//        }
    };

};


#endif // WIDGET_H
