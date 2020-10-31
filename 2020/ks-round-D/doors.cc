// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<bool> vb_t;
typedef array<int, 2> ai2_t;
typedef array<u_t, 2> au2_t;

class Query
{
 public:
    Query(u_t _s=0, u_t _k=0): s(_s), k(_k) {}
    u_t s, k;
};
typedef vector<Query> vq_t;

static unsigned dbg_flags;

class Node // Door
{
 public:
    static const int undef = -1;
    Node(u_t _p=undef, const ai2_t& _c=ai2_t{undef, undef},
        const au2_t& _sz=au2_t{0, 0}) :
            parent(_p), child(_c), size(_sz)
        {}
    int parent;
    ai2_t child;
    au2_t size; // rooms
};
typedef vector<Node> vnode_t;

class Doors
{
 public:
    Doors(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t run_query(const Query& q);
    void build_tree();
    void build_tree_nodes();
    void set_tree_levels_sizes(u_t inode, u_t depth);
    u_t response(const Query& q) const;
    bool qinside(const Query& q, u_t inode) const;
    void print_tree(u_t inode, const string& indent) const;
    u_t N, Q;
    vu_t d;
    vq_t qs;
    vu_t solution;
    vnode_t nodes; // tree
    u_t root;
    vvu_t tree_levels;
};

Doors::Doors(istream& fi)
{
    fi >> N >> Q;
    d.reserve(N - 1);
    qs.reserve(Q);
    copy_n(istream_iterator<u_t>(fi), N - 1, back_inserter(d));
    for (u_t i = 0; i < Q; ++i)
    {
        u_t s, k;
        fi >> s >> k;
        Query q(s- 1, k - 1);
        qs.push_back(q);
    }
}

void Doors::solve_naive()
{
    for (const Query& query: qs)
    {
        u_t s = run_query(query);
        solution.push_back(s);
    }
}

u_t Doors::run_query(const Query& q)
{
    u_t ret = q.s;
    // vu_t locked(N - 1, true);
    int lock_left = q.s - 1;
    u_t lock_right = q.s;
    for (u_t i = 0; i < q.k; ++i)
    {
        if (lock_left < 0)
        {
            ++lock_right;
            ret = lock_right;
        }
        else if (lock_right + 1 == N)
        {
            ret = lock_left;
            --lock_left;
        }
        else
        {
            if (d[lock_left] < d[lock_right])
            {
                ret = lock_left;
                --lock_left;
            }
            else
            {
                ++lock_right;
                ret = lock_right;
            }
        }
    }
    return ret;
}

void Doors::solve()
{
    build_tree();
    for (const Query& q: qs)
    {
        u_t r = (q.k == 0 ? q.s : response(q));
        solution.push_back(r);
    }
}

void Doors::build_tree()
{
    build_tree_nodes();
    set_tree_levels_sizes(root, 0);
    if (dbg_flags & 0x1) { print_tree(root, ""); }
}

void Doors::build_tree_nodes()
{
    vu_t stack;
    nodes = vnode_t(N - 1, Node());
    stack.push_back(0);
    for (u_t i = 1; i < N - 1; ++i)
    {
        u_t itop = stack.back();
        const u_t dtop = d[stack.back()];
        const u_t di = d[i];
        if (dtop < di)
        {
            u_t pre_top;
            for (u_t sz = stack.size();
                (sz >= 2) && (d[(pre_top = stack[sz - 2])] < di);
                --sz, itop = pre_top)
            {
                nodes[itop].parent = pre_top;
                nodes[pre_top].child[1] = itop;
                stack.pop_back();
            }
            nodes[itop].parent = i;
            nodes[i].child[0] = itop;
            stack.back() = i;
        }
        else // di < dtop
        {
            stack.push_back(i);
        }
    }
    for (u_t sz = stack.size(), itop = stack.back(); sz >= 2; --sz)
    {
        const u_t pre_top = stack[sz - 2];
        nodes[itop].parent = pre_top;
        nodes[pre_top].child[1] = itop;
        itop = pre_top;
    }
    root = stack[0];
}

void Doors::set_tree_levels_sizes(u_t inode, u_t depth)
{
    while (tree_levels.size() <= depth)
    {
        tree_levels.push_back(vu_t());
    }
    tree_levels[depth].push_back(inode);
    Node& node = nodes[inode];
    for (u_t ci: {0, 1})
    {
        int child = node.child[ci];
        if (child != -1)
        {
            set_tree_levels_sizes(child, depth + 1);
            const Node& child_node = nodes[child];
            node.size[ci] = child_node.size[0] + child_node.size[1];
        }
        else
        {
            node.size[ci] = 1;
        }
    }
}

void Doors::print_tree(u_t inode, const string& indent) const
{
    const Node& node = nodes[inode];
    cerr << indent << "[" << inode << "] D=" << d[inode] <<
        " sizes=[" << node.size[0] << ", " << node.size[1] << "]\n";
    string sub_indent = indent + string("  ");
    for (u_t ci: {0, 1})
    {
        const int c = node.child[ci];
        if (c == Node::undef)
        {
            cerr << sub_indent << "null\n";
        }
        else
        {
            print_tree(c, sub_indent);
        }
    }
}

u_t Doors::response(const Query& q) const
{
    u_t r = q.s;
    u_t low = 0, high = tree_levels.size();
    // find the last door to be opened
    vu_t good_candidates;
    vu_t candidates;
    while (low < high)
    {
        candidates.clear();
        u_t mid_level = (low + high)/2;
        const vu_t& tl = tree_levels[mid_level];
        auto er = equal_range(tl.begin(), tl.end(), q.s);
        u_t i1 = er.first - tl.begin();
        u_t i2 = er.second - tl.begin();
        if (i1 == i2)
        {
            if (i1 == 0)
            {
                candidates.push_back(tl[0]);
            }
            else if (i1 < tl.size())
            {
                candidates.push_back(tl[i1 - 1]);
                candidates.push_back(tl[i1]);
            }
            else // i1 == tl.size()
            {
                candidates.push_back(tl[i1 - 1]);
            }
        }
        else
        {
            candidates.push_back(tl[i1]);
        }
        bool possible = false;
        for (u_t candid: candidates)
        {
            possible = possible || qinside(q, candid);
        }
        if (possible)
        {
            good_candidates = candidates;
            low = mid_level + 1;
        }
        else
        {
            high = mid_level;
        }
    }
    u_t hard_door = good_candidates[0];
    if ((good_candidates.size() == 2) & (d[good_candidates[1]] < d[hard_door]))
    {
        hard_door = good_candidates[1];
    }
    const Node& door_node = nodes[hard_door];
    if (q.s <= hard_door) // comming from left
    {
         r = hard_door + (q.k - (door_node.size[0] - 1));
    }
    else // coming from right
    {
         r = (hard_door + 1) - (q.k - (door_node.size[1] - 1));
    }

    return r;
}

bool Doors::qinside(const Query& q, u_t inode) const
{
    bool inside = false;
    const Node& node = nodes[inode];
    if (q.s <= inode)
    {
        inside = (inode - q.s) + 1 <= node.size[0];
    }
    else
    {
        inside = (q.s - inode) <= node.size[1];
    }
    inside = inside && (q.k < node.size[0] + node.size[1]);
    return inside;
}

void Doors::print_solution(ostream &fo) const
{
    for (u_t s: solution)
    {
        fo << ' ' << s + 1;
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

    void (Doors::*psolve)() =
        (naive ? &Doors::solve_naive : &Doors::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Doors doors(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (doors.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        doors.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
