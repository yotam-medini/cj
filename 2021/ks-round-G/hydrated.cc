// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <array>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;

static unsigned dbg_flags;

class Furniture
{
 public:
    ll_t xy_xy[2][2];
    ull_t dist(ll_t x, ll_t y) const
    {
        ll_t xy[2] = {x, y};
        ull_t d = 0;
        for (u_t i: {0, 1})
        {
            if (xy[i] < xy_xy[0][i])
            {
                d += xy_xy[0][i] - xy[i];
            }
            else if (xy[i] > xy_xy[1][i])
            {
                d += xy[i] - xy_xy[1][i];
            }
        }
        return d;
    }
};
typedef vector<Furniture> vfurn_t;

class Hydrated
{
 public:
    Hydrated(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ll_t solve_dim(u_t dim) const;
    u_t K;
    vfurn_t furns;
    ll_t solution[2];
};

Hydrated::Hydrated(istream& fi) : solution{-1, -1}
{
    fi >> K;
    furns.reserve(K);
    for (u_t i = 0; i < K; ++i)
    {
        Furniture furn;
        for (u_t j: {0, 1})
        {
            for (u_t xyi: {0, 1})
            {
                fi >> furn.xy_xy[j][xyi];
            }
        }
        furns.push_back(furn);
    }
}


void minby(ll_t& v, ll_t z)
{
    if (v > z)
    {
        v = z;
    }
}

void maxby(ll_t& v, ll_t z)
{
    if (v < z)
    {
        v = z;
    }
}

void Hydrated::solve_naive()
{
    ll_t xmin = furns[0].xy_xy[0][0];
    ll_t xmax = furns[0].xy_xy[0][1];
    ll_t ymin = furns[0].xy_xy[1][0];
    ll_t ymax = furns[0].xy_xy[1][1];
    for (const Furniture& furn: furns)
    {
        minby(xmin, furn.xy_xy[0][0]);
        maxby(xmax, furn.xy_xy[1][0]);
        minby(ymin, furn.xy_xy[0][1]);
        maxby(ymax, furn.xy_xy[1][1]);
    }
    if (dbg_flags & 0x1) { cerr << "xmin="<<xmin << ", xmax="<<xmax <<
        ", ymin="<<ymin << ", ymax="<<ymax << '\n'; }

    ull_t best_dist = K * ((xmax - xmin) + (ymax - ymin));
    for (ll_t x = xmin; x <= xmax; ++x)
    {
        for (ll_t y = ymin; y <= ymax; ++y)
        {
            ull_t d = 0;
            for (const Furniture& furn: furns)
            {
                d += furn.dist(x, y);
            }
            if (best_dist > d)
            {
                best_dist = d;
                solution[0] = x;
                solution[1] = y;
            }
        }
    }
}

void Hydrated::solve()
{
     for (u_t dim: {0, 1})
     {
         solution[dim] = solve_dim(dim);
     }
}

ll_t Hydrated::solve_dim(u_t dim) const
{
    typedef pair<ll_t, u_t> ptfi_t;
    typedef vector<ptfi_t> vptfi_t;
    vptfi_t ptfis;
    ptfis.reserve(2*K);
    ull_t d_best = 0;
    for (u_t fi = 0; fi < K; ++fi)
    {
        const Furniture& furn = furns[fi];
        d_best += furn.xy_xy[0][dim];
        ptfis.push_back(ptfi_t{furn.xy_xy[0][dim], fi});
        ptfis.push_back(ptfi_t{furn.xy_xy[1][dim], fi});
    }
    sort(ptfis.begin(), ptfis.end());
    const ll_t zmin = furns[ptfis[0].second].xy_xy[0][dim];
    d_best -= ll_t(K) * zmin;
    ull_t dcurr = d_best;
    ll_t ret = zmin;
    ll_t zcurr = zmin;
    u_t i = 0;
    for (i = 0; (i < 2*K) && (ptfis[i].first == zmin); ++i) {}
    if (dbg_flags & 0x1) { cerr << "#(zmin-equal) = " << i << '\n'; }
    u_t n_left = 0;
    u_t n_right = K - i;
    for ( ; i < 2*K; ++i)
    {
        const ll_t znext = ptfis[i].first;
        const u_t fi = ptfis[i].second;
        const Furniture& furn = furns[fi];
        ull_t zdelta = znext - zcurr;
        if (zdelta > 0)
        {
            dcurr += n_left * zdelta;
            dcurr -= n_right * zdelta;
        }
        if (dbg_flags) { cerr << "dim="<<dim << ", i="<<i <<
            ", zcurr="<<zcurr << ", znext="<<znext <<
            ", n_left="<<n_left << ", n_right="<<n_right <<
            ", dcurr="<<dcurr << '\n'; }
        if (d_best > dcurr)
        {
            d_best = dcurr;
            ret = znext;
        }
        if (znext == furn.xy_xy[0][dim])
        {
            --n_right;
        }
        else
        {
            ++n_left;
        }
        zcurr = znext;
    }
    return ret;
}

void Hydrated::print_solution(ostream &fo) const
{
    fo << ' ' << solution[0] << ' ' << solution[1];
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Hydrated::*psolve)() =
        (naive ? &Hydrated::solve_naive : &Hydrated::solve);
    if (solve_ver == 1) { psolve = &Hydrated::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hydrated hydrated(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hydrated.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hydrated.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
