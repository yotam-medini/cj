// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;


static unsigned dbg_flags;

class Dtree
{
 public:
    Dtree(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    size_t n;
    ull_t d, h;
    vull_t ys;
    double solution;
};

Dtree::Dtree(istream& fi) : solution(0)
{
    fi >> n >> d >> h;
    copy_n(istream_iterator<ull_t>(fi), n, back_inserter(ys));
}

void Dtree::solve_naive()
{
    for (ul_t i = 0; i < n; ++i)
    {
        ull_t height = h;
        double d_up = 0.;
        if ((i + 1 < n) && (ys[i + 1] - ys[i] < h))
        {
            height = ys[i + 1] - ys[i];
            d_up = double(d) * double(h - height)/double(h);
        }
        double area = (d + d_up)*height/2.;
        solution += area;
    }
    
}

void Dtree::solve()
{
    solve_naive();
}

void Dtree::print_solution(ostream &fo) const
{
    fo << setprecision(8) << solution << '\n';
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Dtree::*psolve)() =
        (naive ? &Dtree::solve_naive : &Dtree::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Dtree dtree(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (dtree.*psolve)();
        dtree.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
