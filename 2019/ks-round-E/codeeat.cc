// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
// #include <algorithm>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Codeeat
{
 public:
    Codeeat(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t s, d;
    vul_t cslot, eslot;
    vul_t aday, bday;
    vector<bool> solution;
};

Codeeat::Codeeat(istream& fi) 
{
    fi >> d >> s;
    cslot.reserve(s);
    eslot.reserve(s);
    aday.reserve(d);
    bday.reserve(d);
    for (u_t si = 0; si < s; ++si)
    {
        ul_t c, e;
        fi >> c >> e;
        cslot.push_back(c);
        eslot.push_back(e);
        
    }
    for (u_t di = 0; di < d; ++di)
    {
        ul_t a, b;
        fi >> a >> b;
        aday.push_back(a);
        bday.push_back(b);
    }
}

void Codeeat::solve_naive()
{
    for (u_t di = 0; di < d; ++di)
    {
        ul_t ctotal = accumulate(cslot.begin(), cslot.end(), 0);
        ul_t etotal = accumulate(eslot.begin(), eslot.end(), 0);
        // bool acan = (s * aday[di] <= ctotal);
        // bool bcan = (s * bday[di] <= etotal);
        // bool can = acan && bcan;
        bool can = false;
        if (aday[di] == 0)
        {
            can = bday[di] <= etotal;
        }
        else if (bday[di] == 0)
        {
            can = aday[di] <= ctotal;
        }
        else if ((ctotal > 0) && (etotal > 0))
        {
            double alpha = double(aday[di]) / double(ctotal);
            double beta =  double(bday[di]) / double(etotal);
            double apb = alpha + beta;
            can = (apb <= 1.);
        }
        solution.push_back(can);
    }
}

void Codeeat::solve()
{
    solve_naive();
}

void Codeeat::print_solution(ostream &fo) const
{
    fo << ' ';
    for (bool b: solution)
    {
        fo << (b ? 'Y' : 'N');
    }
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

    void (Codeeat::*psolve)() =
        (naive ? &Codeeat::solve_naive : &Codeeat::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Codeeat codeeat(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (codeeat.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        codeeat.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
