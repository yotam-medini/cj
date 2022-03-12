// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
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
typedef map<vau2_t, u_t> graph_t;
typedef map<u_t, u_t> u2u_t;

static unsigned dbg_flags;

static u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

class Subx
{
 public:
    Subx(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef u2u_t node_t;
    typedef queue<node_t> q_t;
    typedef set<node_t> setnode_t;
    bool is_possible() const;
    void bfs();
    void node_check_add(const node_t& node);
    u_t N, A, B;
    vu_t U;
    u_t solution;
    q_t q;
    setnode_t nodes_seen;
};

Subx::Subx(istream& fi) : solution(0)
{
    fi >> N >> A >> B;
    U.reserve(N + 1);
    U.push_back(0); // dummy, [0] ignored
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(U));
}

void Subx::solve_naive()
{
    if (is_possible())
    {
         bfs();
    }
}

bool Subx::is_possible() const
{
    const u_t g = gcd(A, B);
    const u_t modn = U[N] % g;
    bool possible = true;
    for (u_t i = 1; possible && (i < N); ++i)
    {
        possible = (U[i] == 0) || ((i % g) == modn);
    }
    return possible;
}

static void u2u_dec(u2u_t& u2u, u_t k)
{
    u2u_t::iterator iter = u2u.find(k);
    if (iter->second == 1)
    {
        u2u.erase(iter);
    }
    else
    {
        --iter->second;
    }
}

static void u2u_inc(u2u_t& u2u, u_t k)
{
    u2u_t::iterator iter = u2u.find(k);
    if (iter == u2u.end())
    {
        u2u.insert(u2u.end(), u2u_t::value_type(k, 1));
    }
    else
    {
        ++iter->second;
    }
}

void Subx::bfs()
{
    u2u_t v;
    for (u_t i = 1; i <= N; ++i)
    {
        if (U[i] > 0)
        {
            v.insert(v.end(), u2u_t::value_type(i, U[i]));
        }
    }
    q.push(v);
    nodes_seen.insert(nodes_seen.end(), v);
    const u_t dba = B - A;
    while (!q.empty())
    {
        const node_t& node = q.front();
        const u2u_t::value_type& nu = *(node.begin());
        if ((node.size() == 1) && (nu.second == 1) &&
           ((solution == 0) || (solution > nu.first)))
        {
            solution = nu.first;
        }
        const u_t vmax = node.rbegin()->first;
        for (u2u_t::const_iterator iter = node.begin(), itere = node.end();
            iter != itere; ++iter)
        {
            u_t low = iter->first, high = low + dba;
            if ((solution == 0) || (low + A < solution))
            {
                u2u_t::const_iterator iterh = node.find(high);
                if (iterh != node.end())
                {
                    node_t subnode(node);
                    u2u_dec(subnode, low);
                    u2u_dec(subnode, high);
                    u2u_inc(subnode, high + A);
                    node_check_add(subnode);
                }
                if (low < vmax)
                {
                    for (u_t a: {A, B})
                    {
                        node_t subnode(node);
                        u2u_dec(subnode, low);
                        u2u_inc(subnode, low + a);
                        node_check_add(subnode);
                    }
                }
            }
        }
        q.pop();
    }
}

void Subx::node_check_add(const node_t& node)
{
    if (nodes_seen.find(node) == nodes_seen.end())
    {
        nodes_seen.insert(nodes_seen.end(), node);
        q.push(node);
    }
}

void Subx::solve()
{
    solve_naive();
}

void Subx::print_solution(ostream &fo) const
{
    if (solution > 0)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
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

    void (Subx::*psolve)() =
        (naive ? &Subx::solve_naive : &Subx::solve);
    if (solve_ver == 1) { psolve = &Subx::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Subx subx(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (subx.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        subx.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
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
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
