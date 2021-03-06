// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <array>
#include <iterator>
#include <algorithm>

#include <cstdlib>

using namespace std;

typedef unsigned long u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;

typedef array<int, 0x100> ac2i_t;
typedef vector<ac2i_t> vac2i_t;

typedef array<vi_t, 0x100> ac2vi_t;
typedef vector<ac2vi_t> vac2vi_t;

static unsigned dbg_flags;

static void minby(int &v, int by)
{
    if (v > by)
    {
        v = by;
    }
}

static void minby(u_t &v, u_t by)
{
    if (v > by)
    {
        v = by;
    }
}

class Smooth
{
 public:
    Smooth(istream& fi);
    void solve_naive();
    void solve(bool naive);
    void print_solution(ostream&) const;
 private:
    u_t sub_solve_2(int v0, int v1);
    u_t sub_solve_3(int v0, int v1, int v2);
    u_t subsolve(u_t ai, int c, vi_t *head);
    bool is_smooth(const vi_t &sub) const;
    u_t D, I, N;
    int M;
    vi_t a;
    u_t infinity;
    vac2i_t memo;
    u_t solution;
    bool adbg;
    vac2vi_t amemo;
};

Smooth::Smooth(istream& fi) : solution(0), adbg((dbg_flags & 0x1) != 0)
{
    fi >> D >> I >> M >> N;
    a = vi_t(vi_t::size_type(N), u_t(0));
    for (u_t i = 0; i < N; ++i)
    {
        fi >> a[i];
    }
    infinity = D * N + 1;
}

void Smooth::solve_naive()
{
    solution = is_smooth(a) ? 0 : u_t(-1);
    if (solution != 0)
    {
        switch (a.size())
        {
         case 1:
            solution = 0;
            break;
         case 2:
            solution = sub_solve_2(a[0], a[1]);
            break;
         case 3:
            solution = sub_solve_3(a[0], a[1], a[2]);
            break;
         default:
            solution = u_t(-1);
        }
    }
}

void Smooth::solve(bool naive)
{
    if (naive && a.size() <= 3)
    {
        solve_naive();
    }
    else
    {
        ac2i_t undef;
        fill_n(&undef[0], 0x100, -1);
        memo = vac2i_t(vac2i_t::size_type(a.size()), undef);
        ac2vi_t aundef;
        amemo = vac2vi_t(vac2vi_t::size_type(a.size()), aundef);
        u_t best = u_t(-1);
        vi_t smoothed;
        for (unsigned c = 0; c < 0x100; ++c)
        {
            vi_t candid;
            u_t subret = subsolve(N - 1, c, adbg ? &candid : 0);
            if (best > subret)
            {
                best = subret;
                if (adbg) { swap(smoothed, candid); }
            }
        }
        solution = best;
        if (adbg) { cerr << "best: ";
            ostream_iterator<int> oi(cerr, " ");
            copy(smoothed.begin(), smoothed.end(), oi); cerr << "\n";}
    }
}

bool Smooth::is_smooth(const vi_t &sub) const
{
    bool ret = true;
    vi_t::const_iterator i = sub.begin(), inext = i, e = sub.end();
    ++inext;
    for (; ret && (inext != e); i = inext++)
    {
        int v = *i;
        int vnext = *inext;
        ret = abs(vnext - v) <= M;
    }
    return ret;
}

u_t Smooth::sub_solve_2(int v0, int v1)
{
    u_t ret = 0;
    int delta = v1 - v0;
    int adelta = abs(delta);
    if (adelta > M)
    {
        int d_cost = D;
        int i_cost = infinity;
        if (M > 0)
        {
            int n_ins = (adelta - 1)/M;
            i_cost = I*n_ins;
            if (n_ins > 1)
            {
                int ic_cost = I*(n_ins - 1) + (adelta - n_ins * M);
                minby(i_cost, ic_cost);
            }
        }
        int c_cost = adelta - M;
        if (dbg_flags & 0x1) {
            cerr << __func__ << "("<<v0<<", "<< v1<<") adelta="<<adelta <<
               ", d_cost="<<d_cost << ", i_cost\n"; }
        ret = min(min(d_cost, i_cost), c_cost);
    }
    return ret;
}

u_t Smooth::sub_solve_3(int v0, int v1, int v2)
{
    // assuming original is not smooth
    u_t ret = 0, subret = 0;

    int delta01 = v1 - v0;
    int adelta01 = abs(delta01);
    int delta12 = v2 - v1;
    int adelta12 = abs(delta12);
    if ((adelta01 > M) || (adelta12 > M))
    {
        u_t sub01 = sub_solve_2(v0, v1);
        u_t sub02 = sub_solve_2(v0, v2);
        u_t sub12 = sub_solve_2(v1, v2);
        u_t d_cost = D + min(min(sub01, sub02), sub12);
        ret = d_cost;
        if (dbg_flags & 0x1) {
            cerr << __func__ << "("<<v0<<", "<< v1<<", "<<v2 << ") " <<
            "d_cost="<< d_cost << "\n"; }

        if ((adelta01 > M) || (adelta12 > M))
        {
            int min_01 = min(v0, v1);
            int min_012 = min(min_01, v2);
            int max_012 = max(max(v0, v1), v2);
            for (int c = min_012; c <= max_012; ++c)
            {
                subret = abs(c - v0) + sub_solve_2(v0, v1) + sub12;
                minby(ret, subret);
                subret = abs(c - v1) + sub_solve_2(v0, c) + sub_solve_2(c, v2);
                minby(ret, subret);
                subret = abs(c - v2) + sub01 + sub_solve_2(v1, c);
                minby(ret, subret);
            }
            u_t icost = (M > 0 ? I*((adelta01 - 1)/M + (adelta12 - 1)/M) :
                infinity);
            minby(ret, icost);
            if (dbg_flags & 0x1) { cerr << 
                   " .. 3 ..  icost="<<icost <<
                   ", adelta01="<<adelta01 <<
                   ", adelta12="<<adelta12 << "\n"; }
        }
        if ((adelta01 > M) && (adelta12 <= M))
        {
            u_t icost = (M > 0 ? I*((adelta01 - 1)/M) : infinity);
            minby(ret, icost);
            minby(ret, adelta01 - M);
            if (dbg_flags & 0x1) { cerr << 
                   " .. 3 .. icost="<<icost <<
                   ", adelta01="<<adelta01 << "\n"; }
        }
        if ((adelta12 > M) && (adelta01 <= M))
        {
            u_t icost = (M > 0 ? I*((adelta12 - 1)/M) : infinity);
            minby(ret, icost);
            minby(ret, adelta12 - M);
            if (dbg_flags & 0x1) { cerr << 
                   " .. 3 .. icost="<<icost <<
                   ", adelta12="<<adelta12 << "\n"; }
        }
    }
    return ret;
}

static void vpush(vi_t &v, int n, int to, int step)
{
    for (int x = to - (n - 1)*step; --n >= 0; x += step)
    {
        v.push_back(x);
    }
}

// return cost of smoothnig till ai where pre-segmanet ends with c
u_t Smooth::subsolve(u_t ai, int c, vi_t *smoothed)
{
    u_t ret = infinity;
    int cached = memo[ai][c];
    if (cached != -1)
    {
        ret = cached;
        if (smoothed) { *smoothed = amemo[ai][c]; }
    }
    else
    {
        if (smoothed) { smoothed->clear(); }
        int n_ins = 0;
        if (ai == 0)
        {
            ret = D;
            u_t adelta = abs(a[0] - c);
            u_t i_cost = infinity, ic_cost = infinity;
            u_t change = 0;
            if (M > 0 && adelta > 0)
            {
                n_ins = (adelta + M - 1)/M;
                i_cost = I*n_ins;
                if (n_ins > 0)
                {
                    change = adelta - (n_ins - 1) * M;
                    ic_cost = (i_cost - I) + change;
                }
            }
            if ((ret > adelta) || (ret > i_cost) || (ret > ic_cost))
            {
                if (adelta <= min(i_cost, ic_cost))
                {
                    ret = adelta;
                    if (smoothed) { smoothed->push_back(c); }
                }
                else
                {
                    if (i_cost < ic_cost)
                    {
                        ret = i_cost;
                        if (smoothed)
                        {
                            smoothed->push_back(a[0]);
                            vpush(*smoothed, n_ins, c, a[0] < c ? M : -M);
                        }
                    }
                    else
                    {
                        ret = ic_cost;
                        if (smoothed)
                        {
                            int a0n = a[0] + (a[0] < c ? change : -change);
                            smoothed->push_back(a0n);
                            vpush(*smoothed, n_ins - 1, c, a0n < c ? M : -M);
                        }
                    }
                }
            }
        }
        else
        {
            ret = D + subsolve(ai - 1, c, smoothed);
            for (int cpre = 0; cpre <= 0x100; ++cpre)
            {
#if 0
if ((ai == 2 && (c == 5 || c == 7) && (cpre == 1 || cpre == 5)) ||
    (ai == 2 && c == 90 && cpre == 107) ||
    (ai == 2 && c == 96 && cpre == 113) ||
    (ai == 1 && c == 5 && cpre == 1) ||
    (ai == 1 && c == 113 && cpre == 147) ||
    (ai == 1 && c == 107 && cpre == 147)) {
 cerr << "yotam stop\n";
}
#endif
                vi_t head;
                u_t precost = subsolve(ai - 1, cpre, (adbg ? &head : 0));
                int delta = c - cpre;
                int adelta = abs(delta);
                int c_delta = c - a[ai];
                int ac_delta = abs(c_delta);
                u_t c_cost = (adelta <= M ? ac_delta : infinity);
                u_t i_cost = infinity;
                if (c_delta == 0) // insert before
                {
                    if (delta == 0)
                    {
                        i_cost = 0;
                    }
                    else if (M > 0)
                    {
                        n_ins = (adelta - 1)/M;
                        i_cost = I*n_ins;
                    }
                }
                else if (abs(a[ai] - cpre) <= M) // insert after
                {
                    if (M > 0)
                    {
                        n_ins = (ac_delta + M - 1)/M;
                        i_cost = I*n_ins;
                    }
                }
                u_t ic_cost = precost + min(i_cost, c_cost);
                if (ret > ic_cost)
                {
                    ret = ic_cost;
                    if (smoothed) 
                    { 
                        swap(*smoothed, head);
                        if (c_cost < i_cost)
                        {
                            smoothed->push_back(c);
                        }
                        else
                        {
                            if (c_delta == 0) 
                            {
                                if (delta != 0)
                                {
                                    vpush(*smoothed, n_ins+1, c, 
                                        delta > 0 ? M : -M);
                                }
                            }
                            else
                            {
                                smoothed->push_back(a[ai]);
                                vpush(*smoothed, n_ins, c, a[ai] < c ? M : -M);
                            }
                        }
                    }
                }
            }
        }
        memo[ai][c] = ret;
        if (adbg) { amemo[ai][c] = *smoothed; }
    }
    return ret;
}

void Smooth::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Smooth smooth(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        smooth.solve(naive);
        fout << "Case #"<< ci+1 << ":";
        smooth.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
