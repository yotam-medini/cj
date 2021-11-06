// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <array>
#include <queue>
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
typedef vector<aull2_t> vaull2_t;

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

class CycleDist
{
 public:
    CycleDist(const vull_t& _fwd=vull_t(), const vull_t& _bwd=vull_t());
    void set(const vull_t& _fwd, const vull_t& _bwd); // _fwd.size()==_bwd.size()
    ull_t dist(u_t i, u_t j) const;
    u_t dist_naive(u_t i, u_t j) const;
    u_t size() const { return fwd.size(); }
 private:
    void set_sums();
    vull_t fwd, bwd;
    vull_t fwd_sum, bwd_sum;
};

CycleDist::CycleDist(const vull_t& _fwd, const vull_t& _bwd) :
    fwd(_fwd), bwd(_bwd)
{
    set_sums();
}

void CycleDist::set(const vull_t& _fwd, const vull_t& _bwd)
{
    fwd = _fwd;
    bwd = _bwd;
    set_sums();
}

void CycleDist::set_sums()
{
    const size_t n = fwd.size();
    fwd_sum.clear();
    bwd_sum.clear();
    fwd_sum.reserve(n);
    bwd_sum.reserve(n);
    fwd_sum.push_back(0);
    bwd_sum.push_back(0);
    for (size_t i = 0; i < n; ++i)
    {
        fwd_sum.push_back(fwd_sum.back() + fwd[i]);
        bwd_sum.push_back(bwd_sum.back() + bwd[i]);
    }
}

ull_t CycleDist::dist(u_t i, u_t j) const
{
    ull_t d = 0;
    if (i != j)
    {
        ull_t d_fwd = 0, d_bwd = 0;
        if (i < j)
        {
            d_fwd = fwd_sum[j] - fwd_sum[i];
            d_bwd = (bwd_sum.back() - bwd_sum[j]) + bwd_sum[i];
        }
        else // if (j < i)
        {
            d_fwd = (fwd_sum.back() - fwd_sum[i]) + fwd_sum[j];
            d_bwd = bwd_sum[i] - bwd_sum[j];
        }
        d = min(d_fwd, d_bwd);
    }
    return d;
}

class Brick
{
 public:
    Brick() : lr{-1, -1} {}
    CycleDist cycle_dist;
    ai2_t lr;
    vu_t lr_child;
};
typedef vector<Brick> vbrick_t;;

class Node // per position
{
 public:
    Node(u_t d=0, const ai2_t& ps=ai2_t{-1, -1}, u_t _ib=0) :
        depth(d),
        parents(ps),
        ibrick(_ib)
    {}
    string str() const;
    u_t depth;
    ai2_t parents;
    u_t ibrick; // index to bricks;
    vaull2_t to_ancestor; // 2^p steps
    vaull2_t ex_ancestor; // 2^p steps
};

string Node::str() const
{
    ostringstream os;
    os << "{D: " << depth << ", P={"<<parents[0] << ", " << parents[1] << "}"
        ", ib=" << ibrick << ", to=" << v2str(to_ancestor) <<
        ", ex=" << v2str(ex_ancestor) << "}";
    return os.str();

}

typedef vector<Node> vnode_t;

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
    void build_bricks();
    void compute_dists();
    void brick_set_cd(Brick& brick) const;
    u_t K, Q;
    string prog;
    vu_t L, R, P;
    vu_t S, E;
    ull_t solution;
    vu_t pmatch;
    vbrick_t bricks; // K/2 + 1
    vnode_t nodes; // K
};

Emacs::Emacs(istream& fi) : solution(0)
{
    fi >> K >> Q;
    fi >> prog;
    L.reserve(K); R.reserve(K); P.reserve(K);
    copy_n(istream_iterator<u_t>(fi), K, back_inserter(L));
    copy_n(istream_iterator<u_t>(fi), K, back_inserter(R));
    copy_n(istream_iterator<u_t>(fi), K, back_inserter(P));
    S.reserve(Q); E.reserve(Q);
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(S));
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(E));
}

void Emacs::solve_naive()
{
    compute_pmatch();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        ull_t t = dijkstra(S[qi] - 1, E[qi] - 1);
        solution += t;
    }
}

void Emacs::compute_pmatch()
{
    pmatch = vu_t(size_t(K), 0);
    vu_t left_pos;
    for (u_t ci = 0; ci < K; ++ci)
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
    vu_t dists(size_t(K), u_t(-1)); // infinity
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
            if (ci + 1 < K)
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
    build_bricks();
    compute_dists();
}

void Emacs::build_bricks()
{
    u_t depth = 0;
    vvu_t depths_nodes;
    vi_t lps; lps.push_back(-1);
    bricks.push_back(Brick());
    vu_t ib_stack; ib_stack.push_back(0);
    depths_nodes.push_back(vu_t()); // Fake empty presenting  [-1, K]
    for (u_t i = 0; i < K; ++i)
    {
        // nodes.size() == i
        // depth == depths_nodes.size();
        if (prog[i] == '(')
        {
            bricks[ib_stack.back()].lr_child.push_back(i);
            ib_stack.push_back(bricks.size());            
            bricks.push_back(Brick());
            bricks.back().lr[0] = i;
            depths_nodes.push_back(vu_t());
            ++depth;
            nodes.push_back(Node(depth, ai2_t{lps.back(), -1}, ib_stack.back()));
            depths_nodes.back().push_back(i);
            lps.push_back(i);
        }
        else
        {
            lps.pop_back();
            for (u_t j: bricks[ib_stack.back()].lr_child)
            {
                nodes[j].parents[1] = i;
            }
            bricks[ib_stack.back()].lr[1] = i;
            ib_stack.pop_back();
            bricks[ib_stack.back()].lr_child.push_back(i);
            nodes.push_back(Node(depth, ai2_t{lps.back(), -1}, ib_stack.back()));
            // depths_nodes.back().push_back(i);
            --depth;
        }
    }
    bricks[0].lr = ai2_t{-1, int(K)};
    for (u_t i: bricks[0].lr_child)
    {
        nodes[i].parents[1] = K;
    }
    if (dbg_flags) {
        cerr << "Nodes: {\n";
        for (u_t i = 0; i < K; ++i) {
            const Node& node = nodes[i];
            cerr << "  [" << i << "] " << node.str() << '\n';
        }
        cerr << "}\nBricks: {\n";
        for (u_t bi = 0; bi < bricks.size(); ++bi) {
            const Brick& brick = bricks[bi];
            cerr << "  [" << bi << "] lr=" << brick.lr << ", Sub-LRs: " <<
                v2str(brick.lr_child) << '\n';
        }
        cerr << "}\n";
    }
}

void Emacs::compute_dists()
{
    vau2_t brick_stack;
    brick_stack.push_back(au2_t{0, 0});
    while (!brick_stack.empty())
    {
        au2_t& bi_ci = brick_stack.back();
        if (dbg_flags & 0x2) { cerr << __func__ << ", bici="<<bi_ci << '\n'; }
        Brick& brick = bricks[bi_ci[0]];
        u_t ci = bi_ci[1];
        const u_t nlrc = brick.lr_child.size();
        if (ci < nlrc)
        {
            u_t li = brick.lr_child[ci];
            u_t bi_child = nodes[li].ibrick;
            brick_stack.push_back(au2_t{bi_child, 0});
        }
        else
        {
            brick_set_cd(brick);
            brick_stack.pop_back();
            if (!brick_stack.empty())
            {
                brick_stack.back()[1] += 2;
            }
        }
    }
}

void Emacs::brick_set_cd(Brick& brick) const
{
    const u_t nlrc = brick.lr_child.size();
    CycleDist& cd = brick.cycle_dist;
    if (dbg_flags & 0x2) { cerr << "#(cd)=" << cd.size() << '\n'; }
    vull_t fwd, bwd;
    const int l = brick.lr[0];
    const u_t r = brick.lr[1];
    if (nlrc == 0)
    {
        ull_t l2r = min(R[l], P[l]);
        ull_t r2l = min(L[r], P[r]);
        fwd.push_back(l2r);
        fwd.push_back(r2l);
        bwd.push_back(r2l);
        bwd.push_back(l2r);
    }
    else
    {

        fwd.push_back(l >= 0 ? R[l] : INF_DIST);
        bwd.push_back(L[l + 1]); // l + 1 == lr_child[0]
        for (u_t lri = 0; lri < nlrc; lri += 2)
        {
            const u_t ni = brick.lr_child[lri];
            const u_t bi = nodes[ni].ibrick;
            const CycleDist& scd = bricks[bi].cycle_dist;
            const u_t cd_sz = scd.size();
            fwd.push_back(scd.dist(0, cd_sz - 1));
            bwd.push_back(scd.dist(cd_sz - 1, 0));
        }
        fwd.push_back(r < K ? P[r] : INF_DIST);
        fwd.push_back(l >= 0 ? P[l] : INF_DIST);
    }
    cd.set(fwd, bwd);
    if (dbg_flags & 0x2) { cerr << "#(cd)=" << cd.size() << '\n'; }
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
