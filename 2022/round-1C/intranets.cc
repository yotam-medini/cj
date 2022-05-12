// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
// #include <unordered_set>
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
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
// typedef unordered_set<ut> unordu_t;
typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;

static unsigned dbg_flags;

static const ull_t MOD_BIG = 1000000000 + 7; // 1000000007

class KCount
{
 public:
    KCount(u_t _M): M(_M) { run(); }
    u_t M;
    vull_t counters;
 private:
    void run();
    ull_t components_count() const;
    vvu_t edge_priorities;
    vvu_t active;
};

void KCount::run()
{
    counters = vull_t(M/2 + 1, 0);
    u_t E = (M*(M - 1))/2;
    vau2_t ijs; ijs.reserve(E);
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t j = i + 1; j < M; ++j)
        {
            ijs.push_back(au2_t{i, j});
        }
    }
    edge_priorities = vvu_t(M, vu_t(M, 0));
    active = vvu_t(M, vu_t());
    vu_t perm(E, 0);
    iota(perm.begin(), perm.end(), 0);
    ull_t Efact = 1;
    for (ull_t n = 1; n <= E; ++n) { Efact *= n; }
    if (dbg_flags & 0x1) { cerr << "M="<<M<<", E="<<E<<", Efact="<<Efact<<'\n'; }
    ull_t n = 0;
    for (bool more = true; more; 
        more = next_permutation(perm.begin(), perm.end()), ++n)
    {
        if ((dbg_flags & 0x1) && ((n & (n - 1)) == 0)) {
             cerr << "Ran " << n << " / " << Efact << " permutations\n"; }
        for (u_t p = 0; p < E; ++p) // both perm index & priority
        {
            const au2_t& ij = ijs[perm[p]];
            u_t i = ij[0], j = ij[1];
            edge_priorities[i][j] = p;
            edge_priorities[j][i] = p;
        }
        vsetu_t vset_adjs = vsetu_t(M, setu_t());
        for (u_t i = 0; i < M; ++i)
        {
            u_t jlink = i; // undef
            u_t pbest = 0;
            for (u_t j = 0; j < M; ++j)
            {
                if (i != j)
                {
                    if (pbest <= edge_priorities[i][j])
                    {
                         pbest = edge_priorities[i][j];
                         jlink = j;
                    }
                }
            }
            vset_adjs[i].insert(jlink);
            vset_adjs[jlink].insert(i);
        }
        for (u_t i = 0; i < M; ++i)
        {
            active[i] = vu_t(vset_adjs[i].begin(), vset_adjs[i].end());
        }
        ull_t cc = components_count();
        ++counters[cc];
    }
}

ull_t KCount::components_count() const
{
    ull_t n = 0;
    vector<bool> visited(M, false);
    vu_t node2c(M, 0);
    for (u_t v = 0; v < M; ++v)
    {
        if (!visited[v])
        {
            node2c[v] = n;
            setu_t q;
            q.insert(v);
            while (!q.empty())
            {
                u_t node = *q.begin();
                q.erase(q.begin());
                for (u_t a: active[node])
                {
                    if (!visited[a])
                    {
                        visited[a] = true;
                        node2c[a] = n;
                        q.insert(a);
                    }
                }
            }
            ++n;
        }
    }
    return n;
}

class Intranets
{
 public:
    Intranets(istream& fi);
    Intranets(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t M;
    ull_t solution;
};

Intranets::Intranets(istream& fi) : solution(0)
{
    fi >> M;
}

void Intranets::solve_naive()
{
}

void Intranets::solve()
{
    solve_naive();
}

void Intranets::print_solution(ostream &fo) const
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

    void (Intranets::*psolve)() =
        (naive ? &Intranets::solve_naive : &Intranets::solve);
    if (solve_ver == 1) { psolve = &Intranets::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Intranets intranets(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (intranets.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        intranets.print_solution(fout);
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
        Intranets p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Intranets p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("intranets-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = test_case(argc, argv);
    dbg_flags |= 0x1;
    for (u_t M : {3, 4, 5, 6})
    {
        KCount kc(M);
        cout << "M = " << M;
        for (u_t i = 0; i < kc.counters.size(); ++i)
        {
            cout << "  #(" << i << " components) = " << kc.counters[i] << '\n';
        }
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
