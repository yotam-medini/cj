// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
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
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;

static unsigned dbg_flags;

class Chain
{
 public:
    Chain(istream& fi);
    Chain(const vull_t& _F, const vu_t& _P) : 
        N(_F.size()), F(_F), P(_P), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void get_initiators(vu_t& initiators) const;
    ull_t simulate(const vu_t& initiators, const vu_t& order) const;
    void build_graph();
    void get_root_funs(u_t root, vull_t& funs);
    u_t N;
    vull_t F;
    vull_t F_over;
    vu_t P;
    vu_t Pm1;
    ull_t solution;
    vvu_t graph;
    vu_t roots;
};

Chain::Chain(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(F));
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(P));
}

void Chain::solve_naive()
{
    Pm1.reserve(N);
    for (u_t p: P)
    {
        Pm1.push_back(p > 0 ? p - 1 : N);
    }
    vu_t initiators;
    get_initiators(initiators);
    const u_t n_initiators = initiators.size();
    vu_t perm(size_t(n_initiators), 0);
    iota(perm.begin(), perm.end(), 0);
    for (bool permutate = true; permutate; 
        permutate = next_permutation(perm.begin(), perm.end()))
    {
        ull_t perm_fun = simulate(initiators, perm);
        if (solution < perm_fun)
        {
            solution = perm_fun;
        }
    }
}

ull_t Chain::simulate(const vu_t& initiators, const vu_t& order) const
{
    ull_t total_fun = 0;
    vull_t active_fun(F);
    for (u_t i: order)
    {
        ull_t fun_max = 0;
        for (u_t m = initiators[i]; m != N; m = Pm1[m])
        {
            fun_max = max(fun_max, active_fun[m]);
            active_fun[m] = 0;
        }
        total_fun += fun_max;
    }
    return total_fun;
}

void Chain::solve()
{
#if 1
    solve_naive();
#else 
    // Bad algorithm for now...
    Pm1.reserve(N);
    for (u_t p: P)
    {
        Pm1.push_back(p > 0 ? p - 1 : N);
    }
    build_graph();
    F_over = F;
    for (u_t root: roots)
    {
        vull_t root_funs;
        get_root_funs(root, root_funs);
        ull_t root_fun = accumulate(root_funs.begin(), root_funs.end(), 0ull);
        solution += root_fun;
    }
#endif
}

void Chain::get_initiators(vu_t& initiators) const
{
    vector<bool> maybe(N, true);
    for (u_t p: Pm1)
    {
        maybe[p] = false;
    }
    initiators.clear();
    for (u_t i = 0; i < N; ++i)
    {
        if (maybe[i])
        {
            initiators.push_back(i);
        }
    }

}

void Chain::build_graph()
{
    graph.insert(graph.end(), N, vu_t());
    for (u_t i = 0; i < N; ++i)
    {
        u_t v = Pm1[i];
        if (v == N)
        {
            roots.push_back(i);
        }
        else
        {
            graph[v].push_back(i);
        }
    }
}

void Chain::get_root_funs(u_t root, vull_t& funs)
{
    funs.clear();
    ull_t root_fun = F_over[root];
    if (graph[root].empty())
    {
        funs.push_back(root_fun);
    }
    else
    {
        if ((graph[root].size() == 1) && 
            (F_over[graph[root][0]] < F_over[root]))
        {
            F_over[graph[root][0]] = F_over[root];
        }
        for (u_t a: graph[root])
        {
            vull_t afuns;
            get_root_funs(a, afuns);
            funs.insert(funs.end(), afuns.begin(), afuns.end());
        }
        vull_t::iterator iter = min_element(funs.begin(), funs.end());
        const u_t vmin = *iter;
        if (vmin < root_fun)
        {
            *iter = root_fun;
        }
    }
    if (dbg_flags & 0x1) { cerr << "root=" << root << ":";
        for (ull_t fun: funs) { cerr << ' ' << fun; }
        cerr << '\n';
    }
}

void Chain::print_solution(ostream &fo) const
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

    void (Chain::*psolve)() =
        (naive ? &Chain::solve_naive : &Chain::solve);
    if (solve_ver == 1) { psolve = &Chain::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Chain chain(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (chain.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        chain.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vull_t& F, const vu_t& P)
{
    int rc = 0;
    ull_t solution_naive = ull_t(-1), solution = ull_t(-1);
    const u_t N = F.size();
    {
        Chain chain(F, P);
        chain.solve();
        solution = chain.get_solution();
    }
    if (N < 10)
    {
        Chain chain(F, P);
        chain.solve_naive();
        solution_naive = chain.get_solution();
    }
    if (N < 10)
    {
        if (solution_naive != solution)
        {
            rc = 1;
            cerr << "Solution: naive = "<<solution_naive <<
                " != prod = " << solution << '\n';
            ofstream f("chain-fail.in");
            f << "1\n" << N << "\n";
            const char *sep = "";
            for (ull_t fun: F) { f << sep << fun; sep = " "; }
            f << '\n';
            sep = "";
            for (u_t p: P) { f << sep << p; sep = " "; }
            f << '\n';
            f.close();
        }
    }
    return rc;
}

template <typename T>
T randint(T low, T high)
{
    low += rand() % (high + 1 - low);
    return low;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    u_t Fmin = strtoul(argv[ai++], 0, 0);
    u_t Fmax = strtoul(argv[ai++], 0, 0);
    cerr << "Nmin="<<Nmin << ", Nmax="<<Nmax << 
        "Fmin="<<Fmin << ", Fmax="<<Fmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = randint(Nmin, Nmax);
        vull_t F; vu_t P;
        for (u_t i = 1; i <= N; ++i)
        {
            F.push_back(randint(Fmin, Fmax));
            P.push_back(randint<u_t>(0, i - 1));
        }
        rc = test_case(F, P);
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
