#include "interval_tree.h"
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace std;

typedef IntervalTree<int, int> intree_t;
typedef intree_t::interval_t interval_t;
typedef intree_t::iterator ititer_t;
typedef intree_t::node_ptr_t node_ptr_t;

typedef vector<unsigned> vu_t;
typedef vector<interval_t> vinterval_t;

static void it_subpr(const node_ptr_t p, ostream &os, const string &indent)
{
    const intree_t::node_t &node = *p;
    const interval_t &iti = node.data;
    os << indent <<
        "{[" << iti.l << ", " << iti.r << ") max=" << iti.rmax << "}\n";
    const string sub_indent = indent + "  ";
    int i0 = (node.child[0] || node.child[1] ? 0 : 2);
    for (int i = i0; i < 2; ++i)
    {
        if (node.child[i])
        {
            it_subpr(p->child[i], os, sub_indent);
        }
        else
        {
            os << sub_indent << "--\n";
        }
    }
}

static void it_print(const intree_t &intree, const string &msg="", 
    ostream &os=cout)
{
    if (msg != "") { os << msg << '\n'; }
    const node_ptr_t root = intree.get_root();
    if (root)
    {
        it_subpr(root, os, "");
    }
    else
    {
        os << "intree empty\n";
    }
}

static bool it_check_rmax(node_ptr_t p)
{
    bool ok = true;
    intree_t::value_t max_r = p->data.r;
    for (int ci = 0; ok && (ci < 2); ++ci)
    {
        node_ptr_t pc = p->child[ci];
        if (pc)
        {
            ok = it_check_rmax(pc);
            if (max_r < pc->data.rmax)
            {
                max_r = pc->data.rmax;
            }
        }
    }
    ok = ok && (max_r == p->data.rmax);
    return ok;
}

static int test0()
{
    int rc = 0;
    intree_t intree;
    ititer_t i23 = intree.insert(2, 3);
    intree.insert(0, 1);
    intree.insert(4, 5);
    it_print(intree);
    if (!it_check_rmax(intree.get_root()))
    {
        cerr << "Failed: " << __func__ << ':' << __LINE__ << '\n';
        rc = 1;
    }
    intree.remove(i23);
    it_print(intree);
    if (!it_check_rmax(intree.get_root()))
    {
        cerr << "Failed: " << __func__ << ':' << __LINE__ << '\n';
        rc = 1;
    }
    return rc;
}

static unsigned rand_range(unsigned b, unsigned e)
{
    unsigned r = rand();
    unsigned ret = b + r % (e - b);
    return ret;
}

static bool intersect(const interval_t& x, const interval_t& y)
{
    // bool disj = (x.r <= y.l) || (y.r <= x.l);
    bool ret = (y.l < x.r) && (x.l < y.r);
    return ret;
}

static int search_check(const intree_t& intree, int M,
    const vinterval_t &remain)
{
    int rc = 0;
    for (int l = 0; (l < M) && (rc == 0); ++l)
    {
        for (int r = l + 1; (r < M) && (rc == 0); ++r)
        {
            interval_t x(l, r);
            set<interval_t> expected;
            for (const interval_t &y: remain)
            {
                if (intersect(x, y))
                {
                    expected.insert(y);
                }
            }
            vector<interval_t> found;
            intree.search(found, x);
            set<interval_t> sfound(found.begin(), found.end());
            if (sfound != expected)
            {
                cerr << "Search failed: expected=" << expected.size() <<
                    ", found=" << found.size() << 
                    "  x=[" << x.l << ", " << x.r << ")\n";
                rc = 1;
            }
        }
    }
    return rc;
}

static int add_remove(unsigned M, const vinterval_t &intervals,
    const vu_t& remis)
{
    int rc = 0;
    intree_t intree;
    vector<ititer_t> iters;
    for (unsigned ii = 0; ii < intervals.size(); ++ii)
    {
        iters.push_back(intree.insert(intervals[ii]));
    }
    it_print(intree, "after insertions");
    if (!it_check_rmax(intree.get_root()))
    {
        cerr << "Failed: (rmax)" << __func__ << ':' << __LINE__ << '\n';
        rc = 1;
    }
    for (unsigned ii: remis)
    {
        ititer_t iter = iters[ii];
        intree.remove(iter);
    }
    it_print(intree, "after deletions");
    node_ptr_t root = intree.get_root();
    if (root && !it_check_rmax(root))
    {
        cerr << "Failed (rmax): " << __func__ << ':' << __LINE__ << '\n';
        rc = 1;
    }
    if (rc == 0)
    {
        vinterval_t remain;
        for (unsigned ii = 0; ii < intervals.size(); ++ii)
        {
            if (find(remis.begin(), remis.end(), ii) == remis.end())
            {
                remain.push_back(intervals[ii]);
            }
        }
        rc = search_check(intree, M, remain);
    }
    return rc;
}

static int write_add_remove(unsigned M, const vinterval_t &intervals,
    const vu_t& remis)
{
    cout << "add_remove " << M << "  " << intervals.size();
    for (const interval_t &i: intervals)
    {
        cout << "  " << i.l << ' ' << i.r;
    }
    cout << "  " << remis.size() << ' ';
    for (unsigned ii: remis)
    {
        cout << ' ' << ii;
    }
    cout << '\n';
    return add_remove(M, intervals, remis);
}

static int test_add_remove(int argc, char **argv)
{
    int ai = 0;
    unsigned M = strtoul(argv[ai++], 0, 0);
    unsigned na = strtoul(argv[ai++], 0, 0);
    vinterval_t intervals;
    for (unsigned ii = 0; ii < na; ++ii)
    {
        int l = strtoul(argv[ai++], 0, 0);
        int r = strtoul(argv[ai++], 0, 0);
        intervals.push_back(interval_t(l, r));
    }
    unsigned nr = strtoul(argv[ai++], 0, 0);
    vu_t remis;
    for (unsigned ii = 0; ii < nr; ++ii)
    {
        remis.push_back(strtoul(argv[ai++], 0, 0));
    }
    return add_remove(M, intervals, remis);
}

static int test_random1(unsigned M, unsigned n_intervals)
{
    int rc = 0;
    vector<interval_t> intervals;
    while (intervals.size() < n_intervals)
    {
        interval_t::value_t l = rand_range(0, M);
        interval_t::value_t r = rand_range(l + 1, M + 1);
        interval_t i(l, r);
        if (find(intervals.begin(), intervals.end(), i) == intervals.end())
        {
            intervals.push_back(i);
        }
    }
    unsigned nrem = rand_range(0, n_intervals + 1);
    vu_t all(vu_t::size_type(n_intervals), 0);
    iota(all.begin(), all.end(), 0);
    vu_t remis;
    for (unsigned ri = 0; ri < nrem; ++ri)
    {
        unsigned k = rand_range(0, all.size());
        remis.push_back(all[k]);
        swap(all[k], all.back());
        all.pop_back();
    }
    rc = write_add_remove(M, intervals, remis);
    return rc;
}

static int test_random(int argc, char **argv)
{
    int rc = 0;
    unsigned n_tests = strtoul(argv[0], 0, 0);    
    unsigned M = strtoul(argv[1], 0, 0);
    unsigned n_intervals = strtoul(argv[2], 0, 0);
    for (unsigned ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        rc = test_random1(M, n_intervals);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc == 1)
    {
        rc = test0();
    }
    else 
    {
        string test_name(argv[1]);
        if (test_name == string("random"))
        {
            rc = test_random(argc - 2, argv + 2);
        }
        else if (test_name == string("add_remove"))
        {
            rc = test_add_remove(argc - 2, argv + 2);
        }
        else
        {
            cerr << "unknown test: " << test_name << "\n";
            rc = 1;
        }
    }
    return rc;
}
