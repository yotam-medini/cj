#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;

u_t max_bounded_rect_naive(const vu_t& v, vuu_t& be)
{
    u_t max_rect = 0;
    u_t sz = v.size();
    vu_t::const_iterator vb = v.begin();
    for (u_t bi = 0; bi < sz; ++bi)
    {
        for (u_t ei = bi + 1; ei <= sz; ++ei)
        {
            vu_t::const_iterator imin = min_element(vb + bi, vb + ei);
            u_t vi = *imin;
            u_t rect = vi * (ei - bi);
            if (max_rect <= rect)
            {
                if (max_rect < rect)
                {
                    max_rect = rect;
                    be.clear();
                }
                be.push_back(uu_t(bi, ei));
            }
        }
    }
    return max_rect;
}

#if 0
class MaxBoundRect
{
 public:
    MaxBoundRect(const vu_t& _v) : v(_v), result(0), bi(0), ei(0)
    {
        solve();
    }
    u_t get(u_t* pbi=0, u_t* pei=0) const
    {
        u_t dum;
        *(pbi ? : &dum) = bi;
        *(pei ? : &dum) = ei;
        return result;
    }
 private:
    class Node
    {
     public:
        Node(u_t b = 0, u_t i=0, u_t w=0) : ib(b), idx(i), wake(w) {}
        u_t ib;  // X-left of rectangle
	u_t idx; // height v[idx]
	u_t wake;
    };
    typedef vector<vu_t> vvu_t;
    typedef vector<Node> vnode_t;
    void solve();
    void check_push_node(u_t i, u_t candid_ib);
    void push_node(const Node& node)
    {
        wake_stacks[node.wake].push_back(inc_nodes.size());
        inc_nodes.push_back(node);
    }
    void pop_node()
    {
        const Node& top = inc_nodes.back();
        wake_stacks[top.wake].pop_back();
        inc_nodes.pop_back();
    }
    const vu_t &v;
    u_t result;
    u_t bi;
    u_t ei;
    vnode_t inc_nodes; // indices to v
    vvu_t wake_stacks;
    u_t active_node;
};

void MaxBoundRect::solve()
{
    bi = 0;
    ei = 1;
    u_t sz = v.size();
    wake_stacks = vvu_t(vvu_t::size_type(sz ? : 1), vu_t());
    active_node = 0;
    for (u_t i = 0; i < sz; ++i)
    {
        static const Node znode(0, 0, 0);
        u_t y = v[i];
        u_t next_ib = i;
        while ((!inc_nodes.empty()) && v[inc_nodes.back().idx] > y)
        {
            const Node& last = inc_nodes.back();
            next_ib = last.ib;
            pop_node();
        }
        if (active_node >= inc_nodes.size())
        {
            active_node = (inc_nodes.empty() ? 0 : inc_nodes.size() - 1);
        }
        const Node &pre_node = inc_nodes.empty() ? znode : inc_nodes.back();
        if (inc_nodes.empty() || (y > v[pre_node.idx]))
        {
            if (inc_nodes.empty())
            {
                push_node(Node(next_ib, i, i));
            }
            else
            {
                check_push_node(i, next_ib);
            }
        }
        if (!wake_stacks[i].empty())
        {
            active_node = wake_stacks[i].back();
        }
        const Node& wake_node = inc_nodes[active_node];
        u_t ib = wake_node.ib;
        u_t currh = v[wake_node.idx];
	u_t rect = currh * (i + 1 - ib);
	if (result < rect)
	{
	    result = rect;
	    bi = ib;
	    ei = i + 1;
	}
    }
}

void MaxBoundRect::check_push_node(u_t i, u_t candid_ib)
{
    if (inc_nodes.empty())
    {
        push_node(Node(candid_ib, i, i));
    }
    else
    {
        const Node& pre_node = inc_nodes.back(); // [active_node];
        // assume v[pre_node.idx] < v[i]
        u_t x0 = pre_node.ib;
        u_t y0 = v[pre_node.idx];
        u_t x1 = candid_ib;
        u_t y1 = v[i];
        // find x0 < x1, y0 < y1. Find first x such that
        // (x + 1 - x1)*y1 > (x + 1 - x0)*y0
        u_t dy = y1 -y0;
        u_t x = ((x1 - 1)*y1 + y0 + (dy) - x0*y0) / dy; // next wake
        while (false && (!inc_nodes.empty()) && (x < inc_nodes.back().wake))
        {
            pop_node();
        }
        if (x < v.size())
        {
            if (x < i)
            {
                active_node = inc_nodes.size();
            }
            push_node(Node(candid_ib, i, x));
        }
    }
}

u_t OLDmax_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    return MaxBoundRect(v).get(pbi, pei);
}

static void maximize(u_t& v, u_t by)
{
    if (v < by)
    {
        v = by;
    }
}
#endif

class MaxBoundRect
{
 public:
    MaxBoundRect(const vu_t& _v) : v(_v), result(0), bi(0), ei(0)
    {
        solve();
    }
    u_t get(u_t* pbi=0, u_t* pei=0) const
    {
        u_t dum;
        *(pbi ? : &dum) = bi;
        *(pei ? : &dum) = ei;
        return result;
    }
 private:
    void solve();
    u_t top() const { return inc_stack.empty() ? 0 : v[inc_stack.back()]; }
    void pop(u_t ie);
    const vu_t &v;
    u_t result;
    u_t bi;
    u_t ei;
    vu_t inc_stack; // indices to v
};

void MaxBoundRect::solve()
{
    u_t sz = v.size();
    bi = 0;
    ei = 0;
    
    for (u_t i = 1; i < sz; ++i)
    {
        u_t y = v[i];
        if (top() < y)
        {
            inc_stack.push_back(i);
        }
        while (y < top())
        {
            pop(i);
        }
    }
    while (!inc_stack.empty())
    {
        pop(sz);
    }
}

void MaxBoundRect::pop(u_t ie)
{
    u_t back = inc_stack.back();
    u_t dx = ie - back;
    u_t yback = v[back];
    u_t rect = dx*yback;
    if (result < rect)
    {
        result = rect;
        bi = back;
        ei = ie;
    }
    inc_stack.pop_back();
}

u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    return MaxBoundRect(v).get(pbi, pei);
}

#include <cstdlib>

static int test_fast_naive(const vu_t &v)
{
    int rc = 0;
    u_t mr, bi, ei;
    vuu_t naive_be;
    mr = max_bounded_rect(v, &bi, &ei);
    u_t mr_naive;
    mr_naive = max_bounded_rect_naive(v, naive_be);
    if (mr != mr_naive ||
        (find(naive_be.begin(), naive_be.end(), uu_t(bi, ei)) ==
         naive_be.end()))
    {
        cerr <<
            "Naive: R="<<mr_naive << ", be: {";
        for (const uu_t& be: naive_be)
        {
            cerr << "[" << be.first << ", " << be.second << "), ";
        }
        cerr <<"}\n"
            "Fast:  R="<<mr << ", ["<<bi << ", "<<ei << ")\n";
        rc = 1;
    }
    return rc;
}

static int test_explicit(int argc, char** argv)
{
    vu_t v;
    for (int ai = 0; ai < argc; ++ai)
    {
        v.push_back(strtoul(argv[ai], 0, 0));
    }
    int rc = test_fast_naive(v);
    return rc;
}

static int test_rand(int argc, char** argv)
{
    u_t ai = 0;
    u_t n = strtoul(argv[ai++], 0, 0);
    u_t sz = strtoul(argv[ai++], 0, 0);
    u_t M = strtoul(argv[ai++], 0, 0);
    vu_t v(vu_t::size_type(sz), 0);
    int rc = 0;
    for (u_t ti = 0; (ti < n) && (rc == 0); ++ti)
    {
        for (u_t i = 0; i < sz; ++i)
        {
            v[i] = rand() % (M + 1);
        }
        rc = test_fast_naive(v);
        if (rc != 0)
        {
            cerr << "... explicit";
            for (u_t x: v)
            {
                cerr << ' ' << x;
            }
            cerr << '\n';
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    string cmd = (argc > 1 ? argv[1] : "");
    if (cmd == string("explicit"))
    {
        rc = test_explicit(argc - 2, argv + 2);
    }
    else if (cmd == string("rand"))
    {
        rc = test_rand(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Bad command: '" << cmd << "'\n";
        rc = 1;
    }
    return rc;
}
