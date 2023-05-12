// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
#include <tuple>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class ScoreIdx
{
 public:
    ScoreIdx(ull_t _score=0, u_t _i=0) : score(_score), i(_i) {}
    auto as_tuple() const { return make_tuple(score, i); }
    ull_t score;
    u_t i;
};
bool operator<(const ScoreIdx& si0, const ScoreIdx& si1)
{
    return si0.as_tuple() < si1.as_tuple();
}
typedef vector<ScoreIdx> vsi_t;

class Evolutionary
{
 public:
    Evolutionary(istream& fi);
    Evolutionary(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void set_childern();
    bool is_ancestor(u_t p, u_t c) const;
    u_t N;
    ull_t K;
    vull_t S;
    vi_t P;
    ull_t solution;
    vvu_t children;
};

Evolutionary::Evolutionary(istream& fi) : solution(0)
{
    fi >> N >> K;
    S.reserve(N + 1);
    P.reserve(N + 1);
    S.push_back(0); // dummy
    P.push_back(-1); // dummy
    P.push_back(-1); // dummy
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(S));
    copy_n(istream_iterator<u_t>(fi), N - 1, back_inserter(P));
}

void Evolutionary::solve_naive()
{
    // set_childern();
    vsi_t scores_idx; scores_idx.reserve(N);
    for (u_t i = 1; i <= N; ++i)
    {
        scores_idx.push_back(ScoreIdx(S[i], i));
    }
    sort(scores_idx.begin(), scores_idx.end());
    for (u_t bi = 0; bi < N; ++bi)
    {
        const ull_t score = scores_idx[bi].score;
        const ull_t sdk = (score + (K - 1)) / K;
        const u_t b = scores_idx[bi].i;
        vsi_t::const_iterator iter = lower_bound(
            scores_idx.begin(), scores_idx.end(), sdk,
                [](const ScoreIdx& si, ull_t _sdk) -> bool
                {
                    bool lt = si.score < _sdk;
                    if (dbg_flags & 0x2) { cerr << "lt="<<lt <<
                        ", score="<<si.score<< ", _sdk="<<_sdk<< '\n'; }
                    return lt;
                });
        u_t small_e = iter - scores_idx.begin();
        if (dbg_flags & 0x1) { cerr << "bi="<<bi << ", b="<<b <<
            ", score="<<score << ", sdk="<<sdk << ", small_e="<<small_e << '\n';
        }
        ull_t na = 0, nc = 0;
        for (u_t sj = 0; sj < small_e; sj++)
        {
            u_t si = scores_idx[sj].i;
            if (is_ancestor(b, si))
            {
                ++na;
            }
            else
            {
               ++nc;
            }
        }
        ull_t b_triad = na*nc;
        solution += b_triad;
    }
}

void Evolutionary::set_childern()
{
    children.assign(N + 1, vu_t());
    for (u_t c = 2; c <= N; ++c)
    {
        u_t p = P[c];
        children[p].push_back(c);
    }
}

bool Evolutionary::is_ancestor(u_t p, u_t c) const
{
    const int ip = p;
    int a;
    for (a = c; (a != -1) && (a != ip); a = P[a]) {}
    bool isa = (a == ip);
    return isa;
}

void Evolutionary::solve()
{
    solve_naive();
}

void Evolutionary::print_solution(ostream &fo) const
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

    void (Evolutionary::*psolve)() =
        (naive ? &Evolutionary::solve_naive : &Evolutionary::solve);
    if (solve_ver == 1) { psolve = &Evolutionary::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Evolutionary evolutionary(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (evolutionary.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        evolutionary.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("evolutionary-curr.in"); }
    if (small)
    {
        Evolutionary p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Evolutionary p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("evolutionary-fail.in");
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
