// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<au2_t> vau2_t;
typedef vector<vau2_t> vvau2_t;

static unsigned dbg_flags;

class Security
{
 public:
    Security(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    int verify() const;
 private:
    void build_adjs();
    void get_corder();
    void bfs();
    // void boundary_annex_black();
    u_t C, D;
    vi_t x;
    vau2_t directs;
    vu_t solution;
    vvau2_t cadjs; // [computer] = (adj, direction-index)
    vu_t corder;
    vector<bool> black;
    // typedef unordered_map<u_t, u_t> bdy_t;
    typedef unordered_set<u_t> bdy_t;
    bdy_t bdy;
};

Security::Security(istream& fi)
{
    fi >> C >> D;
    copy_n(istream_iterator<u_t>(fi), C - 1, back_inserter(x));
    directs.reserve(D);
    for (u_t di = 0; di < D; ++di)
    {
        u_t u, v;
        fi >> u >> v;
        directs.push_back(au2_t{u - 1, v - 1});
    }
}

void Security::solve_naive()
{
    build_adjs();
    solution = vu_t(size_t(D), 0);
    get_corder();
    bfs();
    for (u_t di = 0; di < D; ++di)
    {
        if (solution[di] == 0)
        {
            solution[di] = 1000000;
        }
    }
}

void Security::solve()
{
    solve_naive();
}

void Security::build_adjs()
{
    cadjs = vvau2_t(size_t(C), vau2_t());
    // for (const au2_t& dir: directs)
    for (u_t di = 0; di < D; ++di)
    {
        const au2_t& dir = directs[di];
        cadjs[dir[0]].push_back(au2_t{dir[1], di});
        cadjs[dir[1]].push_back(au2_t{dir[0], di});
    }
    for (vau2_t& adjs: cadjs)
    {
        sort(adjs.begin(), adjs.end());
    }
}

void Security::get_corder()
{
    vau2_t ord_ci, lat_ci;
    for (u_t ci = 0; ci < C - 1; ++ci)
    {
        int xci = x[ci];
        if (xci < 0)
        {
            ord_ci.push_back(au2_t{u_t(-xci), ci + 1});
        }
        else
        {
            lat_ci.push_back(au2_t{u_t(xci), ci + 1});
        }
    }
    sort(ord_ci.begin(), ord_ci.end());
    sort(lat_ci.begin(), lat_ci.end());
    size_t ordi = 0, lati = 0;
    corder.reserve(C);
    corder.push_back(0);
    while ((ordi < ord_ci.size()) && (lati < lat_ci.size()))
    {
        if (ord_ci[ordi][0] == corder.size())
        {
            corder.push_back(ord_ci[ordi++][1]);
        }
        else
        {
            corder.push_back(lat_ci[lati++][1]);
        }
    }
    for ( ; ordi < ord_ci.size(); ++ordi)
    {
        corder.push_back(ord_ci[ordi][1]);
    }
    for ( ; lati < lat_ci.size(); ++lati)
    {
        corder.push_back(lat_ci[lati][1]);
    }
}

void Security::bfs()
{
    black = vector<bool>(size_t(C), false);
    vu_t clatency; clatency.reserve(C);
    clatency.push_back(0);
    black[0] = true;
    for (int xi: x)
    {
        clatency.push_back(xi > 0 ? u_t(xi) : 0);
    }
    // bdy.insert(bdy.end(), bdy_t::value_type(0, 0));
    bdy.insert(bdy.end(), 0);
    for (u_t oci = 1; oci < C; ++oci)
    {
        u_t computer = corder[oci];
        bool found = false;
        for (bdy_t::iterator iter = bdy.begin(), next = iter;
            iter != bdy.end(); iter = next)
        {
            ++next;
            u_t bdy_computer = *iter;
            bool all_black_adjs = true;
            for (const au2_t adj: cadjs[bdy_computer])
            {
                all_black_adjs = all_black_adjs && black[adj[0]];
                if (adj[0] == computer)
                {
                    found = true;
                    if (clatency[computer] == 0)
                    {
                        /// clatency[computer] = clatency[corder[oci - 1]] + 1;
                        clatency[computer] = clatency[corder[oci - 1]];
                        if ((x[computer - 1] > 0) || (oci == 1) ||
                            (x[computer - 1] != x[corder[oci - 1] - 1]))
                        {
                            clatency[computer] += 1;
                        }
                    }
                    u_t delta = clatency[computer] - clatency[bdy_computer];
                    if (delta > 0)
                    {
                        u_t di = adj[1];
                        solution[di] = delta;
                    }
                }
            }
            if (all_black_adjs)
            {
                bdy.erase(iter);
            }
        }
        if (!found)
        {
            cerr << "computer: " << computer << " not found\n";
            exit(1);
        }
        black[computer] = true;
        bdy.insert(bdy.end(), computer);
    }
}

int Security::verify() const
{
    int rc = 0;
    // Dijkstra
    const u_t infinity = 1000000*C;
    vu_t dists(size_t(C), infinity);
    dists[0] = 0;
    priority_queue<au2_t> pq;
    pq.push(au2_t{0, 0});
    while (!pq.empty())
    {
        const au2_t& dist_node = pq.top();
        const u_t dist = dist_node[0];
        const u_t node = dist_node[1];
        pq.pop();
        if (dists[node] == dist)
        {
            for (const au2_t& adi: cadjs[node])
            {
                const u_t a = adi[0];
                const u_t di = adi[1];
                const u_t dist_candid = dist + solution[di];
                if (dists[a] > dist_candid)
                {
                    dists[a] = dist_candid;
                    pq.push(au2_t{dist_candid, a});
                }
            }
        }

    }
    vau2_t dists_idxs; dists_idxs.reserve(C);
    for (u_t i = 0; i < C; ++i)
    {
        dists_idxs.push_back(au2_t{dists[i], i});
    }
    sort(dists_idxs.begin(), dists_idxs.end());
    u_t n_before = 0;
    for (u_t i = 0; (rc == 0) && (i < C - 1); ++i)
    {
        if ((x[i] > 0) && u_t(x[i]) != dists[i + 1])
        {
            cerr << "i="<<i << ", x[i]=" << x[i] << " != D[i+1]=" <<
                dists[i + 1] << '\n';
            rc = 1;
        }
        u_t ci = dists_idxs[i + 1][1];
        if (dists_idxs[i][0] < dists_idxs[i + 1][0])
        {
            n_before = i + 1;
        }
        if (x[ci - 1] < 0)
        {
            u_t n_before_expected = u_t(-x[ci - 1]);
            if (n_before != n_before_expected)
            {
                cerr << "i+1=" << i+1 << ", n_before=" << n_before << 
                    ", n_before_expected=" << n_before_expected << '\n';
                rc = 1;
            }
        }
    }
    return rc;
}

void Security::print_solution(ostream &fo) const
{
    for (u_t y: solution)
    {
        fo << ' ' << y;
    }
}

int main(int argc, char ** argv)
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

    void (Security::*psolve)() =
        (naive ? &Security::solve_naive : &Security::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; (ci < n_cases) && (rc == 0); ci++)
    {
        Security security(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (security.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        security.print_solution(fout);
        fout << "\n";
        fout.flush();
        if (dbg_flags & 0x100)
        {
            rc = security.verify();
        }
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return rc;
}
