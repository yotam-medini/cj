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

void vifgt_sub_shift(u_t& v, u_t t, u_t d)
{
    if (v > t)
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
        const au2_t& _parents=au2_t{u_t(-1), u_t(-1)}) : 
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
    void ifgt_sub_shift(u_t t, u_t d)
    {
        vifgt_sub_shift(s, t, d);
        vifgt_sub_shift(e, t, d);
    }
    u_t s, e;
    u_t qdi; // index ot original query index
};
typedef vector<Query> vquery_t;

class SubProblem
{
 public:
    SubProblem(vull_t& _q_dists) : 
       sbis{u_t(-1), u_t(-1), u_t(-1), u_t(-1)}, q_dists(_q_dists) {}
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
        while (sbis[SB_F] + 1 - sbis[SB_B] <= half)
        {
            sbis[SB_C] = sbis[SB_B];
            sbis[SB_E] = sbis[SB_F];
            const au2_t& parents = nodes[sbis[SB_B]].parents;
            sbis[SB_B] = parents[0];
            sbis[SB_F] = parents[1];
        }
        if (dbg_flags & 0x2) { cerr << "sbis: ";
            copy(&sbis[0], &sbis[4], ostream_iterator<u_t>(cerr, " "));
            cerr << '\n'; }
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
    const u_t dshift = (sbis[3] + 1) - sbis[0];
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
                qsub.ifgt_sub_shift(sbis[3], dshift);
                sub_queries[0].push_back(qsub);
            }
            else if (s_i == e_i) // same region, one of: {B, CDE, F}
            {
                qsub.sub(sbis[s_i - 1]);
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
                const u_t n_nodes = (sbis[sbi] + 1) - sbis[sbi - 1];
                sp.nodes.reserve(n_nodes);
                sp.nodes.push_back(
                    Node(n_nodes - 1, INF_DIST, INF_DIST, INF_DIST));
                for (u_t i = sbis[sbi - 1] + 1; i < sbis[sbi]; ++i)
                {
                    const Node& node = nodes[i];
                    Node subnode(node);
                    subnode.sub_shift(sbis[sbi - 1]);
                    if (node.parents[0] <= sbis[sbi - 1]) // top in sub-region
                    {
                        subnode.parents = {0, n_nodes - 1};
                    }
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
    Emacs(
        const string& _prog, 
        const vull_t& _L, 
        const vull_t& _R, 
        const vull_t& _P, 
        const vu_t& _S, 
        const vu_t& _E);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
    const vull_t& get_q_dists() const { return q_dists; }
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

    vull_t q_dists;
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

Emacs::Emacs(
        const string& _prog, 
        const vull_t& _L, 
        const vull_t& _R, 
        const vull_t& _P, 
        const vu_t& _S, 
        const vu_t& _E) :
    K(_prog.size()),
    Q(_S.size()),
    prog(string("(") + _prog + string(")")),
    S(_S),
    E(_E)
{
    L.reserve(K + 2); R.reserve(K + 2); P.reserve(K + 2);
    L.push_back(INF_DIST);
    R.push_back(INF_DIST);
    P.push_back(INF_DIST);
    L.insert(L.end(), _L.begin(), _L.end());
    R.insert(R.end(), _R.begin(), _R.end());
    P.insert(P.end(), _P.begin(), _P.end());
    L.push_back(INF_DIST);
    R.push_back(INF_DIST);
    P.push_back(INF_DIST);
}

void Emacs::solve_naive()
{
    compute_pmatch();
    q_dists.reserve(Q);
    for (u_t qi = 0; qi < Q; ++qi)
    {
        ull_t t = dijkstra(S[qi], E[qi]);
        if (dbg_flags & 0x1) { cerr << "q["<<qi<<"]=" << t << '\n'; }
        q_dists.push_back(t);
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
    q_dists.insert(q_dists.end(), size_t(Q), INF_DIST);
    SubProblem sp(q_dists);
    sp.nodes.reserve(K+2);
    for (u_t i = 0; i < K + 2; ++i)
    {
        sp.nodes.push_back(Node(0, L[i], R[i], P[i]));
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
        if (prog[ci] == '(')
        {
            node.parents[0] = (left_pos.empty() ? u_t(-1) : left_pos.back());
            left_pos.push_back(ci);
        }
        else // ')'
        {
            u_t li = left_pos.back();
            node.imatch = li;
            nodes[li].imatch = ci;
            left_pos.pop_back();
            node.parents[0] = (left_pos.empty() ? u_t(-1) : left_pos.back());
        }
    }
    for (u_t ci = 1; ci < K + 1; ++ci)
    {
        Node& node = nodes[ci];
        node.parents[1] = nodes[node.parents[0]].imatch;
    }
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

class Test
{
 public:
    Test(u_t _K, u_t _LRPmax, u_t _nt, u_t& _tcount) :
       K(_K), LRPmax(_LRPmax), nt(_nt), tcount(_tcount) {}
    int test_gen_prog(const string& head, u_t n_open_left);
 private:
    int compare_prog(const string& prog);
    u_t rand_lrp() const
    {
        u_t ret = 1;
        if (1 < LRPmax)
        {
            ret += (rand() % (LRPmax - 1));
        }
        return ret;
    }
    u_t K;
    u_t LRPmax;
    u_t nt;
    u_t& tcount;
};

int Test::compare_prog(const string& prog)
{
    int rc = 0;
    vull_t L, R, P;
    L.reserve(K);
    R.reserve(K);
    P.reserve(K);
    while (L.size() < K)
    {
        L.push_back(rand_lrp());
        R.push_back(rand_lrp());
        P.push_back(rand_lrp());
    }
    vu_t S, E;
    S.reserve(K*K);
    E.reserve(K*K);
    for (u_t s = 1; s <= K; ++s)
    {
        for (u_t e = 1; e <= K; ++e)
        {
            S.push_back(s);
            E.push_back(e);
        }
    }
    cerr << "Running test " << tcount << " : " << prog << '\n';
    ++tcount;

    Emacs emacs_naive(prog, L, R, P, S, E);
    emacs_naive.solve_naive();
    const vull_t& naive_dists = emacs_naive.get_q_dists();

    Emacs emacs(prog, L, R, P, S, E);
    emacs.solve();
    const vull_t& dists = emacs.get_q_dists();

    if (naive_dists != dists)
    {
        rc = 1;
        cerr << "compare results failed\n";
        u_t qi = 0;
        for (; naive_dists[qi] == dists[qi]; ++qi) {}
        ofstream f("emacs-auto.in");
        f << "1\n" << K << " 1\n" << prog << '\n';
        const char *sep = "";
        for (ull_t x: L) { f << sep << x; sep = " "; }; sep = "";
        for (ull_t x: R) { f << sep << x; sep = " "; }; sep = "";
        for (ull_t x: P) { f << sep << x; sep = " "; }; sep = "";
        f << S[qi] << '\n' << E[qi] << '\n';
        f.close();
    }

    return rc;
}

int Test::test_gen_prog(const string& head, u_t n_open_left)
{
    int rc = 0;
    u_t head_sz = head.size();
    if (head_sz == K)
    {
        rc = compare_prog(head);
    }
    else
    {
        if (n_open_left < (K - head_sz))
        {
            rc = test_gen_prog(head + string("("), n_open_left + 1);
        }
        if ((rc == 0) && (n_open_left > 0))
        {
            rc = test_gen_prog(head + string(")"), n_open_left - 1);
        }
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 1; // "test";
    u_t nt = stoi(argv[++ai]);
    u_t Kmin = stoi(argv[++ai]);
    u_t Kmax = stoi(argv[++ai]);
    u_t LRPmax = stoi(argv[++ai]);
    u_t tcount = 0;
    for (u_t K = 2*(Kmin/2); 
        (rc == 0) && (tcount < nt) && (K <= 2*(Kmax/2)); K += 2)
    {
        Test ktest(K, LRPmax, nt, tcount);
        rc = ktest.test_gen_prog(string(""), 0);
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
