#ifndef BACKEND_H
#define BACKEND_H

#include "math.h"
#include "stdio.h"
#include <list>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <QPolygonF>

using namespace std;
double fRand(double fMin, double fMax);
class fcalc
{
public:
fcalc();

int lexer(string text);
int parser();
int optimizer();

double calculate(double x);
double calculate(double x[]);
double calculate();

int varCount();
list<string> varNames();
string varName();
private:
struct token
{
    string value;
    int type;
};
list<string> f; //available functions
list<token> tokens;
map<string, int> op; //math operators and bracets
map<string, double> c; //math constants
map<string, double> constants; //constants in function
list<string> variables; //variables in function
string const_pattern; //pattern for constant id

int generate_const_name(int index, double value);
int add_var(string s);
double func(string s, double x);
double oper(string s, double x, double y);

};

class fgenc
{
public:
fgenc();
fgenc(fcalc * c);
QPolygonF gen_0(int limits[], double * p[], int count_gen=100);
QPolygonF gen_next();
double get_opt();

private:
fcalc * fcc;
int chrom_count;
int max_generations;
int genes_count;
double optimum;
vector< vector<double> > population;
void mutation(vector<double> & x);
void crossingover(vector<double> & x, vector<double> & y);


};

#endif // BACKEND_H
