#include "backend.h"
#include "widget.h"
#include "ui_widget.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_series_data.h>
#include <qwt_plot_renderer.h>

#include "math.h"
#include "stdio.h"
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define O 1
#define F 2
#define V 3
#define C 4

#define ABS "abs"
#define SIN "sin"
#define COS "cos"
#define TG "tg"
#define CTG "ctg"
#define HSIN "hsin"
#define HCOS "hcos"
#define LN "ln"
#define LG "lg"


using namespace std;

int fcalc::generate_const_name(int index, double value)
{
    string s;
    stringstream out;
    out << index;
    s =const_pattern+out.str();
    constants[s]=value;
    token T;
    T.type = C;
    T.value=s;
    tokens.push_back(T);
    return 1;
}
int fcalc::add_var(string s)
{
    token T;
    T.type = V;
    T.value = s;
    tokens.push_back(T);
    list<string>::iterator i;
    for(i=variables.begin(); i!=variables.end(); i++)
        if(s.compare((*i))==0)
        return 0;
    variables.push_back(s);
    return 1;
}
fcalc::fcalc()
{
    f.push_back(SIN);
    f.push_back(COS);
    f.push_back(TG);
    f.push_back(CTG);
    f.push_back(HSIN);
    f.push_back(HCOS);
    f.push_back(LN);
    f.push_back(LG);
    f.push_back(ABS);

    op["+"]=1;
    op["-"]=1;
    op["*"]=2;
    op["/"]=2;
    op["^"]=3;

    op["("]=0;
    op[")"]=0;

    c["e"] =  2.7182818284;
    c["pi"] = 3.1415926535;
    c["0"] = 0;
    c["0.0"] = 0;

    const_pattern = "C";
}
int fcalc::lexer(string text)
{
    try
    {
    constants.clear();
    variables.clear();
    tokens.clear();
    list<token> stk;
    token T;
    int const_count=0, var_count=0;
    int j=0;
    while(j<text.length())
    {
        if(text[j]==' ')
            text.erase(j, 1);
        else
        j++;
    }
    if(text.length()<1)
        return 1;

    int cur_pos=0;
    double parameter;
    while(cur_pos<text.length())
    {
        //operator token
        map<string, int>::iterator m_it;
        for(m_it=op.begin(); m_it!=op.end(); m_it++)
            if(text.compare(cur_pos, (*m_it).first.length(), (*m_it).first)==0)
            {
                T.value=(*m_it).first;
                T.type=O;
                tokens.push_back(T);
                cur_pos+=(*m_it).first.length();
                break;
            }
        if(m_it!=op.end()) continue;
        //function token
        list<string>::iterator i;
        for(i=f.begin(); i!=f.end(); i++)
            if(text.compare(cur_pos, i->length(), *i)==0)
            {
                T.value=*i;
                T.type=F;
                tokens.push_back(T);
                cur_pos+=i->length();
                break;
            }
        if(i!=f.end()) continue;
        //constant token
        parameter = atof(&text[cur_pos]);
        if(parameter!=0.0)
        {
            generate_const_name(const_count, parameter);
            const_count++;

        while(((text[cur_pos]>='0')&&(text[cur_pos]<='9'))||(text[cur_pos]=='.'))
            cur_pos++;
        continue;
        }
        //lex.constant token
        map<string, double>::iterator it;
        for(it=c.begin(); it!=c.end(); it++)
            if(text.compare(cur_pos, (*it).first.length(), (*it).first)==0)
            {
                generate_const_name(const_count, (*it).second);
                const_count++;
                cur_pos+=(*it).first.length();
                break;
            }
            if(it!=c.end()) continue;
        //variable token
        string bb;
        int ind=cur_pos;
        while(((text[ind]>='A')&&(text[ind]<='Z'))||((text[ind]>='a')&&(text[ind]<='z')))
            bb+=text[ind++];
        while((text[ind]>='0')&&(text[ind]<='9'))
            bb+=text[ind++];
        if(ind==cur_pos)
            return 2;

        cur_pos = ind;
        if(add_var(bb)==1)
            var_count++;

    }
    return 0;
    }
    catch(...)
    {
        return -1;
    }
}
int fcalc::parser()
{
    try
    {
    list<token> out;
    list<token> stk;
    list<token>::iterator i;
    for(i=tokens.begin(); i!=tokens.end(); i++)
    {
        switch((*i).type)
        {
        case O:
            if((*i).value=="(")
            {
                stk.push_back((*i));
                break;
            }
            if((*i).value==")")
            {
                while(stk.back().value!="(")
                {
                    out.push_back(stk.back());
                    stk.pop_back();
                }

                if(stk.back().value!="(")
                    return 1;
                stk.pop_back();
                if(stk.back().type==F)
                {
                    out.push_back(stk.back());
                    stk.pop_back();
                }
                break;
            }
            if(stk.size()>0)
            while(op[stk.back().value]>=op[(*i).value])
            {
                out.push_back(stk.back());
                stk.pop_back();
            }
            stk.push_back((*i));
            break;
        case F:
            stk.push_back((*i));
            break;
        default:
            out.push_back((*i));
        }
    }
    while(!stk.empty())
    {
        if(stk.back().type!=O)
            return 2;
        out.push_back(stk.back());
        stk.pop_back();
    }
    tokens.clear();
    for(i=out.begin(); i!=out.end(); i++)
        tokens.push_back((*i));
    return 0;
    }
    catch(...)
    {
        return -1;
    }
}
int fcalc::optimizer()
{
    try
    {
   list<token> stk;
   list<token>::iterator i;

    for(i=tokens.begin(); i!=tokens.end();i++)
    {
        if((i->type==V)|| (i->type==C))
        {
            stk.push_back(*i);
        }
        else
        if((i->type==F)&&(!stk.empty()))
        {
            if(stk.back().type==C)
            {
                constants[stk.back().value]=func(i->value, constants[stk.back().value]);
            }
            else
                stk.push_back(*i);
        }
        else
        if(i->type==O)
        {
            if(stk.size()<2)
                return -1;
            if(stk.back().type==C)
            {
                token  T;
                T.type=C;
                T.value=stk.back().value;
                stk.pop_back();
                if(stk.back().type!=C)
                {
                    stk.push_back(T);
                    stk.push_back(*i);
                }
                else
                {
                    double res = oper(i->value, constants[stk.back().value], constants[T.value]);
                    constants[stk.back().value]=res;
                    constants.erase(T.value);
                }
            }
            else
                stk.push_back(*i);

        }
    }
    tokens.clear();
    for(i=stk.begin(); i!=stk.end(); i++)
        tokens.push_back((*i));

    return 0;
    }
    catch(...)
    {
        return -1;
    }
}
int fcalc::varCount()
{
    return variables.size();
}
double fcalc::calculate()
{
    if((constants.size()==1)&&(tokens.size()==1))
        return constants[tokens.back().value];
    throw "expression could be shorten";
}
double fcalc::calculate(double x)
{
    if(variables.size()!=1)
        throw "no variables in expression";
    if((tokens.size()==1)&&(constants.size()==0))
        return x;
    int j=0;
    try
    {
    list<token> stk;
    list<token>::iterator it;
    map<string, double> cnst(constants.begin(), constants.end());

     for(it=tokens.begin(); it!=tokens.end();it++)
     {
         if((it->type==C)||(it->type==V))
         {
             stk.push_back(*it);
             continue;
         }
         if(it->type==F)
         {
             if(stk.back().type==C)
                 cnst[stk.back().value]=func(it->value,cnst[stk.back().value]);
             else
                 cnst[stk.back().value]=func(it->value, x);
               //  cnst[stk.back().value]=func(it->value, cnst[stk.back().value]);
             continue;
         }
         if(stk.size()<2)
             throw(1);
             double x1, x2;
             string s="";
             if(stk.back().type==C)
             {
                 x1=cnst[stk.back().value];
                 s=stk.back().value;
             }
             else
                 x1=x;
             stk.pop_back();
             if(stk.back().type==C)
                 x2=cnst[stk.back().value];
             else
             {
                 x2=x;
                 stk.back().type=C;
             }
             double res = oper(it->value, x2, x1);
             if(s!="")
             {
                 stk.back().value=s;
                 cnst[s]=res;
             }
             else
                 cnst[stk.back().value]=res;
     }
     return cnst[stk.back().value];
    }
    catch(...)
    {
        throw "function could not be calculated";
    }
}
double fcalc::calculate(double x[])
{
if(variables.size()<1)
    throw "no variables in expression";
map<string, double> vars;
list<string>::iterator i;
int j=0;
try
{
for(j=0, i = variables.begin(); i != variables.end(); ++i, j++)
    vars[(*i)]=x[j];

list<token> stk;
list<token>::iterator it;
map<string, double> cnst(constants.begin(), constants.end());

 for(it=tokens.begin(); it!=tokens.end();it++)
 {
     if((it->type==C)||(it->type==V))
     {
         stk.push_back(*it);
         continue;
     }
     if(it->type==F)
     {
         if(stk.back().type==C)
             cnst[stk.back().value]=func(it->value,cnst[stk.back().value]);
         else
             cnst[stk.back().value]=func(it->value, cnst[stk.back().value]);
         continue;
     }
     if(stk.size()<2)
         throw(1);
         double x1, x2;
         string s="";
         if(stk.back().type==C)
         {
             x1=cnst[stk.back().value];
             s=stk.back().value;
         }
         else
             x1=vars[stk.back().value];
         stk.pop_back();
         if(stk.back().type==C)
             x2=cnst[stk.back().value];
         else
         {
             x2=vars[stk.back().value];
             stk.back().type=C;
         }
         double res = oper(it->value, x2, x1);
         if(s!="")
         {
             stk.back().value=s;
             cnst[s]=res;
         }
         else
             cnst[stk.back().value]=res;
 }
return cnst[stk.back().value];
}
catch(...)
{
    throw "function could not be calculated";
}
}
double fcalc::func(string s, double x)
{
    if(s==SIN)
        return sin(x);
    if(s==COS)
        return cos(x);
    if(s==TG)
        return tan(x);
    if(s==CTG)
        return 1/tan(x);
    if(s==HSIN)
        return sinh(x);
    if(s==HCOS)
        return (exp(x)-exp(-x))/2;
    if(s==LN)
        return log(x);
    if(s==LG)
        return log10(x);
    if(s==ABS)
        return(x<0?-x:x);

    return 0.0;
}
double fcalc::oper(string s, double x, double y)
{
    if(s=="+")
        return (x+y);
    if(s=="-")
        return (x-y);
    if(s=="/")
        return (x/y);
    if(s=="*")
        return (x*y);
    if(s=="^")
        return pow(x, y);

    return 0.0;
}
list<string> fcalc::varNames()
{
    return variables;
}
string fcalc::varName()
{
    return variables.back();
}


double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int bRand(int range)
{
   return  rand() % (range+1)+1;
}

//-----------------------------------------------------------------------------------------

fgenc::fgenc(fcalc * c)
{
    fcc = c;
}
fgenc::fgenc()
{
    fcc = NULL;
}
QPolygonF fgenc::gen_0(int limits[], double * p[], int count_gen)
{
    QPolygonF samples = QPolygonF();
    try
    {
    int vc = fcc->varCount();
    if(vc<1)
        return samples;
    srand(time(NULL));

    for(int j=0;j<vc; j++)
    {
        int range_min = limits[j*2];
        int range_max=limits[j*2+1];
        if((range_max-range_min)<=0)
            return samples;
        for(int i=0; i<count_gen; i++)
            p[i][j]= fRand(range_min, range_max);
    }
    int i;

    if(vc==1)
    {
        for(i=0; i<count_gen; i++)
        {
            p[i][vc]=fcc->calculate(p[i][0]);
            samples<<QPointF(p[i][0], p[i][1]);
            vector <double> chromosome;
            for(int j=0; j<=vc;j++)
                chromosome.push_back(p[i][j]);
            population.push_back(chromosome);
        }

    }
    else
    for(i=0; i<count_gen; i++)
    {
        p[i][vc]=fcc->calculate(p[i]);
        samples<<QPointF(p[i][0], p[i][1]);
        vector <double> chromosome;
        for(int j=0; j<=vc;j++)
            chromosome.push_back(p[i][j]);
        population.push_back(chromosome);
    }
    return samples;
    }
    catch(...)
    {
        return samples;
    }
}

    bool optcmp(vector<double> left, vector<double> right)
    {
        if(left[left.size()-1]>right[right.size()-1])
            return true;
        else return false;
    }

double fgenc::get_opt()
{
    return optimum;
}

QPolygonF fgenc::gen_next()
{
    QPolygonF points = QPolygonF();

    sort(population.begin(), population.end(), optcmp);
    vector< vector<double> >::iterator i;
    i=population.begin();
    optimum = (*i)[(*i).size()-1];
    vector< vector<double> > new_gen;
    for(i=population.begin(); i!=population.begin()+population.size()/2; i++)
    {
        new_gen.push_back(*i);
    }
    if((*population.begin()).size()>2)
    for(i=population.begin(); i!=population.begin()+population.size()/2; i+=2)
    {
        crossingover(*i, *(i+1));
        new_gen.push_back(*i);
        new_gen.push_back(*(i+1));
    }
    else
        for(i=population.begin(); i!=population.begin()+population.size()/2; i++)
        {
            mutation(*i);
            new_gen.push_back(*i);
        }

    population.clear();


    for(i=new_gen.begin(); i!=new_gen.end(); i++)
    {
        population.push_back(*i);
        points <<QPointF((*i)[0], (*i)[1]);
    }
    return points;
}
void fgenc::mutation(vector<double> & x)
{
    int b;
    int N = x.size();
    if(N>2)
    b= bRand(N);
    else
        b=0;
    x[b]+=fRand(-10, 10);
    x[N-1]=fcc->calculate(x[b]);
}

void fgenc::crossingover(vector<double> & x, vector<double> & y)
{
        int N=x.size();
        int b;
        if(N==3)
            b=1;
        else
            b  = bRand(N);

        swap_ranges(x.begin(), x.begin()+b, y.begin());
        double tempx[N-1];
        double tempy[N-1];
        for(int i=0; i<N-1;i++)
        {
            tempx[i]=x[i];
            tempy[i]=y[i];
        }
        x[N-1]=fcc->calculate(tempx);
        y[N-1]=fcc->calculate(tempy);
}

