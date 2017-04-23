// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef mpq_class mpqc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Horse
{
 public:
    Horse(ul_t vk=0, ul_t vs=0): k(vk), s(vs) {}
    ul_t k;
    ul_t s;
};

bool operator <(const Horse &h0, const Horse &h1)
{
    bool lt = h0.k < h1.k;
    return lt;
}

typedef vector<Horse> vhorse_t;

class Cruise
{
 public:
    Cruise(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t d;
    unsigned n;
    vhorse_t horses;
    double solution;
};

Cruise::Cruise(istream& fi)
{
    fi >> d >> n;
    horses.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        ul_t  k, s;
        fi >> k >> s;
        horses.push_back(Horse(k, s));
    }
}

void Cruise::solve_naive()
{
    // assume n == 1
    const mpqc_t qd(d, 1);
    mpqc_t  th0(d - horses[0].k, horses[0].s);
    mpqc_t  cruise_speed = qd / mpqc_t(th0);
    if (n == 2 && horses[0].s > horses[1].s)
    {
        ul_t dk = horses[1].k - horses[0].k;
        ul_t ds = horses[0].s - horses[1].s;
        mpqc_t tmatch(dk, ds);
        mpqc_t xmatch = mpqc_t(horses[0].k, 1) + 
            mpqc_t(horses[0].s, 1) * tmatch;
        if (dbg_flags & 0x1)
        {
            cerr << "dk="<<dk << ", ds="<<ds << "\n";
            cerr << "tmatch="<<tmatch << ", xmatch="<<xmatch << "\n";
        }
        if (xmatch < qd)
        {
            mpqc_t s1 = xmatch / tmatch;
            if (cruise_speed > s1)
            {
                cruise_speed = s1;
            }
            mpqc_t th1(d - horses[1].k, horses[1].s);
            mpqc_t s2 = qd / th1;
            if (dbg_flags & 0x1)
            {
                cerr << "th1="<<th1 << ", s2="<<s2 << "\n";
            }
            if (cruise_speed > s2)
            {
                cruise_speed = s2;
            }
        }
    }
    solution = cruise_speed.get_d();

}

#if 0
void Cruise::solve()
{
    sort(horses.begin(), horses.end());
    if (n <= 2)
    {
        solve_naive();
    }
    else
    {
        cerr << "fail: n > 2\n";
        exit(1);
    }
}
#endif

void Cruise::solve()
{
    sort(horses.begin(), horses.end());
    const mpqc_t qd(d, 1);
    mpqc_t  th0(d - horses[0].k, horses[0].s);
    mpqc_t  cruise_speed = qd / mpqc_t(th0);
    for (unsigned i = 1; i < n; ++i)
    {
        const Horse h = horses[i];
        mpqc_t  thi(d - h.k, h.s);
        mpqc_t  si = qd / mpqc_t(thi);
        if (cruise_speed > si)
        {
            cruise_speed = si;
        }
    }
    solution = cruise_speed.get_d();
}

void Cruise::print_solution(ostream &fo) const
{
    // fo << setprecision(6) << solution;
    char buf[0x20];
    sprintf(buf, "%.6f", solution);
    fo << " " << buf;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Cruise::*psolve)() =
        (naive ? &Cruise::solve_naive : &Cruise::solve);

    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cruise problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
