// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <unordered_map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<ll_t> vll_t;
typedef unordered_map<ll_t, u_t> ll2u_t;

static unsigned dbg_flags;

class Arith
{
 public:
    Arith(u_t s=0, ll_t d=0, int irep=(-1)) :
        start(s), delta(d), ireplace(irep) {}
    u_t start;
    ll_t delta;
    int ireplace;
};
bool operator<(const Arith& a0, const Arith& a1)
{
    typedef tuple<u_t, ll_t, int> tuple3_t;
    const tuple3_t t0{a0.start, a0.delta, a0.ireplace};
    const tuple3_t t1{a1.start, a1.delta, a1.ireplace};
    bool lt = t0 < t1;
    return lt;
}

class State
{
 public:
    State() {}
    vector<Arith> ariths;
};

class Longest
{
 public:
    Longest(istream& fi);
    Longest(const vll_t& _a) : N(_a.size()), a(_a), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    void improve(u_t l)
    {
        if (solution < l)
        {
            solution = l;
        }
    }
    void state_init(State& state);
    u_t N;
    vll_t a;
    u_t solution;
};

Longest::Longest(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ll_t>(fi), N, back_inserter(a));
}

void Longest::solve_naive()
{
    u_t b_best = 0, e_best = 0, r_best = N;
    vll_t a_scr(a);
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b + 2; e <= N; ++e)
        {
            u_t sz = e - b;
            bool progression = sz <= 3;
            u_t r_progression = N;
            for (int side: {-1, 0, 1})
            {
                for (u_t r = b; (r < e) && !progression; ++r)
                {
                    ll_t save = a_scr[r];
                    if (r == 0)
                    {
                        a_scr[0] = 2*a_scr[1] - a_scr[2];
                    }
                    else if (r + 1 == e)
                    {
                        a_scr[r] = 2*a_scr[r - 1] - a_scr[r - 2];
                    }
                    else
                    {
                        switch (side)
                        {
                         case -1:
                            if (r >= 2)
                            {
                                a_scr[r] = 2*a_scr[r - 1] - a_scr[r - 2];
                            }
                            break;
                         case 0:
                            a_scr[r] = (a_scr[r - 1] + a_scr[r + 1])/2;
                            break;
                         case 1:
                            if (r + 2 < e)
                            {
                                a_scr[r] = 2*a_scr[r + 1] - a_scr[r + 2];
                            }
                            break;
                        }
                    }
                    bool arith = true;
                    const ll_t delta = a_scr[b + 1] - a_scr[b];
                    for (u_t k = b + 1; arith && (k < e); ++k)
                    {
                        arith = (delta == a_scr[k] - a_scr[k - 1]);
                    }
                    progression = arith;
                    if (progression)
                    {
                        r_progression = r;
                    }
                    a_scr[r] = save;
                }
            }
            if (progression && (solution < sz))
            {
                solution = sz;
                b_best = b; e_best = e; r_best = r_progression;
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "b="<<b_best << ", e="<<e_best <<
        ", r=" << r_best << '\n'; }
}

void maxby(u_t& v, u_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Longest::solve()
{
    solution = (N <= 3 ? N : 3);
    if (N >= 4)
    {
        State state;
        state_init(state);
        size_t max_arith = state.ariths.size();
        for (int i = 3; i < int(N); ++i)
        {
            const ll_t delta = a[i] - a[i - 1];
            for (u_t ai = 0; ai < state.ariths.size(); )
            {
                Arith& arith = state.ariths[ai];
                if (delta == arith.delta)
                {
                    if ((arith.ireplace == -1) || (arith.ireplace + 1 != i))
                    {
                        improve(i + 1 - arith.start);
                        ++ai;
                    }
                    else // delete it
                    {
                        arith = state.ariths.back();
                        state.ariths.pop_back();
                    }
                }
                else
                {
                    if (arith.ireplace == -1)
                    {
                        arith.ireplace = i;
                        improve(i + 1 - arith.start);
                        ++ai;
                    }
                    else if ((arith.ireplace + 1 == i) &&
                        (2*arith.delta == a[i] - a[i - 2]))
                    {
                        improve(i + 1 - arith.start);
                        ++ai;
                    }
                    else // delete it
                    {
                        arith = state.ariths.back();
                        state.ariths.pop_back();
                    }
                }
            }
            if (delta != a[i - 1] - a[i - 2])
            {
                state.ariths.push_back(Arith(i - 1, delta));
                state.ariths.push_back(Arith(i - 2, delta, i - 1));
            }
            if ((2*a[i - 1] != (a[i - 2] + a[i]) &&
                ((a[i - 2] + a[i]) % 2 == 0)))
            {
                int new_delta = (a[i] - a[i - 2])/2;
                if (new_delta != delta)
                {
                    state.ariths.push_back(Arith(i - 2, new_delta, i - 1));
                }
            }
            max_arith = max(max_arith, state.ariths.size());
        }
        if (dbg_flags & 0x100) { cerr << "max_arith="<<max_arith << '\n'; }
    }
}

void Longest::state_init(State& state)
{
    // state.ariths.push_back(Arith(0, a[1] - a[0]));
    if ((a[0] + a[2]) % 2 == 0)
    {
        if ((a[0] + a[2]) == 2*a[1])
        {
            state.ariths.push_back(Arith(0, (a[1] - a[0])));
        }
        else
        {
            state.ariths.push_back(Arith(0, a[1] - a[0], 2));
            state.ariths.push_back(Arith(1, a[2] - a[1]));
            state.ariths.push_back(Arith(0, a[2] - a[1], 0));
            state.ariths.push_back(Arith(0, (a[2] - a[0])/2, 1));
        }
    }
    else if ((a[0] + a[2]) != 2*a[1])
    {
        state.ariths.push_back(Arith(0, a[1] - a[0], 2));
        state.ariths.push_back(Arith(1, a[2] - a[1]));
        state.ariths.push_back(Arith(0, a[2] - a[1], 0));
    }
}

void Longest::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (Longest::*psolve)() =
        (naive ? &Longest::solve_naive : &Longest::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Longest longest(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (longest.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        longest.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(vll_t& a)
{
    int rc = 0;
    u_t solution_naive = a.size() + 1, solution = a.size() + 2;
    const u_t N = a.size();
    bool small = (N < 20);
    if (small)
    {
        Longest p(a);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Longest p(a);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Inconsistent\n";
        ofstream f("longest-fail.in");
        f << "1\n" << a.size();
        const char *sep = "\n";
        for (ll_t x: a)
        {
            f << sep << x; sep = " ";
        }
        f << '\n';
        f.close();
    }
    cerr << "N=" << N << ", solution=" << solution << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    ll_t delta_max = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests << ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
        ", delta_max="<<delta_max << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        dbg_flags = (ti == 0 ? 0x100 : 0);
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = Nmin + (rand() % (Nmax + 1 - Nmin));
        vll_t a;
        a.push_back(0);
        while (a.size() < N)
        {
            ll_t delta = -delta_max + (rand() % (2*delta_max + 1));
            a.push_back(a.back() + delta);
        }
        rc = test_case(a);
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
