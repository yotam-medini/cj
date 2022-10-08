// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
// typedef unordered_map<ull_t, vu_t> ull_to_vu_t;
class Best
{
 public:
    Best(ull_t t=0, u_t n=0) : time(t), next(n) {}
    ull_t time;
    u_t next;
};
typedef unordered_map<ull_t, Best> memo_t;

static unsigned dbg_flags;

class Touchbar
{
 public:
    Touchbar(istream& fi);
    Touchbar(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void set_dupless();
    void set_key2pos();
    ull_t get_time_from(u_t si, u_t ki);
    void show_solution_path(u_t p0) const;
    u_t N;
    vu_t S;
    u_t M;
    vu_t K;
    ull_t solution;
    vu_t s_dupless;
    vvu_t key2pos;
    memo_t memo;
};

Touchbar::Touchbar(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(S));
    fi >> M;
    copy_n(istream_iterator<u_t>(fi), M, back_inserter(K));
}

void Touchbar::solve_naive()
{
    set_dupless();
    set_key2pos();
    solution = ull_t(N) * ull_t(M);
    u_t k0 = s_dupless[0];
    u_t solution_path_0 = N; // undef
    for (u_t ki: key2pos[k0])
    {
        ull_t t = get_time_from(0, ki);
        if (solution > t)
        {
            solution = t;
            solution_path_0 = ki;
        }
    }
    if (dbg_flags & 0x1) { show_solution_path(solution_path_0); }
}

void Touchbar::solve()
{
    solve_naive();
}

void Touchbar::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void Touchbar::set_dupless()
{
    s_dupless.clear();
    s_dupless.push_back(S[0]);
    for (u_t n: S)
    {
        if (s_dupless.back() != n)
        {
            s_dupless.push_back(n);
        }
    }
}

void Touchbar::set_key2pos()
{
    u_t kmax = *max_element(K.begin(), K.end());
    key2pos = vvu_t(kmax + 1, vu_t());
    for (u_t i = 0; i < K.size(); ++i)
    {
        key2pos[K[i]].push_back(i);
    }
}

ull_t Touchbar::get_time_from(u_t si, u_t ki)
{
    ull_t time = 0;
    if (si < s_dupless.size())
    {
        ull_t memo_key = (ull_t(si) << 16) | ull_t(ki);
        memo_t::iterator iter = memo.find(memo_key);
        if (iter == memo.end())
        {
            u_t skey = s_dupless[si];
            Best best(ull_t(N) * ull_t(M), M); // undef
            for (u_t kj: key2pos[skey])
            {
                const ull_t ij_delta = (ki < kj ? kj - ki : ki - kj);
                const ull_t kj_time = get_time_from(si + 1, kj);
                const ull_t ki_time = ij_delta + kj_time;
                if (best.time > ki_time)
                {
                    best.time = ki_time;
                    best.next = kj;
                }
            }
            time = best.time;
            memo.insert(iter, memo_t::value_type(memo_key, best));
        }
        else
        {
            time = iter->second.time;
        }
    }
    return time;
}

void Touchbar::show_solution_path(u_t ki) const
{
    cerr << "Path:";
    for (u_t si = 0; si < s_dupless.size(); ++si)
    {
        cerr << ' ' << ki;
        ull_t memo_key = (ull_t(si) << 16) | ull_t(ki);
        memo_t::const_iterator iter = memo.find(memo_key);
        ki = iter->second.next;
    }
    cerr << '\n';
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

    void (Touchbar::*psolve)() =
        (naive ? &Touchbar::solve_naive : &Touchbar::solve);
    if (solve_ver == 1) { psolve = &Touchbar::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Touchbar touchbar(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (touchbar.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        touchbar.print_solution(fout);
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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Touchbar p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Touchbar p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("touchbar-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
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
