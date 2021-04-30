// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

static const ull_t h_tpns = 1;
static const ull_t m_tpns = 12;
static const ull_t s_tpns = 720;

static const ull_t tenp10 = 10000000000;
static const ull_t tenp9  =  1000000000;
static const ull_t circle_ticks = ull_t(360) * ull_t(12) * tenp10;
static const ull_t circle_ticks_p1 = circle_ticks + 1;
static const ull_t mh_tpns = m_tpns - h_tpns;
static const ull_t hour_ns = 60*60*tenp9;

// (m_tpns-h_tpns)^{-1} mod circle_ticks
static ull_t comppute_inv11(ull_t g)
{
    ull_t qm = (circle_ticks / g) + 1;
    ull_t r0 = qm * g - circle_ticks;
    ull_t inv = qm;
    ull_t r = r0;
    while (r % g != 1)
    {
        r += r0;
        inv += qm;
    }
    --inv;

    ull_t m = g * inv, mm = m % circle_ticks;
    if (mm != 1)
    {
        cerr << __func__ << " Failed: inv=" << inv <<
            ", m=" << m << ", mm=" << mm << '\n';
        exit(1);
    }
    return inv;
}
// static const ull_t inv11 = 15709090909091; 
static const ull_t inv11 = comppute_inv11(mh_tpns);

ull_t mult_cycle(ull_t x, ull_t y)
{
    ull_t z = 0;
    ull_t xp = x;
    while (y)
    {
        if (y & ull_t(0x1))
        {
            z = (z + xp) % circle_ticks;
        }
        xp = (ull_t(2)*xp) % circle_ticks;
        y = y / ull_t(2);
    }
    return z;
}

ull_t div11_cycle(ull_t x)
{
    ull_t y = (x ? mult_cycle(x, inv11) : 0);
    return y;
}

class HMS_NS
{
 public:
    HMS_NS(u_t _h=0, u_t _m=0, u_t _s=0, ull_t _ns=0) :
        h(_h), m(_m), s(_s), ns(_ns) 
    {
    }
    u_t h, m, s;
    ull_t ns;
};

bool operator <(const HMS_NS& t0, const HMS_NS& t1)
{
    bool lt = false;
    if (t0.h < t1.h)
    {
        lt = true;
    }
    else if (t0.h == t1.h)
    {
        if (t0.m < t1.m)
        {
            lt = true;
        }
        else if (t0.m == t1.m)
        {
            if (t0.s < t1.s)
            {
                lt = true;
            }
            else if (t0.s == t1.s)
            {
                lt = t0.ns < t1.ns;
            }
        }
    }
    return lt;
}

class Clock
{
 public:
    Clock(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool hands_solve(HMS_NS& hmsns, ull_t h, ull_t m, ull_t s);
    ull_t A, B, C;
    u_t solh, solm, sols;
    ull_t sol_ns;
};

Clock::Clock(istream& fi) : solh(0), solm(0), sols(0), sol_ns(0)
{
    fi >> A >> B >> C;
}

void Clock::solve_naive()
{
    static const ull_t P[6][3] =
    {
        {0, 1, 2},
        {0, 2, 1},
        {1, 0, 2},
        {1, 2, 0},
        {2, 0, 1},
        {2, 1, 0}
    };
    HMS_NS hms, hms_min;
    ull_t  abc[3];
    abc[0] = A;
    abc[1] = B;
    abc[2] = C;
    u_t n_solved = 0;
    for (u_t pi = 0; pi < 6; ++pi)
    {
        if (hands_solve(hms, abc[P[pi][0]], abc[P[pi][1]], abc[P[pi][2]]))
        {
            if ((n_solved == 0) || (hms < hms_min))
            { 
                hms_min = hms;
            }
            ++n_solved;
        }
    }
    solh = hms_min.h;
    solm = hms_min.m;
    sols = hms_min.s;
    sol_ns = hms_min.ns;
}

bool Clock::hands_solve(HMS_NS& hmsns, ull_t h, ull_t m, ull_t s)
{
    ull_t mh = (m > h ? m - h : (m + circle_ticks) - h);
    ull_t ns = div11_cycle(mh);
    ull_t offset = ((h + circle_ticks) - ns) % circle_ticks;
    ull_t sec_ticks = (ns * s_tpns + offset) % circle_ticks;
    bool solved = (s == sec_ticks);
    if (solved)
    {
        hmsns.ns = ns % tenp9;
        ull_t seconds = ns / tenp9;
        hmsns.s = seconds % 60;
        ull_t minutes = seconds / 60;
        hmsns.m = minutes % 60;
        hmsns.h = minutes / 60;
    }
    return solved;
}

void Clock::solve()
{
    solve_naive();
}

void Clock::print_solution(ostream &fo) const
{
    fo << ' ' << solh << ' ' << solm << ' ' << sols << ' ' << sol_ns;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    if (dbg_flags & 0x1)
    {
        cerr << "inv11 = " << inv11 << '\n';
        if (mult_cycle(inv11, 11) != 1)
        {
            cerr << "Failed mult_cycle(inv11, 11)\n";
            exit(1);
        }
        if (mult_cycle(11, inv11) != 1)
        {
            cerr << "Failed mult_cycle(11, inv11)\n";
            exit(1);
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

    void (Clock::*psolve)() =
        (naive ? &Clock::solve_naive : &Clock::solve);
    if (solve_ver == 1) { psolve = &Clock::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Clock clock(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (clock.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        clock.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
