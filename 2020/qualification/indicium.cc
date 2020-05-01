// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <cstdlib>
#include <string>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;

typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;
typedef vector<vsetu_t> vvsetu_t;

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

static vvvu_t g_perms;
void init_perms()
{
    g_perms.clear();
    g_perms.push_back(vvu_t());
    for (u_t n = 1; n <= 6; ++n)
    {
        vvu_t perms;
        vu_t p;
        permutation_first(p, n);
        perms.push_back(p);
        while (permutation_next(p))
        {
            perms.push_back(p);
        }
        sort(perms.begin(), perms.end()); // not needed ?
        g_perms.push_back(perms);
    }
}

#include <array>
#include <map>
#include <queue>
typedef array<unsigned, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef map<au2_t, u_t> au2_2u_t;

typedef vector<u_t> vu_t;
typedef map<u_t, vu_t> u2vu_t;
typedef map<u_t, u_t> u2u_t;

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

class QR
{
 public:
    QR(u_t n, u_t k) : q(k / n), r(k % n), q0(q), q1(q)
    {
        if (r > 0)
        {
            u_t a0 = r/2;
            u_t a1 = r - a0;
            q0 = q + a0;
            q1 = q + a1;
            if (q0 == q)
            {
                --q0;
                ++q1;
            }
        }

    }
    u_t q, r, q0, q1;
};

class Indicium
{
 public:
    Indicium(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool complete_mat();
    bool next_mat();
    bool match_row(const vu_t& p) const;
    void simple_diag(u_t q);
    void via_bipartite(const QR& qr);
    const vu_t& set_row(vu_t& row, const vu_t& row0, u_t shift) const;
    void fill_shift_rows(const vu_t& row0, u_t flip);
    void rev_mat();
    u_t N, K;
    bool possible;
    vvu_t mat;
};

Indicium::Indicium(istream& fi) :  possible(true)
{
    fi >> N >> K;
}

void Indicium::solve_naive()
{
    if (g_perms.empty())
    {
        init_perms();
    }
    const vvu_t& perms = g_perms[N];
    const u_t np = perms.size();
    possible = false;
    for (u_t pi0 = 0; (pi0 < np) && !possible; ++pi0)
    {
        mat.clear();
        mat.push_back(perms[pi0]);
        bool done = !complete_mat();
        while (!possible && !done)
        {
            u_t trace = 0;
            for (u_t i = 0; i < N; ++i)
            {
                trace += mat[i][i];
            }
            u_t ktrace = trace + N; // since actually we should count from 1
            possible = ktrace == K;
            if (!possible)
            {
                done = !next_mat();
            }
        }
    }
}

bool Indicium::complete_mat()
{
    const vvu_t& perms = g_perms[N];
    const u_t np = perms.size();
    bool completed = false;
    for (u_t pi = 0; (pi < np) && !completed; ++pi)
    {
        const vu_t& p = perms[pi];
        bool match = match_row(p);
        if (match)
        {
            mat.push_back(p);
            completed = (mat.size() == N);
            if (!completed)
            {
                completed = complete_mat();
                if (!completed)
                {
                    mat.pop_back();
                }
            }
        }
    }
    completed = (mat.size() == N);
    return completed;
}

bool Indicium::next_mat()
{
    bool next = false;
    // const vvu_t& perms = g_perms[N]; (better to save pi ?)
    // const u_t np = perms.size();
    mat.pop_back();
    while ((mat.size() > 0) && !next)
    {
        vu_t p = mat.back();
        mat.pop_back();
        bool found_next_row = false;
        while ((!found_next_row) && permutation_next(p))
        {
            found_next_row = match_row(p);
        }
        if (found_next_row)
        {
            mat.push_back(p);
            next = complete_mat();
        }
    }
    return next;
}

bool Indicium::match_row(const vu_t& p) const
{
    u_t sz = mat.size();
    u_t sub_trace = p[sz];
    for (u_t i = 0; i < mat.size(); ++i)
    {
        sub_trace += mat[i][i];
    }
    u_t k = K - N;
    u_t add_potential = (N - (sz + 1))*(N - 1);
    bool match = (sub_trace <= k) && (k <= sub_trace + add_potential);
    for (u_t r = 0, nr = mat.size(); match && (r < nr); ++r)
    {
        const vu_t& row = mat[r];
        for (u_t c = 0; match && (c < N); ++c)
        {
            match = (p[c] != row[c]);
        }
    }
    return match;
}

void Indicium::solve()
{
    const u_t NNm1 = N*(N-1);
    const u_t k = K - N;
    possible = (k != 1) && (k + 1 != NNm1);
    possible = possible && ((N != 3) || ((k != 2) && (k != 4)));
    //possible = (k != 1) && (k + 1 != N) &&
    //    (k + N - 1 != NNm1) && (k + 1 != NNm1);
    if (possible)
    {
        mat.reserve(N);
        const bool rev = 2*k > NNm1;
        u_t krev = (rev ? NNm1 - k : k);
        const QR qr(N, krev);
        if (qr.q1 == qr.q)
        {
            simple_diag(qr.q);
        }
        else
        {
            via_bipartite(qr);
        }
        if (rev)
        {
            rev_mat();
        }
    }
}

void Indicium::simple_diag(u_t q)
{
    vu_t row0;
    row0.reserve(N);
    for (u_t i = 0; i != N; ++i)
    {
        row0.push_back((q + i) % N);
    }
    for (u_t ri = 0; ri != N; ++ri)
    {
        vu_t row;
        mat.push_back(set_row(row, row0, ri));
    }
}

void Indicium::via_bipartite(const QR& qr)
{
    const u_t a = qr.q, b = qr.q0, c = qr.q1;
    vu_t row;
    row.reserve(N);

    // row 0
    row.push_back(b);
    row.push_back(a);
    for (u_t i = 0; i != N; ++i) 
    { 
        if ((i != b) && (i != a))
        {
            row.push_back(i);
        }
    }
    const setu_t aall(row.begin(), row.end());
    vsetu_t avail(size_t(N), aall);
    mat.push_back(row);
    for (u_t i = 0; i != N; ++i)
    {
        avail[i].erase(row[i]);
        if (i > 1)
        {
            avail[i].erase(a);
        }
    }

    for (u_t r = 1; r != N; ++r)
    {
        fill(row.begin(), row.end(), N);
        vu_t tmp_erase_count;
        if (r == 1)
        {
            row[0] = a;
            row[1] = c;
            avail[0].erase(a);
            avail[1].erase(c);
            for (u_t ci = 2; ci != N; ++ci)
            {
                u_t ne = avail[ci].erase(c);
                tmp_erase_count.push_back(ne);
            }
        }
        else
        {
            row[r] = a;
        }
        vau2_t match;
        vau2_t edges;
        for (u_t ci = (r == 1 ? 2 : 0); ci != N; ++ci)
        {
            if (ci != r)
            {
                for (u_t av: avail[ci])
                {
                    edges.push_back(au2_t{ci, av});
                }
            }
        }
        int match_expect = N - (r == 1 ? 2 : 1);
        int n_matched = bipartitee_max_match(match, edges);
        if (n_matched != match_expect)
        {
            cerr << "Failed\n"; exit(1);
        }
        if (r == 1)
        {
            for (u_t ci = 2; ci != N; ++ci)
            {
                if (tmp_erase_count[ci - 2] > 0)
                {
                    avail[ci].insert(c);
                }
            }
        }
        for (const au2_t& lr: match)
        {
            row[lr[0]] = lr[1];
        }
        for (u_t i = 0; i != N; ++i)
        {
            avail[i].erase(row[i]);
        }
        mat.push_back(row);
    }
}

const vu_t& Indicium::set_row(vu_t& row, const vu_t& row0, u_t shift) const
{
    const u_t n = row0.size();
    row.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        row.push_back(row0[(n - shift + i) % n]);
    }
    return row;
}

void Indicium::rev_mat()
{
    const u_t n = mat.size();
    const u_t nm1 = n - 1;
    for (vu_t& row: mat)
    {
        for (u_t ci = 0; ci != n; ++ci)
        {
            row[ci] = nm1 - row[ci];
        }
    }
}

void Indicium::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << " POSSIBLE";
        for (const vu_t& row: mat)
        {
            fo << '\n';
            const char* sep = "";
            for (u_t x: row)
            {
                fo << sep << (x + 1);
                sep = " ";
            }
        }
    }
    else
    {
        fo << " IMPOSSIBLE";
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

    void (Indicium::*psolve)() =
        (naive ? &Indicium::solve_naive : &Indicium::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Indicium indicium(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (indicium.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        indicium.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
