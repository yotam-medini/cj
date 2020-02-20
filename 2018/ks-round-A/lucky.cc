// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>
#include <cstdio>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<double> vd_t;

static unsigned dbg_flags;

static string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    return ret;
}

class Lucky
{
 public:
    Lucky(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    u_t k;
    vul_t v;
    double solution;
};

Lucky::Lucky(istream& fi) : solution(0)
{
    fi >> n >> k;
    v.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t x;
        fi >> x;
        v.push_back(x);
    }
}

void Lucky::solve_naive()
{
    vd_t sv{v.begin(), v.end()};
    sort(sv.begin(), sv.end());
    double dn = 1./double(n);
    double e = accumulate(sv.begin(), sv.end(), 0.) * dn;
    const vd_t::const_iterator svb = sv.begin();
    const vd_t::const_iterator sve = sv.end();
    for (u_t dip = k; dip > 0; --dip)
    {
        vd_t::const_iterator ub = upper_bound(svb, sve, e);
        u_t up = sv.end() - ub;
        u_t down = n - up;
        e = dn*(accumulate(ub, sve, 0.) + down * e);
    }
    solution = e;
}

void Lucky::solve()
{
    solve_naive();
}

void Lucky::print_solution(ostream &fo) const
{
    fo << ' ' << dtos(solution);
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

    void (Lucky::*psolve)() =
        (naive ? &Lucky::solve_naive : &Lucky::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lucky lucky(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lucky.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        lucky.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
