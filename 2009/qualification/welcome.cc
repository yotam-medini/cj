// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

static unsigned dbg_flags;

const char *welcome = "welcome to code jam";
// const char *welcome = "ab";
enum { SOLUTION_MOD = 10000 };

class Welcome
{
 public:
    Welcome(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    unsigned sub_solve(unsigned tb, unsigned te, unsigned wb, unsigned we);
    string text;
    const char *cstext;
    unsigned solution;
};

Welcome::Welcome(istream& fi) : solution(0)
{
    getline(fi, text);
    cstext = text.c_str();
}

void Welcome::solve()
{
    unsigned welcome_len = strlen(welcome);
    unsigned cstext_len = strlen(cstext);
    if (welcome_len <= cstext_len)
    {
        solution = sub_solve(0, cstext_len, 0, welcome_len);
    }
}

unsigned Welcome::sub_solve(unsigned tb, unsigned te,
                               unsigned wb, unsigned we)
{
    unsigned ret = 0;

    if (we == wb)
    {
        ret = 1;
    }
    else if (te - tb == we - wb)
    {
        if (equal(cstext + tb, cstext + te, welcome + wb))
        {
             ret = 1;
        }
    }
    else
    {
        unsigned tm = (tb + te)/2;
        // Must have:   
        //   wm - wb <=  tm - tb  &&  we - wm <=  te - tm;   <-->
        //   wm <= wb + tm - tb   &&  wm >= we + tm - te 
        unsigned wmb = (we + tm > te ? max(wb, we + tm - te) : wb);
        unsigned wme = (wb + tm > tb ? min(we, wb + tm - tb) : we);
        for (unsigned wm = wmb; wm <= wme; ++wm)
        {
            unsigned n_left = sub_solve(tb, tm, wb, wm);
            unsigned n_right = n_left ? sub_solve(tm, te, wm, we) : 0;
            ret += (n_left * n_right);
        }
        ret %= SOLUTION_MOD;
    }
    if (dbg_flags & 0x1)
    {
        cerr << 
            "text["<<tb<<":"<<te<<"]=" << string(cstext + tb, cstext + te) << 
            "\n" 
            "wlcm["<<wb<<":"<<we<<"]=" << string(welcome + wb, welcome + we) <<
            "\n" "ret="<<ret<<"\n\n";
    }
    return ret;
}


void Welcome::print_solution(ostream &fo)
{
    solution %= SOLUTION_MOD;
    char solstr[5];
    sprintf(solstr, "%04d", solution);
    fo << solstr;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    if (argc > 4) { welcome = argv[4]; }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;
    (*pfi).getline(0, 0);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Welcome problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

