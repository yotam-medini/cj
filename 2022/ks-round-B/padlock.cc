// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Padlock
{
 public:
    Padlock(istream& fi);
    Padlock(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    typedef unordered_map<ull_t, ull_t> memo_t;
    ull_t dbe_to_key(ull_t d, u_t b, u_t e) const
    {
        ull_t key = (d << 32) | (ull_t(b) << 16) | ull_t(e);
        return key;
    }
    void key_to_dbe(ull_t& d, u_t& b, u_t&e, ull_t key) const
    {
        d = (key >> 32) & ((ull_t(1) << 32) - 1);
        b = (key >> 16) & 0xffff;
        e = key & 0xffff;
    }
    ull_t n_ops_from(ull_t d, u_t b, u_t e);
    ull_t delta_d(ull_t d0, ull_t d1)
    {
        ull_t d = d0 < d1 ? d1 - d0 : d0 - d1;
        d = min(d, D - d);
        return d;
    }
    u_t N;
    ul_t D;
    vul_t V;
    ull_t solution;
    memo_t memo;
};

Padlock::Padlock(istream& fi) : solution(0)
{
    fi >> N >> D;
    copy_n(istream_iterator<ul_t>(fi), N, back_inserter(V));
}

static void min_by(ull_t& v, ull_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void Padlock::solve_naive()
{
    ull_t min_ops = ull_t(-1);
    for (u_t b = 0, e = 0; b < N; b = e)
    {
        const ul_t d = V[b];
        for (e = b; (e < N) && (V[e] == d); ++e) {}
        ull_t n_ops = n_ops_from(d, b, e);
        min_by(min_ops, n_ops);
    }
    solution = min_ops;
}

void Padlock::solve()
{
    solve_naive();
}

ull_t Padlock::n_ops_from(ull_t d, u_t b, u_t e)
{
    ull_t key = dbe_to_key(d, b, e);
    memo_t::iterator iter = memo.find(key);
    if (iter == memo.end())
    {
        ull_t n_ops = ull_t(-1);
        if ((b == 0) && (e == N))
        {
            n_ops = min(d, D - d);
        }
        else
        {
            if (b > 0)
            {
                u_t bb = b - 1;
                const ul_t dbb = V[bb];
                for (; (bb > 0) && (V[bb - 1] == dbb); --bb) {}
                ull_t delta = delta_d(d, dbb);
                ull_t ops = delta + n_ops_from(dbb, bb, e);
                min_by(n_ops, ops);
            }
            if (e < N)
            {
                u_t ee = e;
                const ul_t dee = V[ee];
                for (; (ee < N) && (V[ee] == dee); ++ee) {}
                ull_t delta = delta_d(d, dee);
                ull_t ops = delta + n_ops_from(dee, b, ee);
                min_by(n_ops, ops);
            }
        }
        iter = memo.insert(iter, memo_t::value_type(key, n_ops));
    }
    return iter->second;
}

void Padlock::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Padlock::*psolve)() =
        (naive ? &Padlock::solve_naive : &Padlock::solve);
    if (solve_ver == 1) { psolve = &Padlock::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Padlock padlock(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (padlock.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        padlock.print_solution(fout);
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

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("padlock-curr.in"); }
    if (small)
    {
        Padlock p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Padlock p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("padlock-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
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
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
     cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
     for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
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
