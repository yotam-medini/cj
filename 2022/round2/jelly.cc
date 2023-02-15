// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<ll_t, 2> all2_t;
typedef array<u_t, 2> au2_t;
typedef vector<all2_t> vall2_t;
typedef vector<au2_t> vau2_t;
typedef deque<u_t> dqu_t;
typedef vector<dqu_t> vdqu_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

// ========================================================================
// Implementing:
// https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm#Pseudocode
typedef vector<vu_t> vvu_t;

class HopcroftKarp
{
 public:
    HopcroftKarp(const vau2_t& _edges) : caller_edges(_edges) {}
    const vau2_t& solve();
 private:
    typedef unordered_map<u_t, u_t> unord_u2u_t;
    static const u_t infty;
    void init();
    void init_indices(u_t ei, vu_t& indices, unord_u2u_t& xmap);
    bool bfs();
    bool dfs(u_t u);
    bool dfs_recursive(u_t u);
    const vau2_t& caller_edges;
    vau2_t edges;
    vvu_t adjs;
    vau2_t match;
    vu_t u_indices, v_indices;
    u_t usize, vsize;
    vu_t upair, vpair;
    u_t unil, vnil;
    vu_t dist;
};
const u_t HopcroftKarp::infty = u_t(-1);

const vau2_t& HopcroftKarp::solve()
{
    init();
    while (bfs())
    {
        for (u_t u = 0; u < usize; ++u)
        {
            if (upair[u] == vnil)
            {
                (void)(dbg_flags & 0x2 ? dfs_recursive(u) : dfs(u));
            }
        }
    }
    for (u_t u = 0; u < u_indices.size(); ++u)
    {
        const u_t v = upair[u];
        if (v != vnil)
        {
            match.push_back(au2_t{u_indices[u], v_indices[v]});
        }
    }
    return match;
}

void HopcroftKarp::init()
{
    unord_u2u_t umap, vmap;
    init_indices(0, u_indices, umap);
    init_indices(1, v_indices, vmap);
    unil = u_indices.size();
    vnil = v_indices.size();
    usize = u_indices.size(), vsize = v_indices.size(); // 'const' now on
    upair = vu_t(usize, vnil);
    vpair = vu_t(vsize, unil);
    dist = vu_t(usize + 1, infty);
    edges.reserve(caller_edges.size());
    adjs = vvu_t(usize, vu_t());
    for (const au2_t& ce: caller_edges)
    {
        const u_t u = umap[ce[0]], v = vmap[ce[1]];
        edges.push_back(au2_t{u, v});
        adjs[u].push_back(v);
    }
}

void HopcroftKarp::init_indices(u_t ei, vu_t& indices, unord_u2u_t& xmap)
{
    for (const au2_t& ce: caller_edges)
    {
        u_t x = ce[ei];
        unord_u2u_t::iterator iter = xmap.find(x);
        if (iter == xmap.end())
        {
            xmap.insert(iter, unord_u2u_t::value_type(x, indices.size()));
            indices.push_back(x);
        }
    }
}

bool HopcroftKarp::bfs()
{
    deque<u_t> q;
    fill(dist.begin(), dist.end(), infty);
    for (u_t u = 0; u < usize; ++u)
    {
        if (upair[u] == vnil)
        {
            dist[u] = 0;
            q.push_back(u);
        }
    }
    while (!q.empty())
    {
        const u_t u = q.front(); q.pop_front();
        if (dist[u] < dist[unil])
        {
            const vu_t& u_adjs = adjs[u];
            for (u_t v: u_adjs)
            {
                const u_t u_next = vpair[v];
                if (dist[u_next] == infty)
                {
                    dist[u_next] = dist[u] + 1;
                    q.push_back(u_next);
                }
            }
        }
    }
    bool found = (dist[unil] != infty);
    return found;
}

bool HopcroftKarp::dfs(u_t u)
{
    bool found = false;
    vau2_t stack; // (u, i_adj)
    stack.push_back(au2_t{u, 0});
    while (!stack.empty())
    {
        u = stack.back()[0];
        if (u == unil)
        {
            found = true;
            stack.pop_back();
            while (!stack.empty())
            {
                const u_t usb = stack.back()[0];
                const u_t i_adj = stack.back()[1];
                const u_t vsb = adjs[usb][i_adj];
                upair[usb] = vsb;
                vpair[vsb] = usb;
                stack.pop_back();
                found = true;
            }
        }
        else
        {
            u_t& i_adj = stack.back()[1];
            const vu_t& u_adjs = adjs[u];
            if (i_adj < u_adjs.size())
            {
                const u_t v = u_adjs[i_adj];
                const u_t u_next = vpair[v];
                if (dist[u_next] == dist[u] + 1)
                {
                    stack.push_back(au2_t{u_next, 0});
                }
                else
                {
                    ++i_adj;
                }
            }
            else
            {
                dist[u] = infty;
                stack.pop_back();
            }
        }
    }
    return found;
}

bool HopcroftKarp::dfs_recursive(u_t u)
{
    bool found = (u == unil);
    if (!found)
    {
        const vu_t& u_adjs = adjs[u];
        const u_t na = u_adjs.size();
        for (u_t vi = 0; (vi < na) && !found; ++vi)
        {
            const u_t v = u_adjs[vi];
            const u_t u_next = vpair[v];
            if (dist[u_next] == dist[u] + 1)
            {
                found = dfs_recursive(u_next);
                if (found)
                {
                    upair[u] = v;
                    vpair[v] = u;
                }
            }
        }
        if (!found)
        {
            dist[u] = infty;
        }
    }
    return found;
}

void hopcroft_karp(vau2_t& maximal_match, const vau2_t& edges)
{
    HopcroftKarp hk(edges);
    maximal_match = hk.solve();
}

// ========================================================================

class DistIdx
{
 public:
    DistIdx(ull_t _d2=0, u_t _i=0) : d2(_d2), i(_i) {}
    ull_t d2;
    u_t i;
};
bool operator<(const DistIdx& di0, const DistIdx& di1)
{
    bool lt = make_tuple(di0.d2, di0.i) < make_tuple(di1.d2, di1.i);
    return lt;
}
typedef deque<DistIdx> dqdistidx_t;
typedef vector<dqdistidx_t> vdqdistidx_t;

class Jelly
{
 public:
    Jelly(istream& fi);
    Jelly(const vall2_t& _children, const vall2_t& _sweets) :
        N(_children.size()), children(_children), sweets(_sweets),
        possible(true) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    bool is_possible() const { return !solution.empty(); }
 private:
    ll_t dist2(const all2_t& p0, const all2_t& p1) const
    {
        const ll_t dx = p0[0] - p1[0];
        const ll_t dy = p0[1] - p1[1];
        const ll_t d2 = dx*dx + dy*dy;
        return d2;
    }
    void set_d2_child_sweet();
    void solve_by_match(const vau2_t& match);
    u_t nearest_available(
        u_t ci,
        vu_t& next_candidate,
        const vector<bool>& s_picked);
    u_t N;
    vall2_t children;
    vall2_t sweets;
    vau2_t solution;
    bool possible;
    vdqdistidx_t d2_child_sweet;
};

Jelly::Jelly(istream& fi) : possible(true)
{
    fi >> N;
    children.reserve(N);
    while (children.size() < N)
    {
        all2_t xy;
        fi >> xy[0] >> xy[1];
        children.push_back(xy);
    }
    sweets.reserve(N + 1);
    while (sweets.size() < N + 1)
    {
        all2_t xy;
        fi >> xy[0] >> xy[1];
        sweets.push_back(xy);
    }
}

void Jelly::solve_naive()
{
    vu_t perm(N, 0);
    iota(perm.begin(), perm.end(), 0);
    for (bool more = true; solution.empty() && more;
        more = next_permutation(perm.begin(), perm.end()))
    {
        vau2_t candidate;
        possible = true;
        vu_t available(N + 1, 0);
        iota(available.begin(), available.end(), 0);
        for (u_t pi = 0; possible && (pi < N); ++pi)
        {
            const u_t ci = perm[pi];
            ll_t d2_min = numeric_limits<ll_t>::max();
            u_t ai_min = N + 1, si_min = ai_min;
            for (u_t ai = 0; ai < available.size(); ++ai)
            {
                const u_t si = available[ai];
                const ll_t d2 = dist2(children[ci], sweets[si]);
                if ((d2_min > d2) || ((d2_min == d2) && (si_min == 0)))
                {
                    d2_min = d2;
                    ai_min = ai;
                    si_min = si;
                }
            }
            possible = (si_min > 0);
            candidate.push_back(au2_t{ci, si_min});
            available[ai_min] = available.back();
            available.pop_back();
        }
        if (possible && (candidate.size() == N))
        {
            solution = candidate;
        }
    }
}

void Jelly::solve()
{
    solution.clear();
    set_d2_child_sweet();
    if (possible)
    {
        vau2_t edges;
        for (u_t ci = 0; ci < N; ++ci)
        {
            for (const DistIdx& di: d2_child_sweet[ci])
            {
                edges.push_back(au2_t{ci, di.i});
            }
        }
        vau2_t match;
        hopcroft_karp(match, edges);
        u_t n_match = match.size();
        possible = (n_match == N);
        if (possible)
        {
            sort(match.begin(), match.end());
            solve_by_match(match);
        }
    }
}

void Jelly::set_d2_child_sweet()
{
    possible = true;
    d2_child_sweet.reserve(N);
    for (u_t ci = 0; possible && (ci < N); ++ci)
    {
        const all2_t& child = children[ci];
        const ll_t d2_to_jelly = dist2(child, sweets[0]);
        dqdistidx_t d2_sweets;
        for (u_t si = 1; si < N + 1; ++si)
        {
            const all2_t& sweet = sweets[si];
            ll_t d2 = dist2(child, sweet);
            if (d2 <= d2_to_jelly)
            {
                d2_sweets.push_back(DistIdx(d2, si));
            }
        }
        sort(d2_sweets.begin(), d2_sweets.end());
        possible = !d2_sweets.empty();
        d2_child_sweet.push_back(d2_sweets);
        if (dbg_flags & 0x2) { cerr << "ci="<<ci << ", sweets:";
            for (const DistIdx& di: d2_sweets) { cerr << ' ' << di.i; }
            cerr << '\n'; }
    }
}

u_t Jelly::nearest_available(
    u_t ci,
    vu_t& next_candidate,
    const vector<bool>& s_picked)
{
    const dqdistidx_t& csweets = d2_child_sweet[ci];
    u_t& nsi = next_candidate[ci];
    for ( ; s_picked[csweets[nsi].i]; ++nsi) {}
    const u_t si = csweets[nsi].i;
    return si;
}

void Jelly::solve_by_match(const vau2_t& match)
{
    vu_t match_sweet_to_child(N + 1, 0);
    for (const au2_t& m: match)
    {
         match_sweet_to_child[m[1]] = m[0];
    }
    vector<bool> c_picked(N, false);
    vector<bool> s_picked(N + 1, false);
    vector<bool> scanned(N, false); // childern for loop
    vu_t next_candidate(N, 0);
    setu_t children_pending;
    for (u_t ci = 0; ci < N; ++ci)
    {
         children_pending.insert(children_pending.end(), ci);
    }
    while (!children_pending.empty())
    {
        // trivial select match == nearset
        for (setu_t::iterator iter = children_pending.begin(), iter1 = iter;
            iter != children_pending.end(); iter = iter1)
        {
            ++iter1;
            const u_t ci = *iter;
            const u_t si = nearest_available(ci, next_candidate, s_picked);
            if (match[ci][1] == si) // assuming match[ci][0] == ci
            {
                solution.push_back(au2_t{ci, si});
                children_pending.erase(iter);
                c_picked[ci] = s_picked[si] = true;
            }
        }
        // find loop
        const u_t ci0 = (children_pending.empty()
            ? N : *children_pending.begin());
        if (ci0 != N)
        {
            vu_t scanned_indices;
            u_t ci = ci0;
            while (!scanned[ci])
            {
                scanned[ci] = true;
                scanned_indices.push_back(ci);
                const u_t si = nearest_available(ci, next_candidate, s_picked);
                ci = match_sweet_to_child[si];
            }
            if (dbg_flags & 0x1) { cerr << "Loop @ ci=" << ci << '\n'; }
            for (u_t i: scanned_indices) { scanned[i] = false; }
            while (!c_picked[ci])
            {
                const u_t si = nearest_available(ci, next_candidate, s_picked);
                solution.push_back(au2_t{ci, si});
                children_pending.erase(ci);
                c_picked[ci] = s_picked[si] = true;
                ci = match_sweet_to_child[si];
            }
        }
    }
}

void Jelly::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << ' ' << "IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << "POSSIBLE";
        for (const au2_t& ab: solution)
        {
            fo << '\n' << ab[0] + 1 << ' ' << ab[1] + 1;
        }
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

    void (Jelly::*psolve)() =
        (naive ? &Jelly::solve_naive : &Jelly::solve);
    if (solve_ver == 1) { psolve = &Jelly::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Jelly jelly(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (jelly.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        jelly.print_solution(fout);
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

static ll_t rand_range(ll_t nmin, ll_t nmax)
{
    ll_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(
    const char* fn, const vall2_t& children, const vall2_t& sweets)
{
    const u_t N = children.size();
    ofstream f(fn);
    f << "1\n" << N << '\n';
    for (const all2_t& xy: children) { f << xy[0] << ' ' << xy[1] << '\n'; }
    for (const all2_t& xy: sweets) { f << xy[0] << ' ' << xy[1] << '\n'; }
    f.close();
}

static int test_case(
    bool& possible,
    const vall2_t& children,
    const vall2_t& sweets)
{
    int rc = 0;
    possible = false;
    bool possible_naive = false;
    const u_t N = children.size();
    bool small = (N <= 10);
    if (dbg_flags & 0x100) { save_case("jelly-curr.in", children, sweets); }
    if (small)
    {
        Jelly p{children, sweets};
        p.solve_naive();
        possible_naive = p.is_possible();
    }
    {
        Jelly p{children, sweets};
        p.solve();
        possible = p.is_possible();
    }
    if (small && (possible != possible_naive))
    {
        rc = 1;
        cerr << "Failed: possible = " << possible << " != " <<
            possible_naive << " = possible_naive\n";
        save_case("jelly-fail.in", children, sweets);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        (possible ? "POSSIBLE" : "IMPOSSIBLE") << '\n'; }
    return rc;
}

static void random_points(
    vall2_t& points,
    u_t N,
    ll_t Xmin,
    ll_t Xmax,
    ll_t Ymin,
    ll_t Ymax)
{
    set<all2_t> set_points;
    points.size();
    while (points.size() < N)
    {
        all2_t xy;
        bool found = false;
        while (!found)
        {
            xy[0] = rand_range(Xmin, Xmax);
            xy[1] = rand_range(Ymin, Ymax);
            found = (set_points.find(xy) == set_points.end());
        }
        points.push_back(xy);
    }
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
    const ll_t Xmin = strtol(argv[ai++], nullptr, 0);
    const ll_t Xmax = strtol(argv[ai++], nullptr, 0);
    const ll_t Ymin = strtol(argv[ai++], nullptr, 0);
    const ll_t Ymax = strtol(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Xmin=" << Xmin << ", Xmax=" << Xmax <<
        ", Ymin=" << Ymin << ", Ymax=" << Ymax <<
        '\n';
    for (u_t ti = 0, n_impossible = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests <<
            " #(impossible)=" << n_impossible << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        vall2_t children, sweets;
        random_points(children, N, Xmin, Xmax, Ymin, Ymax);
        random_points(sweets, N + 1, Xmin, Xmax, Ymin, Ymax);
        bool possible = false;
        rc = test_case(possible, children, sweets);
        if (!possible) { ++n_impossible; }
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
