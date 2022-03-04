// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef array<int, 2> ai2_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<ai2_t> vai2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

template <typename T, std::size_t N >
ostream& operator<<(ostream& os, const array<T, N>& a)
{
    os << '(';
    const char *sep = "";
    for (const T& x: a)
    {
        os << sep << x; sep = ", ";
    }
    os << ')';
    return os;
}

template <typename V>
string v2str(const V& v)
{
    ostringstream os;
    os << "[";
    const char *sep = "";
    for (const auto& x: v)
    {
        os << sep << x; sep = " ";
    }
    os << "]";
    return os.str();
}

ostream& operator<<(ostream& os, const au2_t& a)
{
    return os << '{' << a[0] << ", " << a[1] << '}';
}

void minby(u_t& v, u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

static const ull_t INF_DIST = 100000ull * 1000000ull; // 10^5 x 10^6

static void min_by(ull_t& a, ull_t x)
{
    if (a > x)
    {
        a = x;
    }
}

void vsub_ifgt(u_t& v, u_t d)
{
    if (v > d)
    {
        v -= d;
    }
}

class Node
{
 public:
    Node(
        u_t _imatch=0,
        ull_t _l=0, 
        ull_t _r=0, 
        ull_t _p=0,
        const au2_t& _parents=au2_t{0, 0}) : 
        imatch(_imatch),
        lr{_l, _r}, 
        p(_p),
        parents(_parents)
    {}
    void sub_shift(u_t d)
    {
        imatch -= d;
        parents[0] -= d;
        parents[1] -= d;
    }
    void ifgt_sub_shift(u_t t, u_t d)
    {
        vifgt_sub_shift(imatch, t, d);
        vifgt_sub_shift(parents[0], t, d);
        vifgt_sub_shift(parents[1], t, d);
    }
    ull_t l() const { return lr[0]; }
    ull_t r() const { return lr[1]; }
    u_t imatch; // mathing bracket within sub-problem
    aull2_t lr;
    ull_t p;
    au2_t parents;
 private:
    void vifgt_sub_shift(u_t& v, u_t t, u_t d)
    {
        if (v > t)
        {
            v -= d;
        }
    }
};
typedef vector<Node> vnode_t;

class Query
{
 public:
    Query(u_t _s=0, u_t _e=0, u_t _qdi=0) : s(_s), e(_e), qdi(_qdi) {}
    void sub(u_t t)
    {
        s -= t;
        e -= t;
    }
    void sub_ifgt(u_t t)
    {
        vsub_ifgt(s, t);
        vsub_ifgt(e, t);
    }
    u_t s, e;
    u_t qdi; // index ot original query index
};
typedef vector<Query> vquery_t;

class SubProblem
{
 public:
    SubProblem(vull_t& _q_dists) : q_dists(_q_dists) {}
    void solve();
    vnode_t nodes;
    vquery_t queries;    
 private:
    enum {SB_B, SB_C, SB_E, SB_F, SB_N}; // special brackets
    void minimize_lrp();
    void solve_small();
    void dijkstra_fromto(vull_t& dists, u_t inode, bool from_mode) const;
    void split_queries();
    void split();
    array<u_t, SB_N> sbis;
    vull_t sb_dists_from[SB_N];
    vull_t sb_dists_to[SB_N];
    vull_t& q_dists;
    vquery_t sub_queries[SB_N];
};

void SubProblem::solve()
{
    minimize_lrp();
    const u_t sz = nodes.size();
    if (sz <= 4)
    {
        solve_small();
    }
    else
    {
        const u_t half = sz / 2;
        // see problem analysis: 
        //  codingcompetitions.withgoogle.com/codejam/round/
        //      000000000019ffb9/000000000033893b#analysis
        sbis[SB_B] = half;
        sbis[SB_F] = nodes[sbis[SB_B]].imatch;
        if (sbis[SB_F] < sbis[SB_B])
        {
            swap(sbis[SB_B], sbis[SB_F]);
        }
        while (sbis[SB_F] + 1 - sbis[SB_B] < half)
        {
            sbis[SB_C] = sbis[SB_B];
            sbis[SB_E] = sbis[SB_F];
            const au2_t& parents = nodes[sbis[SB_B]].parents;
            sbis[SB_B] = parents[0];
            sbis[SB_F] = parents[1];
        }
        for (u_t sbi = 0; sbi < SB_N; ++sbi)
        {
            dijkstra_fromto(sb_dists_from[sbi], sbis[sbi], true);            
            dijkstra_fromto(sb_dists_to[sbi], sbis[sbi], false);            
        }
        split_queries();
        split();
    }
}

void SubProblem::minimize_lrp()
{
    for (u_t i = 0, n = nodes.size(); i < n; ++i)
    {
        Node& node = nodes[i];  
        if ((i > 0) && (node.imatch == i - 1))
        {
            node.lr[0] = node.p = min(node.lr[0], node.p);
        }
        if ((i + 1 < n) && (node.imatch == i + 1))
        {
            node.lr[1] = node.p = min(node.lr[1], node.p);
        }
    }
}

void SubProblem::solve_small()
{
    const u_t sz = nodes.size();
    vector<vull_t> si_dists{size_t(sz), vull_t()};
    for (const Query& query: queries)
    {
        if (si_dists[query.s].empty())
        {
            dijkstra_fromto(si_dists[query.s], query.s, true);
        }
        min_by(q_dists[query.qdi], si_dists[query.s][query.e]);
    }
}

void SubProblem::dijkstra_fromto(vull_t& dists, u_t si, bool from_mode) const
{
    dists.clear();
    const u_t sz = nodes.size();
    dists.insert(dists.begin(), sz, ull_t(-1));
    typedef pair<ull_t, u_t> dist_idx_t;
    typedef priority_queue<dist_idx_t> q_t;
    q_t q;
    dists[si] = 0;
    q.push(dist_idx_t(0, si));
    while (!q.empty())
    {
        const dist_idx_t& top = q.top();
        ull_t d = top.first;
        u_t ci = top.second;
        q.pop();
        const Node& node = nodes[ci];
        if (d == dists[ci])
        {
            vector<dist_idx_t> dis; dis.reserve(3);
            ull_t add;
            if (ci > 0)
            {
                add = (from_mode ? node.lr[0] : nodes[ci - 1].lr[1]);
                dis.push_back(dist_idx_t{d + add, ci - 1});
            }
            if (ci + 1 < sz)
            {
                add = (from_mode ? node.lr[1] : nodes[ci + 1].lr[0]);
                dis.push_back(dist_idx_t{d + add, ci + 1});
            }
            add = (from_mode ? node.p : nodes[node.imatch].p);
            dis.push_back(dist_idx_t(d + add, node.imatch));
            for (const dist_idx_t& di: dis)
            {
                if (dists[di.second] > di.first)
                {
                    dists[di.second] = di.first;
                    q.push(di);
                }
            }
        }
        
    }
}

void SubProblem::split_queries()
{
    const u_t* sbis0 = &sbis[0];
    for (u_t qi = 0, nq = queries.size(); qi < nq; ++qi)
    {
        const Query& query = queries[qi];
        auto er_s = equal_range(sbis0, sbis0 + SB_N, query.s);
        auto er_e = equal_range(sbis0, sbis0 + SB_N, query.e);
        u_t s_i = er_s.first - sbis0;
        u_t e_i = er_e.first - sbis0;
        if (er_s.first != er_s.second)
        {
            min_by(q_dists[query.qdi], sb_dists_from[s_i][query.e]);
        }
        else if (er_e.first != er_e.second)
        {
            min_by(q_dists[query.qdi], sb_dists_to[e_i][query.s]);
        }
        else
        {
            ull_t dmin = ull_t(-1), d;
            for (u_t sbi = 0; sbi < SB_N; ++sbi)
            {
                d = sb_dists_to[sbi][query.s] + sb_dists_from[sbi][query.e];
                min_by(dmin, d);
            }
            min_by(q_dists[query.qdi], dmin);
            Query qsub(query);
            if ((s_i % 4) + (e_i % 4) == 0) // both in A or G regions
            {
                qsub.sub_ifgt(sbis[SB_F]);
                sub_queries[0].push_back(qsub);
            }
            else if (s_i == e_i) // same region, one of: {B, CDE, F}
            {
                qsub.sub(sbis[s_i]);
                sub_queries[s_i].push_back(qsub);
            }
        }
    }
}

void SubProblem::split()
{
    for (u_t sbi = 0; sbi < SB_N; ++sbi)
    {
        if (!sub_queries[sbi].empty())
        {
            SubProblem sp(q_dists);
            swap(sp.queries, sub_queries[sbi]);
            if (sbi == 0) // of two regions: A+G
            {
                const u_t dshift = (sbis[3] + 1) - sbis[0];
                const u_t n_nodes = sbis[0] + nodes.size() - sbis[3];
                sp.nodes.push_back(
                    Node(n_nodes - 1, INF_DIST, INF_DIST, INF_DIST));
                for (u_t i = 1; i < sbis[0]; ++i)
                {
                    Node subnode(nodes[i]);
                    subnode.ifgt_sub_shift(sbis[3], dshift);
                    if (i + 1 == sbis[0])
                    {
                        subnode.lr[1] = INF_DIST;
                    }
                    sp.nodes.push_back(subnode);
                }
                for (u_t i = sbis[3] + 1; i < nodes.size(); ++i)
                {
                    Node subnode(nodes[i]); 
                    subnode.ifgt_sub_shift(sbis[3], dshift);
                    sp.nodes.push_back(subnode);
                }
                sp.nodes[sbis[0]].lr[0] = INF_DIST;
                sp.nodes.back().imatch = 0;
            }
            else // single region
            {
                const u_t dshift = (sbis[3] + 1) - sbis[0];
                const u_t n_nodes = (sbis[sbi] + 1) - sbis[sbi - 1];
                sp.nodes.push_back(
                    Node(n_nodes - 1, INF_DIST, INF_DIST, INF_DIST));
                for (u_t i = sbis[sbi] + 1; i < sbis[sbi + 1]; ++i)
                {
                    Node subnode(nodes[i]);
                    subnode.sub_shift(dshift);
                    sp.nodes.push_back(subnode);
                }
                sp.nodes.push_back(Node(0, INF_DIST, INF_DIST, INF_DIST));
            }
            sp.solve();
        }
    }
}

class Emacs
{
 public:
    Emacs(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void compute_pmatch();
    ull_t dijkstra(u_t si, u_t ei);
    u_t left(u_t i) const { return (prog[i] == '(' ? i : pmatch[i]); }

    // non-naive  methods
    void match_nodes(vnode_t& nodes);

    u_t K, Q;
    string prog;
    vull_t L, R, P;
    vu_t S, E;
    ull_t solution;
    vu_t pmatch;
};

Emacs::Emacs(istream& fi) : solution(0)
{
    fi >> K >> Q;
    prog.reserve(K + 2 + 1);
    fi >> prog;
    prog = string("(") + prog + string(")");
    L.reserve(K + 2); R.reserve(K + 2); P.reserve(K + 2);
    L.push_back(INF_DIST);
    R.push_back(INF_DIST);
    P.push_back(INF_DIST);
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(L));
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(R));
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(P));
    L.push_back(INF_DIST);
    R.push_back(INF_DIST);
    P.push_back(INF_DIST);
    S.reserve(Q); E.reserve(Q);
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(S));
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(E));
}

void Emacs::solve_naive()
{
    compute_pmatch();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        ull_t t = dijkstra(S[qi], E[qi]);
        if (dbg_flags & 0x1) { cerr << "q["<<qi<<"]=" << t << '\n'; }
        solution += t;
    }
}

void Emacs::compute_pmatch()
{
    pmatch = vu_t(size_t(K + 2), 0);
    vu_t left_pos;
    for (u_t ci = 0; ci < K + 2; ++ci)
    {
        if (prog[ci] == '(')
        {
            left_pos.push_back(ci);
        }
        else // ')'
        {
            u_t li = left_pos.back();
            pmatch[li] = ci;
            pmatch[ci] = li;
            left_pos.pop_back();
        }
    }
}

ull_t Emacs::dijkstra(u_t si, u_t ei)
{
    typedef pair<ull_t, u_t> dist_idx_t;
    typedef priority_queue<dist_idx_t> q_t;
    q_t q;
    vull_t dists(size_t(K + 2), ull_t(-1)); // infinity
    dists[si] = 0;
    q.push(dist_idx_t(0, si));
    while (!q.empty())
    {
        const dist_idx_t& top = q.top();
        ull_t d = top.first;
        u_t ci = top.second;
        q.pop();
        if (d == dists[ci])
        {
            vector<dist_idx_t> dis; dis.reserve(3);
            if (ci > 0)
            {
                dis.push_back(dist_idx_t{d + L[ci], ci - 1});
            }
            if (ci + 1 < K + 2)
            {
                dis.push_back(dist_idx_t{d + R[ci], ci + 1});
            }
            dis.push_back(dist_idx_t(d + P[ci], pmatch[ci]));
            for (const dist_idx_t& di: dis)
            {
                if (dists[di.second] > di.first)
                {
                    dists[di.second] = di.first;
                    q.push(di);
                }
            }
        }
    }
    return dists[ei];
}

void Emacs::solve()
{
    vull_t q_dists(size_t(Q), INF_DIST);
    SubProblem sp(q_dists);
    sp.nodes.reserve(K+2);
    for (u_t i = 0; i < K + 2; ++i)
    {
        sp.nodes.push_back(Node(L[i], R[i], P[i]));
    }
    match_nodes(sp.nodes);
    sp.queries.reserve(Q);
    for (u_t i = 0; i < Q; ++i)
    {
        sp.queries.push_back(Query(S[i], E[i], i));
    }
    sp.solve();
    if (dbg_flags & 0x1)
    {
        for (u_t qi = 0; qi < Q; ++qi)
        {
            cerr << "q["<<qi<<"]=" << q_dists[qi] << '\n';
        }
    }
    solution = accumulate(q_dists.begin(), q_dists.end(), 0);
}

void Emacs::match_nodes(vnode_t& nodes)
{ // non recursive to avoid stack overflow. Similar to compute_pmatch
    vu_t left_pos;
    for (u_t ci = 0; ci < K + 2; ++ci)
    {
        Node& node = nodes[ci];
        node.parents[0] = (left_pos.empty() ? u_t(-1) : left_pos.back());
        if (prog[ci] == '(')
        {
            left_pos.push_back(ci);
        }
        else // ')'
        {
            u_t li = left_pos.back();
            node.imatch = li;
            nodes[li].imatch = ci;
            left_pos.pop_back();
        }
    }
    for (u_t ci = 1; ci < K + 1; ++ci)
    {
        Node& node = nodes[ci];
        node.parents[1] = nodes[node.parents[0]].imatch;
    }
    nodes[K + 1].parents[1] = u_t(-1);
}

void Emacs::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Emacs::*psolve)() =
        (naive ? &Emacs::solve_naive : &Emacs::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Emacs emacs(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (emacs.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        emacs.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

typedef vector<bool> vb_t;

static void check_push_segment(vau2_t& segments, u_t b, u_t e)
{
    if (b < e)
    {
        segments.push_back(au2_t{b, e});
    }
}

static void rand_fill(vb_t& right)
{
    const u_t K = right.size();
    vau2_t segments;
    segments.push_back(au2_t{0, K});
    while (!segments.empty())
    {
        const au2_t seg = segments.back();
        const u_t b = seg[0], e = seg[1];
        segments.pop_back();
        const u_t delta = e - b; // != 0
        const u_t half = delta/2;
        const u_t add = 2*(rand() % half);
        if (add == 0)
        {
            right[b] = false;
            right[e - 1] = true;
            check_push_segment(segments, b + 1, e - 1);
        }
        else
        {
            u_t cut = b + add;
            right[b] = false;
            right[cut - 1] = true;
            check_push_segment(segments, b + 1, cut - 1);
            check_push_segment(segments, cut, e);
        }
    }
}

#if 0
static void rand_fill_recursive(vb_t& left, u_t b, u_t e) // (e - b) % 2 = 0
{
    if (b < e)
    {
        u_t delta = (e - b);
        u_t half = delta / 2;
        u_t add = 2*(rand() % half);
        if (add == 0)
        {
            left[b] = true;
            left[e - 1] = false;
            rand_fill(left, b + 1, e - 1);
        }
        else
        {
            u_t cut = b + add;
            left[b] = true;
            left[cut - 1] = false;
            rand_fill(left, b + 1, cut - 1);
            rand_fill(left, cut, e);
        }
    }
}
#endif

static void write_rand_inpt(const char* fn, u_t K, u_t Q)
{
    ofstream f(fn);
    f << "1\n" << K << ' ' << Q << '\n';
    vb_t right(size_t(K), false);
    rand_fill(right);
    for (bool b: right)
    {
        f << "()"[int(b)];
    }
    f << '\n';
    for (u_t lrp = 0; lrp < 3; ++lrp)
    {
        const char* sep = "";
        for (u_t i = 0; i < K; ++i)
        {
            f << sep << (rand() % 1000000) + 1; sep = " ";
        }
        f << '\n';
    }
    for (u_t se = 0; se < 2; ++se)
    {
        const char* sep = "";
        for (u_t i = 0; i < Q; ++i)
        {
            u_t idx = (rand() % K) + 1;
            f << sep << idx; sep = " ";
        }
        f << '\n';
    }
    f.close();
}

static int run_compare(const char* fn, bool compare)
{
    ull_t solution = 0, solution_naive = 0, dum;
    if (compare)
    {
        ifstream fin(fn);
        fin >> dum; // T=1
        Emacs emacs(fin);
        emacs.solve_naive();
        solution_naive = emacs.get_solution();
    }
    ifstream fin(fn);
    fin >> dum; // T=1
    Emacs emacs(fin);
    emacs.solve();
    solution = emacs.get_solution();
    int rc = 0;
    if (compare && (solution != solution_naive))
    {
        cerr << "Failed: solution=" << solution << " != naive=" <<
            solution_naive << '\n';
        rc = 1;
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 1; // "test";
    int nt = stoi(argv[++ai]);
    int Kmin = stoi(argv[++ai]);
    int Kmax = stoi(argv[++ai]);
    int Q = stoi(argv[++ai]);
    const char* fn_small = "emacs-auto.in";
    const char* fn_big = "emacs-auto-big.in";
    for (int ti = 0; (ti < nt) && (rc == 0); ++ti)
    {
        cerr << "tested: " << ti << '/' << nt << '\n';
        u_t K = Kmin + rand() % (Kmax + 1 - Kmin);
        const char* fn = (K < 100 ? fn_small : fn_big);
        K = 2*(K / 2);
        write_rand_inpt(fn, K, Q);
        rc = run_compare(fn, K < 100);
    }
    return rc;
}

int main(int argc, char ** argv)
{
    int rc = 0;
    if ((argc > 1) && (string(argv[1]) == string("test")))
    {
        rc = test(argc, argv);
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
