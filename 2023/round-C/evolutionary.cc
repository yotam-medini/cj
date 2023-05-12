// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
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
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

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

class Node
{
 public:
    Node(u_t _level=0) : level(_level) {}
    u_t level;
    vu_t children;
    vu_t geo_ancestors; // geo_ancestors[p] = ancesor above 2^p steps
};
typedef vector<Node> vnode_t;

class Evolutionary;
using pf_is_ancestor_t = bool (Evolutionary::*)(u_t p, u_t c) const;

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
    void solve_common(pf_is_ancestor_t);
    void set_childern();
    void set_nodes();
    bool is_ancestor_naive(u_t p, u_t c) const;
    bool is_ancestor(u_t p, u_t c) const;
    u_t N;
    ull_t K;
    vull_t S;
    vi_t P;
    ull_t solution;
    vnode_t nodes;
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
    solve_common(&Evolutionary::is_ancestor_naive);
}

void Evolutionary::solve()
{
    set_nodes();
    solve_common(&Evolutionary::is_ancestor);
}

void Evolutionary::solve_common(pf_is_ancestor_t pf_is_ancestor)
{
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
            if ((this->*pf_is_ancestor)(b, si))
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
    for (u_t c = 2; c <= N; ++c)
    {
        u_t p = P[c];
        nodes[p].children.push_back(c);
    }
}

void Evolutionary::set_nodes()
{
    nodes.assign(N + 1, Node());
    set_childern();
    vau2_t stack;
    stack.push_back(au2_t{1, 0});
    while (!stack.empty())
    {
        au2_t& stack_node = stack.back();
        u_t s = stack_node[0];
        u_t ci = stack_node[1]++;
        Node& node = nodes[s];
        if (ci == 0)
        {
            u_t level = node.level = stack.size() - 1;
            for (u_t p2_step = 0, step = 1; step <= level; ++p2_step, step *= 2)
            {
                u_t si = stack.size() - 1 - step;
                u_t ancestor = stack[si][0];
                node.geo_ancestors.push_back(ancestor);
            }
        }
        const vu_t& children = node.children;
        if (ci < children.size())
        {
            stack.push_back(au2_t{children[ci], 0});
        }
        else
        {
            stack.pop_back();
        }
    }
    if (dbg_flags & 0x1) {
        for (u_t s = 1; s <= N; ++s) {
            const Node& node = nodes[s];
            cerr << "node["<<s<<"], level="<<node.level;
            const vu_t& children = node.children;
            if (!children.empty()) {
                cerr << ", children:";
                for (u_t c: children) { cerr << ' ' << c; }
            }
            cerr << ", Geo-Ancestors:";
            for (u_t a: node.geo_ancestors) { cerr << ' ' << a; }
            cerr << '\n';
        }
    }
}

bool Evolutionary::is_ancestor_naive(u_t p, u_t c) const
{
    const int ip = p;
    int a;
    for (a = c; (a != -1) && (a != ip); a = P[a]) {}
    bool isa = (a == ip);
    return isa;
}

bool Evolutionary::is_ancestor(u_t p, u_t c) const
{
    bool isa = false;
    const u_t p_level = nodes[p].level;
    const u_t c_level = nodes[c].level;
    if (p_level < c_level)
    {
        u_t steps = c_level - p_level;
        u_t s = c;
        while (steps > 0)
        {
            u_t pwr = 0, p2_steps = 1;;
            for ( ; 2*p2_steps <= steps; ++pwr, p2_steps *= 2) {}
            s = nodes[s].geo_ancestors[pwr];
            steps -= p2_steps;
        }
        isa = (s == p);
    }
    return isa;
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
