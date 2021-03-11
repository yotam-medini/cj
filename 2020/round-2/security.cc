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
    int verify(vu_t& dists) const;
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
        if (ord_ci[ordi][0] <= corder.size())
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

int Security::verify(vu_t& dists) const
{
    int rc = 0;
    // Dijkstra
    const u_t infinity = 1000000*C;
    dists = vu_t(size_t(C), infinity);
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

static int real_main(int argc, char** argv)
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
            vu_t dists;
            rc = security.verify(dists);
        }
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return rc;
}

class Edge
{
 public:
    Edge(u_t _v0=0, u_t _v1=0, u_t _d=0) : v0(_v0), v1(_v1), d(_d) {}
    u_t v0, v1, d;
};
typedef vector<Edge> vedges_t;
typedef vector<vedges_t> graph_t;

class Test
{
 public:
    Test(int argc, char** argv) :
       rc(0), seed(1)
    {
        int ai = 0;
        n_tests = stod(argv[ai++]);
        cmin = stod(argv[ai++]);
        cmax = stod(argv[ai++]);
        degmin = stod(argv[ai++]);
        degmax = stod(argv[ai++]);
        dmin = stod(argv[ai++]);
        dmax = stod(argv[ai++]);
        if (ai < argc)
        {
            seed = stod(argv[ai++]);
        }
        if ((cmin > cmax) || (degmin > degmax) || (dmin > dmax))
        {
            cerr << "Bad min>max parameters\n";
            rc = 1;
        }
    }
    int run();
    void compute_distances();
 private:
    typedef vector<bool> vb_t;
    void graph_build();
    bool check_add_edge(u_t v0, u_t v1)
    {
        bool add = !vv[v0][v1];
        if (add)
        {
            vv[v0][v1] = vv[v1][v0] = true;
            u_t d = dmin + rand() % (dmax + 1 - dmin);
            graph[v0].push_back(Edge(v0, v1, d));
            graph[v1].push_back(Edge(v1, v0, d));
        }
        return add;
    }
    void setx();
    void write_fin(const char* fn) const;
    int consistent(const Security& security) const;
    int rc;
    u_t n_tests;
    u_t cmin;
    u_t cmax;
    u_t degmin;
    u_t degmax;
    u_t dmin;
    u_t dmax;
    u_t seed;
    graph_t graph;
    vector<vb_t> vv;
    vu_t dists;
    vi_t x;
};

int Test::run()
{
    const char* fn_in = "security-auto.in";
    for (u_t ti = 0; (ti < n_tests) && (rc == 0); ++ti)
    {
        cerr << ti << '/' << n_tests << " tests, seed=" << seed << '\n';
        srand(seed);
        graph_build();
        compute_distances();
        setx();
        write_fin(fn_in);
        u_t dum;
        if (graph.size() < 20)
        {
            ifstream f(fn_in);
            f >> dum;
            Security p{f};
            p.solve_naive();
            rc = consistent(p);
        }
        if (rc == 0)
        {
            ifstream f(fn_in);
            f >> dum;
            Security p{f};
            p.solve();
            vu_t naive_dists;
            rc = consistent(p);
        }
        seed = rand();
    }
    return rc;
}

void Test::graph_build()
{
    const u_t c = cmin + (rand() % (cmax + 1 - cmin));
    const u_t deg_maxc = min(c, degmax);
    const u_t deg_minc = min(degmin, deg_maxc);
    graph = graph_t(size_t(c), vedges_t());
    vv = vector<vb_t>(size_t(c), vb_t(size_t(c), false));
    for (u_t i = 0; i < c; vv[i][i] = true, ++i) {}
    vu_t pending, generation, covered;
    covered.push_back(0);
    generation.push_back(0);
    for (u_t ci = 1; ci < c; ++ci)
    {
        pending.push_back(ci);
    }
    while (!pending.empty())
    {
        vu_t next_generation;
        for (u_t g: generation)
        {
            u_t na = deg_maxc + rand() % (deg_maxc + 1 - deg_minc);
            while (na && !pending.empty())
            {
                bool found = false;
                while (!found)
                {
                    bool from_pending = (rand() % 12) != 0;
                    if (from_pending)
                    {
                        u_t pi = rand() % pending.size();
                        u_t a = pending[pi];
                        found = check_add_edge(g, a);
                        if (found)
                        {
                            pending[pi] = pending.back();
                            pending.pop_back();
                            next_generation.push_back(a);
                        }
                    }
                    else
                    {
                        u_t a = covered[rand() % covered.size()];
                        found = check_add_edge(g, a);
                    }
                }
                --na;
            }
        }
        swap(generation, next_generation);
        covered.insert(covered.end(), generation.begin(), generation.end());
    }
}

void Test::compute_distances()
{
    const u_t c = graph.size();
    dists = vu_t(size_t(c), u_t(-1));
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
            for (const Edge& edge: graph[node])
            {
                const u_t candid_dist = dist + edge.d;
                if (dists[edge.v1] > candid_dist)
                {
                    dists[edge.v1] = candid_dist;
                    pq.push(au2_t{candid_dist, edge.v1});
                }
            }
        }
    }
}

void Test::setx()
{
    vau2_t dist_idx;
    for (u_t i = 0, e = graph.size(); i < e; ++i)
    {
        dist_idx.push_back(au2_t{dists[i], i});
    }
    sort(dist_idx.begin(), dist_idx.end());
    vi_t before(graph.size(), 0);
    for (int i = 1, e = graph.size(); i < e; ++i)
    {
        if (dist_idx[i][0] > dist_idx[i - 1][0])
        {
            before[dist_idx[i][1]] = -i;
        }
        else
        {
            before[dist_idx[i][1]] = before[dist_idx[i - 1][1]];
        }
    }
    x.clear();
    for (size_t i = 1, e = graph.size(); i < e; ++i)
    {
        if (rand() % 2 == 0)
        {
            x.push_back(dists[i]);
        }
        else
        {
            x.push_back(before[i]);
        }
    }
}

void Test::write_fin(const char* fn) const
{
    ofstream f(fn);
    const u_t c = graph.size();
    u_t d = 0;
    for (const vedges_t& edges: graph)
    {
        d += edges.size();
    }
    d /= 2; // we counted twice each edge
    f << "1\n" << c << ' ' << d << '\n';
    const char* sep = "";
    for (int xdo: x)
    {
        f << sep << xdo; sep = " ";
    }
    f << '\n';
    for (const vedges_t& edges: graph)
    {
        for (const Edge e: edges)
        {
            if (e.v0 < e.v1)
            {
                f << e.v0 + 1 << ' ' << e.v1 + 1 << '\n';
            }
        }
    }
    f.close();
}

int Test::consistent(const Security& security) const
{
    vu_t sdists;
    int crc = security.verify(sdists);
    if (crc == 0)
    {
        const u_t c = sdists.size();
        vau2_t dist_idx;
        for (u_t i = 0; i < c; ++i)
        {
            dist_idx.push_back(au2_t{sdists[i], i});
        }
        sort(dist_idx.begin(), dist_idx.end());
        vi_t before(size_t(c), 0);
        for (int i = 1; i < int(c); ++i)
        {
            if (dist_idx[i][0] > dist_idx[i - 1][0])
            {
                before[dist_idx[i][1]] = -i;
            }
            else
            {
                before[dist_idx[i][1]] = before[dist_idx[i - 1][1]];
            }
        }
        for (u_t i = 1; (crc == 0) && (i < c); ++i)
        {
            const int xi = x[i - 1];
            if (xi > 0)
            {
                if (u_t(xi) != sdists[i])
                {
                    cerr << "Inconsistent: i=" << i << ", xi="<<xi <<
                        ", sdists[i]="<< sdists[i] << '\n';
                    crc = 1;
                }
            }
            else // < 0
            {
                if (before[i] != xi)
                {
                    cerr << "Inconsistent: i=" << i <<
                        ", before[o]=" << before[i] << ", xi" << xi <<  '\n';
                    crc = 1;
                }
            }
        }
    }
    return crc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (argc > 1 && string(argv[1]) == string("-test"))
    {
        rc = Test(argc - 2, argv + 2).run();
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
