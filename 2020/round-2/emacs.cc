// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
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
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

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

class Parent
{
 public:
    Parent(class Node* p=0) : pnode(p), price_up{0, 0}, price_down{0, 0} {}
    class Node* pnode;
    aull2_t price_up; // left-to-right, right-to-left
    aull2_t price_down; // left-to-right, right-to-left
};
typedef vector<Parent> vparent_t;

// All positions have Node, and also the special top.
// Only top, and positions with left-parentheses have children.
// All have depth (top's depth = 0).
// depth of modulo 2^n will have parents dists for 2*0, 2^2 ... 2*n above.

class Node
{
 public:
    Node(u_t _i=0) :
        i(_i), depth(0), price(0),
        lparent(u_t(-1)) {}
    u_t i;
    u_t depth;
    ull_t price; // teleport; 
    u_t lparent; 
    vu_t children;
    vull_t children_dists_sums[2]; // left-to-right, right-to-left
    vparent_t parents_dists; // 2^n-geometric
};
typedef vector<Node> vnode_t;

#if 0
void Node::print(ostream& os, const string& indent) const
{
    os << indent << i << ", #=" << children.size() << '\n';
    const string subind = indent + string("  ");
    for (u_t ci = 0; ci < n_children; ++ci)
    {
        children[ci].print(os, subind);
    }
}
#endif

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
    void build_tree();
    void set_children_dists();
    void set_parents_dists();
    void node_set_parents_dists(vparent_t& pdists, Node* pnode);
    ull_t qdist(u_t start, u_t end) const;
    void tree_print(ostream& os, const Node& node, const string& indent = "")
        const;
    u_t K, Q;
    string prog;
    vu_t L, R, P;
    vu_t S, E;
    ull_t solution;
    vu_t pmatch;
    Node root;
    vnode_t tree;
    vu_t Palt; // P or better
    vau2_t depth_idxs;
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

void Emacs::solve()
{
    compute_pmatch();
    build_tree();
    if (dbg_flags & 0x1) { tree_print(cerr, root); }
    set_children_dists();
    if (dbg_flags & 0x1) { tree_print(cerr, root); }
    // set_parents_dists();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        ull_t t = qdist(S[qi] - 1, E[qi] - 1);
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

void Emacs::build_tree()
{
    tree.reserve(K);
    for (u_t i = 0; i < K; ++i)
    {
        tree.push_back(Node(i));
    }
    priority_queue<au2_t, vau2_t, greater<au2_t>> q; // (septh, idx)
    for (u_t l = 0; l < K; l = pmatch[l] + 1)
    {
        root.children.push_back(l);
        q.push(au2_t{1, l});
    }
    while (!q.empty())
    {
        const au2_t& depth_idx = q.top();
        const u_t depth = depth_idx[0];
        const u_t i = depth_idx[1];
        const u_t depth1 = depth + 1;
        q.pop();
        Node& node = tree[i];
        const u_t e = pmatch[i] - 1;
        for (u_t l = i + 1; l < e; l = pmatch[l] + 1)
        {
            node.children.push_back(l);
            const u_t r = pmatch[l];
            tree[l].lparent = tree[r].lparent = i;
            q.push(au2_t{depth1, l});
        }
    }
}

void Emacs::set_children_dists()
{
    Palt = P;
    depth_idxs.reserve(K/2);
    for (u_t i = 0; i < K; ++i)
    {
        if (prog[i] == '(')
        {
            depth_idxs.push_back(au2_t{tree[i].depth, i});
        }
    }
    sort(depth_idxs.begin(), depth_idxs.end(), greater<au2_t>());
    for (const au2_t& di: depth_idxs)
    {
        const u_t i = di[1];
        const u_t r = pmatch[i];
        Node& node = tree[i];
        if (node.children.empty())
        {
            minby(Palt[i], R[i]);
            minby(Palt[r], L[r]);
        }
        else
        {
            const size_t nc = node.children.size();
            vull_t& ps0 = node.children_dists_sums[0];
            vull_t& ps1 = node.children_dists_sums[1];
            ps0.reserve(nc + 1);
            ps1.reserve(nc + 1);
            ps0.push_back(0);
            ps1.push_back(0);
            for (u_t ci = 0; ci < nc; ++ci)
            {
                const u_t j = node.children[ci];
                const u_t step = (j > i ? R[j - 1] : 0) + Palt[j];
                ps0.push_back(ps0.back() + step);
            }
            {
                const u_t jr = pmatch[node.children.back()];
                u_t r_steps_r = ps0.back() + (jr < r ? R[jr] : 0);
                minby(Palt[i], r_steps_r);
            }
            u_t jl = 0;
            for (u_t ci = nc; ci > 0;)
            {
                --ci;
                jl = node.children[ci];
                const u_t jr = pmatch[jl];
                const u_t step = (jr < r ? L[jr + 1] : 0) + Palt[jr];
                ps1.push_back(ps1.back() + step);
            }
            {
                const u_t l_steps_l = ps1.back() + (jl > 0 ? L[jl] : 0);
                minby(Palt[r], l_steps_l);
            }
        }
    }
}

void Emacs::tree_print(ostream& os, const Node& node, const string& indent) const
{
    os << indent << "[" << node.i << ", " << pmatch[node.i] << 
        "], D=" << node.depth << ", #=" << node.children.size() << '\n';
    for (u_t i: {0, 1})
    {
        const vull_t& cps = node.children_dists_sums[i];
        if (!cps.empty())
        {
            os << indent << "CPS[" <<i << "]:";
            for (ull_t x: cps) { os << ' ' << x; }
            os << '\n';
        }
    }
    const string subind = indent + string("  ");
    for (u_t ci: node.children)
    {
        tree_print(os, tree[ci], subind);
    }
}

#if 0
void Emacs::build_tree()
{
    vnodep_t stack; // instead of too deep recursion
    // root is different 'border-less
    // With we had top surrounding (...)
    tree = new Node;
    make_children(stack, tree, 0, K);
    while (!stack.empty())
    {
        Node* pnode = stack.back();
        Node& node = *pnode;
        const au2_t& be = node.be;
        const u_t b = be[0], e = be[1], r = e - 1;
        if (b + 2 == e) // single () == n_children == 0
        {
            node.price[0] = min(R[b], P[b]);
            node.price[1] = min(L[r], P[r]);
            stack.pop_back();
        }
        else if (node.n_children == 0)
        {
            make_children(stack, pnode, b + 1, e - 1);
        }
        else // children already allocated
        {
            aull2_t alt_rl = {0, 0};
            if (node.parent)
            {
                alt_rl = aull2_t{R[b], L[r]};
            }
            for (u_t i: {0, 1})
            {
                vull_t& price_sums = node.children_price_sums[i];
                price_sums.reserve(node.n_children + 1);
                price_sums.push_back(0);
                for (u_t ci = 0, nc = node.n_children; ci < nc; ++ci)
                {
                    const Node& child = node.children[ci];
                    ull_t step = child.price[0];
                    if (i == 0) // right
                    {
                        if (child.be[1] < K)
                        {
                            step += R[child.be[1] - 1];
                        }
                    }
                    else // left
                    {
                        if (child.be[0] > 0)
                        {
                            step += L[child.be[0]];
                        }
                    }
                    alt_rl[i] += step;
                    price_sums.push_back(price_sums.back() + step);
                }
            }
            node.price[0] = min<ull_t>(P[b], alt_rl[0]);
            node.price[1] = min<ull_t>(P[r], alt_rl[1]);
            stack.pop_back();
        }
    }
}

#endif

#if 0
class NodeChild
{
 public:
    NodeChild(Node* _p=0, u_t _ci=0) : p(_p), ci(_ci) {}
    Node* p;
    u_t ci;
};
typedef vector<NodeChild> vnodeci_t;

void Emacs::set_parents_dists()
{
    vparent_t pdists;
    vnodeci_t stack; // instead of too deep recursion
    stack.push_back(NodeChild(tree, 0));
    while (!stack.empty())
    {
        u_t bi = stack.size() - 1;
        NodeChild& nc = stack.back();
        if (nc.ci == 0)
        {
            if (dbg_flags & 0x1) {
                cerr << __func__ << " be: " << nc.p->be << '\n'; }
            if (nc.p->parent)
            {
                node_set_parents_dists(pdists, nc.p);
            }
        }
        if (nc.ci < nc.p->n_children)
        {
            Node* child = &nc.p->children[nc.ci];
            stack.push_back(NodeChild(child, 0));
            ++stack[bi].ci;
        }
        else
        {
            stack.pop_back();
        }
    }
}

void Emacs::node_set_parents_dists(vparent_t& pdists, Node* pnode)
{
    size_t idx = pnode->my_index();
    Node* pa = pnode->parent;
    u_t siblings = pa->n_children;
    Parent parent(pa);
    // ull_t right = 
    pdists.push_back(parent);
}
#endif

ull_t Emacs::qdist(u_t start, u_t end) const
{
    ull_t dist = 0;
    return dist;
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
