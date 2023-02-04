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

// ========================================================================
#include <map>
#include <queue>
#include <vector>

typedef vector<u_t> vu_t;
typedef map<u_t, vu_t> u2vu_t;
typedef map<u_t, u_t> u2u_t;
typedef array<u_t, 2> au2_t;
typedef map<au2_t, u_t> au2_2u_t;

class EdgeFlow
{
 public:
    EdgeFlow(u_t c=0, u_t r=0, u_t f=0, bool o=true): 
        capacity(c), residual_capcity(r), flow(f), original(o) {}
    u_t capacity;
    u_t residual_capcity;
    u_t flow;
    bool original;
};
typedef map<au2_t, EdgeFlow> au2_2ef_t;

class GFK
{
 public:
    GFK(const au2_2u_t& ucapacity, u_t _src, u_t _sink) :
        source(_src), sink(_sink)
    {
        build_graph(ucapacity);
        init_edges(ucapacity);
    }
    bool ford_fulkerson(u_t &total_flow, au2_2u_t& result_flow);
 private:
    void build_graph(const au2_2u_t& ucapacit);
    void init_edges(const au2_2u_t& ucapacit);
    bool get_augment_path(u_t& flow, vu_t& path) const;
    void augment_flow(u_t flow, const vu_t& path);
    void add_edge(const au2_t& edge);
    // const au2_2u_t& capacity;
    u_t source;
    u_t sink;
    u2vu_t graph;
    au2_2ef_t eflows;
};

bool GFK::ford_fulkerson(u_t &total_flow, au2_2u_t& result_flow)
{
    bool ok = true;
    u_t flow;
    vu_t path;
    while (get_augment_path(flow, path))
    {
        augment_flow(flow, path);
    }
    total_flow = 0;
    result_flow.clear();
    for (const au2_2ef_t::value_type& kv: eflows)
    {
        const EdgeFlow& ef = kv.second;
        if (ef.flow > 0)
        {
            const au2_t& edge = kv.first;
            if (edge[0] == source)
            {
                total_flow += ef.flow;
            }
            au2_2u_t::value_type v(edge, ef.flow);
            result_flow.insert(result_flow.end(), v);
        }
    }
    return ok;
}

void GFK::build_graph(const au2_2u_t& ucapacity)
{
    for (const au2_2u_t::value_type& kv: ucapacity)
    {
        const au2_t& edge = kv.first;
        add_edge(edge);
    }
}

void GFK::add_edge(const au2_t& edge)
{
    u_t u = edge[0];
    auto er = graph.equal_range(u);
    u2vu_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        vu_t adjs;
        u2vu_t::value_type gkv(u, adjs);
        iter = graph.insert(iter, gkv);
    }
    vu_t& adjs = iter->second;
    adjs.push_back(edge[1]);
}

void GFK::init_edges(const au2_2u_t& ucapacity)
{
    for (const au2_2u_t::value_type& kv: ucapacity)
    {
        const au2_t& edge = kv.first;
        u_t capacity = kv.second;
        EdgeFlow ef(capacity, capacity, 0);
        eflows.insert(eflows.end(), au2_2ef_t::value_type(edge, ef));
    }
}

bool GFK::get_augment_path(u_t& flow, vu_t& path) const
{
    // BFS
    bool found = false;
    u2u_t parent;
    queue<u_t> q;
    q.push(source);
    while (!(q.empty() || found))
    {
        u_t node = q.front();
        q.pop();
        u2vu_t::const_iterator i_adjs = graph.find(node);
        static const vu_t empty_adjs;
        const vu_t& adjs = i_adjs == graph.end() ? empty_adjs : i_adjs->second;
        for (vu_t::const_iterator ai = adjs.begin(), ae = adjs.end();
            (ai != ae) && !found; ++ai)
        {
            const u_t a = *ai;
            if (parent.find(a) == parent.end())
            {
                const au2_t edge{node, a};
                const EdgeFlow& ef = eflows.find(edge)->second;
                if (ef.residual_capcity > 0)
                {
                    found = (a == sink);
                    parent.insert(parent.end(), u2u_t::value_type(a, node));
                    q.push(a);
                }
            }
        }
    }
    path.clear();
    if (found)
    {
        flow = u_t(-1); // infinite
        vu_t revpath;
        u_t v = sink;
        revpath.push_back(v);
        while (revpath.back() != source)
        {
            u_t p = parent.find(v)->second;
            au2_t edge{p, v};
            u_t pu_residual = eflows.find(edge)->second.residual_capcity;
            if (flow > pu_residual)
            {
                flow = pu_residual;
            }
            v = p;
            revpath.push_back(v);
        }
        path.insert(path.end(), revpath.rbegin(), revpath.rend());
    }
    return found;
}

void GFK::augment_flow(u_t flow, const vu_t& path)
{
    for (u_t i = 0, i1 = 1, e = path.size(); i1 != e; i = i1++)
    {
        const au2_t edge{path[i], path[i1]};
        EdgeFlow& ef = eflows.find(edge)->second;
        ef.residual_capcity -= flow;

        const au2_t redge{path[i1], path[i]};
        auto er = eflows.equal_range(redge);
        au2_2ef_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            EdgeFlow ef_res(0, 0, 0, false);
            iter = eflows.insert(iter, au2_2ef_t::value_type(redge, ef_res));
            add_edge(redge);
        }
        EdgeFlow& rev_ef = iter->second;
        rev_ef.residual_capcity += flow;

        if (ef.original)
        {
            ef.flow += flow;
        }
        else
        {
            rev_ef.flow -= flow;
        }
    }
}

bool max_flow(
    u_t& total_flow,
    au2_2u_t& result_flow,
    const au2_2u_t& flow,
    u_t source,
    u_t sink)
{
    GFK gfk(flow, source, sink);
    bool ret = gfk.ford_fulkerson(total_flow, result_flow);
    return ret;
}

typedef unsigned u_t;
typedef set<u_t> setu_t;
typedef map<u_t, unsigned> u2u_t;

unsigned maps_set(u2u_t &c2g, u2u_t &g2c, const setu_t &s, unsigned gi)
{
    for (unsigned i: s)
    {
        c2g.insert(c2g.end(), u2u_t::value_type(i, gi));
        g2c.insert(g2c.end(), u2u_t::value_type(gi, i));
        ++gi;
    }
    return gi;
}

int bipartitee_max_match(vau2_t &match, const vau2_t &edges)
{
    setu_t lset, rset;
    for (const auto &e: edges)
    {
       lset.insert(e[0]);
       rset.insert(e[1]);
    }
    u2u_t l2g, r2g, g2l, g2r;
    u_t eol = maps_set(l2g, g2l, lset, 0);
    u_t eor = maps_set(r2g, g2r, rset, eol);
    u_t source = eor;
    u_t sink = source + 1;
    au2_2u_t flow, result_flow;
    for (u_t i: lset)
    {
        au2_t e{source, l2g[i]};
        flow.insert(au2_2u_t::value_type(e, 1));
    }
    for (const auto &e: edges)
    {
        au2_t e1{l2g[e[0]], r2g[e[1]]};
        flow.insert(au2_2u_t::value_type(e1, 1));
    }
    for (u_t i: rset)
    {
        au2_t e{r2g[i], sink};
        flow.insert(au2_2u_t::value_type(e, 1));
    }
    u_t total_flow = 0;
    bool ok = flow.empty() ||
        max_flow(total_flow, result_flow, flow, source, sink);
    if (ok)
    {
        for (const auto v: result_flow)
        {
            const au2_t &ge = v.first;
            u_t capacity = v.second;
            if ((ge[0] < eol) && (eol <= ge[1]) && (ge[1] < eor) &&
                (capacity == 1))
            {
                au2_t e{g2l[ge[0]], g2r[ge[1]]};
                match.push_back(e);
            }
        }
    }
    int ret = ok ? total_flow : -1;
    return ret;
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

static unsigned dbg_flags;

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
        int n_match = bipartitee_max_match(match, edges);
        possible = (n_match == int(N));
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
