// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
// #include <set>
#include <array>
#include <numeric>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Attraction
{
 public:
   Attraction(u_t _h=0, u_t _s=0, u_t _e=0) : h(_h), s(_s), e(_e) {}
   u_t h, s, e;
};
typedef vector<Attraction> vattr_t;

bool operator<(const Attraction& a0, const Attraction& a1)
{
    return a0.h < a1.h;
}

class Festival
{
 public:
    Festival(istream& fi);
    Festival(u_t _D, u_t _N, u_t _K, const vattr_t& a) :
        D(_D), N(_N), K(_K), attractions(a), solution(0)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    u_t D, N, K;
    vattr_t attractions;
    ull_t solution;
};

Festival::Festival(istream& fi) : solution(0)
{
    fi >> D >> N >> K;
    attractions.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        u_t h, s, e;
        fi >> h >> s >> e;
        Attraction a(h, s, e);
        attractions.push_back(a);
    }
}

void Festival::solve_naive()
{
    int best_day = -1;
    for (ull_t d = 1; d <= D; ++d)
    {
        vu_t h;
        for (const Attraction& a: attractions)
        {
            if ((a.s <= d) && (d <= a.e))
            {
                h.push_back(a.h);
            }
        }
        sort(h.begin(), h.end());
        u_t hsz = h.size();
        u_t hu = min(hsz, K);
        ull_t hbest = 0;
        for (u_t i = hsz - hu; i < hsz; ++i)
        {
            hbest += h[i];
        }
        if (solution < hbest)
        {
            solution = hbest;
            best_day = d;
        }
    }
    if (dbg_flags & 0x1) { cerr << "best_day=" << best_day << '\n'; }
}

void Festival::solve()
{
    vau2_t days_attrs;
    for (u_t i = 0; i < N; ++i)
    {
        const Attraction& attr = attractions[i];
        days_attrs.push_back(au2_t{2*attr.s, i});
        days_attrs.push_back(au2_t{2*attr.e + 1, i});
    }
    sort(days_attrs.begin(), days_attrs.end());
    priority_queue<au2_t, vau2_t, greater<au2_t> > qhappy;
    vector<bool> attr_inq(size_t(N), false);
    ull_t happiness = 0;
    u_t n_inq_expired = 0;
    for (const au2_t& da: days_attrs)
    {
        u_t dday = da[0], ia = da[1];
        const Attraction& attraction = attractions[ia];
        if (dday % 2 == 0) // start
        {
            qhappy.push(au2_t{attraction.h, ia});
            attr_inq[ia] = true;
            happiness += ull_t(attraction.h);
            while (qhappy.size() - n_inq_expired > K)
            {
                const au2_t hmin = qhappy.top();
                const Attraction& old_attraction = attractions[hmin[1]];
                if (2*old_attraction.e < dday)
                { // expired
                    --n_inq_expired;
                }
                else
                {
                    happiness -= hmin[0];
                }
                qhappy.pop();
                attr_inq[hmin[1]] = false;
            }
            if (solution < happiness)
            {
                solution = happiness;
            }
        }
        else // end
        {
            if (attr_inq[ia])
            {
                ++n_inq_expired;
                happiness -= ull_t(attraction.h);
            }
        }
    }

    vattr_t sattr(attractions);
    sort(sattr.begin(), sattr.end());
    int best_day = -1;
    for (ull_t d = 1; d <= D; ++d)
    {
        vu_t h;
        for (const Attraction& a: sattr)
        {
            if ((a.s <= d) && (d <= a.e))
            {
                h.push_back(a.h);
            }
        }
        u_t hsz = h.size();
        u_t hu = min(hsz, K);
        ull_t hbest = 0;
        for (u_t i = hsz - hu; i < hsz; ++i)
        {
            hbest += h[i];
        }
        if (solution < hbest)
        {
            solution = hbest;
            best_day = d;
        }
    }
    if (dbg_flags & 0x1) { cerr << "best_day=" << best_day << '\n'; }
}

void Festival::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int real_main(int argc, char ** argv)
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

    void (Festival::*psolve)() =
        (naive ? &Festival::solve_naive : &Festival::solve);
    if (solve_ver == 1) { psolve = &Festival::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Festival festival(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (festival.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        festival.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(u_t D, u_t K, const vattr_t& attractions)
{
    int rc = 0;
    const u_t Nmax_small = 0x10;
    const u_t N = attractions.size();
    ull_t solution_naive = 0, solution = 1;
    if (N <= Nmax_small)
    {
        Festival festival(D, attractions.size(), K, attractions);
        festival.solve_naive();
        solution_naive = festival.get_solution();
    }
    {
        Festival festival(D, attractions.size(), K, attractions);
        festival.solve();
        solution = festival.get_solution();
    }
    if ((N < Nmax_small) && (solution != solution_naive))
    {
        cerr << "Inconsistent: solution_naive=" << solution_naive <<
            " != solution=" << solution << '\n';
        ofstream f("festival-fail.in");
        f << "1\n" << D << ' ' << N << ' ' << K << '\n';
        for (const Attraction& a: attractions)
        {
             f << a.h << ' ' << a.s << ' ' << a.e << '\n';
        }
        f.close();
    }
    cerr << "   .... solution = " << solution << '\n';
    
    return rc;
}

static u_t randint(u_t low, u_t high)
{
    if (low < high)
    {
        low += rand() % (high - low);
    }
    return low;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    u_t ai = 0;
    const u_t T = strtoul(argv[ai++], 0, 0);
    const u_t Dmin = strtoul(argv[ai++], 0, 0);
    const u_t Dmax = strtoul(argv[ai++], 0, 0);
    const u_t Nmin = strtoul(argv[ai++], 0, 0);
    const u_t Nmax = strtoul(argv[ai++], 0, 0);
    const u_t Kmin = strtoul(argv[ai++], 0, 0);
    const u_t Kmax = strtoul(argv[ai++], 0, 0);
    const u_t Hmin = strtoul(argv[ai++], 0, 0);
    const u_t Hmax = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < T); ++ti)
    {
        const u_t D = randint(Dmin, Dmax);
        const u_t N = randint(Nmin, Nmax);
        const u_t K = randint(min(Kmin, N), min(Kmax, N));
        cerr << "Tested " << ti << '/' << T << 
            ", D=" << D << ", N=" << N << ", K=" << K << '\n';
        vattr_t attractions;
        while (attractions.size() < N)
        {
            const u_t h = randint(Hmin, Hmax);
            const u_t s = randint(1, D);
            const u_t e = randint(s, D);
            attractions.push_back(Attraction(h, s, e));
        }
        rc = test_specific(D, K, attractions);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
