// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;

static unsigned dbg_flags;

class Ant
{
 public:
    Ant(ll_t p=0, int d=0) : hcm_pos(p), dir(d) {}
    void step() { hcm_pos += ll_t(dir); }
    ll_t hcm_pos; // half cm position;
    int dir; // +1 || -1
};
typedef vector<Ant> vant_t;

class TimeSide
{
 public:
    TimeSide(ull_t t=0, u_t s=0) :  time(t), side(s) {}
    ull_t time;
    u_t side;
};
bool operator<(const TimeSide& ts0, const TimeSide& ts1)
{
    return make_tuple(ts0.time, ts0.side) < make_tuple(ts1.time, ts1.side);
}
typedef vector<TimeSide> vts_t;

class Ants
{
 public:
    Ants(istream& fi);
    Ants(ull_t _L, const vull_t& _P, const vu_t& _D) :
        N(_P.size()), L(_L), P(_P), D(_D) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t& get_solution() const { return solution; }
 private:
    u_t N;
    ull_t L;
    vull_t P;
    vu_t D;
    vu_t solution;
};

Ants::Ants(istream& fi)
{
    fi >> N >> L;
    P.reserve(N);
    D.reserve(N);
    while (P.size() < N)
    {
        ull_t p; u_t d;
        fi >> p >> d;
        P.push_back(p);
        D.push_back(d);
    }
}

void Ants::solve_naive()
{
    vant_t ants; ants.reserve(N);
    vu_t active;
    for (u_t i = 0; i < N; ++i)
    {
        ants.push_back(Ant(2*P[i], D[i] == 0 ? -1 : +1));
        active.push_back(i);
    }
    // simulate
    const ll_t Lhcm = 2*L;
    while (!active.empty())
    {
        vu_t drop;
        for (u_t aci = 0; aci < active.size(); ++aci)
        {
            u_t ani = active[aci];
            Ant& ant = ants[ani];
            ant.step();
            if ((ant.hcm_pos == 0) || (ant.hcm_pos == Lhcm))
            {
                drop.push_back(aci);
            }
        }
        if (!drop.empty())
        {
            for (u_t aci: drop)
            {
                solution.push_back(active[aci]);
            }
            for (u_t di = drop.size(); di > 0; )
            {
                --di;
                u_t aci = drop[di];
                active[aci] = active.back();
                active.pop_back();
            }
            sort(active.begin(), active.end());
        }
        for (u_t i = 0; i < active.size(); ++i)
        {
            for (u_t j = i + 1; j < active.size(); ++j)
            {
                Ant& ant_l = ants[active[i]];
                Ant& ant_r = ants[active[j]];
                if (ant_l.hcm_pos == ant_r.hcm_pos)
                {
                    swap(ant_l.dir, ant_r.dir);
                }
            }
        }
    }
}

void Ants::solve()
{
    vts_t drop_events; drop_events.reserve(N);
    vaull2_t pos_idx; pos_idx.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        ull_t drop_time = (D[i] == 0 ? P[i] : L - P[i]);
        drop_events.push_back(TimeSide(drop_time, D[i]));
        pos_idx.push_back(aull2_t{P[i], i});
    }
    sort(drop_events.begin(), drop_events.end());
    sort(pos_idx.begin(), pos_idx.end());
    for (u_t i = 0, pleft = 0, pright = N; i < N; )
    {
        const ull_t t = drop_events[i].time;
        vu_t drop;
        for ( ; (i < N) && (drop_events[i].time == t); ++i)
        {
            u_t ai = u_t(-1);
            if (drop_events[i].side == 0)
            {
                ai = pos_idx[pleft++][1];
            }
            else
            {
                ai = pos_idx[--pright][1];
            }
            drop.push_back(ai);
        }
        sort(drop.begin(), drop.end());
        solution.insert(solution.end(), drop.begin(), drop.end());
    }
}

void Ants::print_solution(ostream &fo) const
{
    for (u_t a: solution) { fo << ' ' << (a + 1); }
}

static int real_main(int argc, char ** argv)
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Ants::*psolve)() =
        (naive ? &Ants::solve_naive : &Ants::solve);
    if (solve_ver == 1) { psolve = &Ants::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Ants ants(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ants.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ants.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char* fn, ull_t L, const vull_t& P, const vu_t& D)
{
    ofstream f(fn);
    const ull_t N = P.size();
    f << "1\n" << N << ' ' << L << '\n';
    for (u_t i = 0; i < N; ++i)
    {
        f << P[i] << ' ' << D[i] << '\n';
    }
    f.close();
}

static int test_case(ull_t L, const vull_t& P, const vu_t& D)
{
    int rc = 0;
    const ull_t N = P.size();
    vu_t solution, solution_naive;
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("ants-curr.in", L, P, D); }
    if (small)
    {
        Ants p(L, P, D);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Ants p(L, P, D);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: inconsistent solutions\n";
        save_case("ants-fail.in", L, P, D);
    }
    if (rc == 0) { cerr << "  N=" << N << ", L=" << L <<
        (small ? " (small) " : " (large) ") << " --> ";
        for (u_t i = 0; i < min<u_t>(4, solution.size()); ++i) {
            cerr << ' ' << solution[i]; } cerr << " ...\n"; }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const ull_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const ull_t Lmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Lmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests << '\n' <<
        ", Nmin="<<Nmin << ", Nmax="<<Nmax << 
        ", Lmin="<<Lmin << ", Lmax="<<Lmax << 
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        ull_t L = rand_range(Lmin, Lmax);
        ull_t N = rand_range(min(Nmin, L - 1), min(Nmax, L - 1));
        unordered_set<ull_t> positions;
        while (positions.size() < N)
        {
            positions.insert(rand_range(1, L - 1));
        }
        vull_t P(positions.begin(), positions.end());
        random_shuffle(P.begin(), P.end());
        vu_t D; D.reserve(N);
        while (D.size() < N)
        {
            D.push_back(rand() % 2);
        }
        rc = test_case(L, P, D);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
