// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 3> aull3_t;
typedef vector<au2_t> vau2_t;
typedef vector<aull3_t> vaull3_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<bool> vb_t;
typedef set<u_t> setu_t;
typedef set<vu_t> setvu_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Graph
{
 public:
    Graph(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void set_adjs();
    bool possible(const vu_t& path);
    u_t N, M, K;
    vaull3_t rooms; // L. R, A
    vau2_t corridors;
    ull_t solution;
    vvu_t adjs;
};

Graph::Graph(istream& fi) : solution(0)
{
    fi >> N >> M >> K;
    rooms.reserve(N);
    corridors.reserve(M);
    for (u_t i = 0; i < N; ++i)
    {
        aull3_t room;
        fi >> room[0] >> room[1] >> room[2];
        rooms.push_back(room);
    }
    for (u_t i = 0; i < M; ++i)
    {
        u_t x, y;
        fi >> x >> y;
        if (x > y) { swap(x, y); }
        au2_t corridor{x, y};
        corridors.push_back(corridor);
    }
}

void Graph::solve_naive()
{
    set_adjs();
    setvu_t checked_paths;
    vu_t perm;
    permutation_first(perm, N);
    for (bool more = true; more; more = permutation_next(perm))
    {
        vu_t path;
        ull_t pts = 0;
        bool good = true;
        for (u_t i = 0; good && (i < N) && (pts < K); ++i)
        {
            u_t ri = perm[i];
            const aull3_t& room = rooms[ri];
            good = (i == 0) || ((room[0] <= pts) && (pts <= room[1]));
            path.push_back(ri);
            pts += room[2];
        }
        good = good && (pts == K);
        if (good)
        {
            if (checked_paths.find(path) == checked_paths.end())
            {
                checked_paths.insert(path);
                if (possible(path))
                {
                    ++solution;             
                }
            }
        }
    }
}

void Graph::set_adjs()
{
    adjs = vvu_t(N, vu_t());
    for (const au2_t& corridor: corridors)
    {
         u_t x = corridor[0], y = corridor[1];
         adjs[x].push_back(y);
         adjs[y].push_back(x);
    }
}

bool Graph::possible(const vu_t& path)
{
    bool ok = true;
    vb_t pre_path(N, false);
    pre_path[path[0]] = true;
    for (u_t i = 1, psz = path.size(); ok && (i < psz); ++i)
    {
        pre_path[path[i]] = true;
        u_t rx = path[i - 1];
        u_t ry = path[i];
        // Can we go from rx to ry (using path[0... i]
        vb_t checked(N, false);
        setu_t q;
        q.insert(rx);
        bool ry_found = false;
        while ((!q.empty()) && !ry_found)
        {
            u_t node = *(q.begin());
            q.erase(q.begin());
            for (u_t a: adjs[node])
            {
                ry_found = ry_found || (a == ry);
                if (!ry_found)
                {
                    if (pre_path[a] && !checked[a])
                    {
                        q.insert(a);
                        checked[a] = true;
                    }
                }
            }
        }
        ok = ry_found;
    }
    return ok;
}

void Graph::solve()
{
     solve_naive();
}

void Graph::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
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

int main(int argc, char ** argv)
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Graph::*psolve)() =
        (naive ? &Graph::solve_naive : &Graph::solve);
    if (solve_ver == 1) { psolve = &Graph::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Graph graph(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (graph.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        graph.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
