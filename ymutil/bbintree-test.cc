#include "bbintree.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <set>
#include <vector>

using namespace std;

typedef BBinTree<int> bti_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef set<bti_t> set_bti_t;
typedef vector<bti_t> vbti_t;

static void iota(vi_t& a, int n)
{
    a.clear();
    for (int i = 0; i < n; ++i)
    {
        a.push_back(i);
    }
}

static bool bti_check_parents(const bti_t &t)
{
    bool ok = true;
    vector<int> v;
    for (auto i = t.begin(), e = t.end(); ok && i != e; ++i)
    {
        auto node = i.node();
        for (unsigned ci = 0; ok && ci != 2; ++ci)
        {
            auto child = node->child[ci];
            if (child && child->parent != node)
            {
                ok = false;
                cerr << "Bad parent/child relation\n";
            }
        }
    }
    return ok;
}

static bool bti_check_order(const bti_t &t)
{
    bool ok = true;
    vector<int> v;
    for (auto i = t.begin(), e = t.end(); i != e; ++i)
    {
        v.push_back(*i);
    }
    vector<int> vs(v);
    sort(vs.begin(), vs.end());
    if (vs != v)
    {
        cerr << "tree is not sorted\n";
        ok = false;
    }
    return ok;
}

static bool bti_ok(const bti_t &t)
{
    bool ok;
    ok = bti_check_parents(t);
    ok = ok && bti_check_order(t);
    if (ok && !t.balanced())
    {
        cerr << "imbalanced\n";
        ok = false;
    }
    if (ok && !t.valid_bf())
    {
        cerr << "invalid balanced factor\n";
        ok = false;
    }
    return ok;
}

static void bti_print(const bti_t &t, unsigned level=0)
{
    typedef pair<unsigned, unsigned> uu_t;
    typedef vector<uu_t> vuu_t;
    vuu_t hd;
    for (auto i = t.begin(), e = t.end(); i != e; ++i)
    {
        // unsigned depth = i.node()->depth();
        bti_t::node_ptr_t node = i.node();
        unsigned depth = node->depth();
        unsigned d = *i;
        hd.push_back(uu_t(depth, d));
    }
    for (unsigned depth = 0; depth < t.height(); ++depth)
    {
        cout << "[D="<< depth <<"]:";
        for (auto i = hd.begin(), e = hd.end(); i != e; ++i)
        {
            unsigned idepth = (*i).first;
            if (idepth == depth)
            {
                cout << " " << setw(3) << (*i).second;
            }
            else
            {
                cout << "    ";
            }
        }
        cout << "\n";
    }
}

static bool test0()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    bti.insert(7);
    bti_t::const_iterator i = bti.begin();
    bti_t::const_iterator e = bti.end();
    while (i != e)
    {
        const int& n = *i;
        cout << n << "\n";
        ++i;
    }
    cout << "height=" << bti.height() << ", bf-valid=" << bti.valid_bf() <<"\n";
    return true;
}


static bool test1()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    for (int k = 0; k < 10; ++k)
    {
        int n = (7*k) % 10;
        cout << "insert(" << n << ")\n";
        bti.insert(n);
        bti_check_parents(bti);
        bti_check_order(bti);
        if (!bti.balanced()) { cout << " Not balanced\n"; }
        if (!bti.valid_bf()) { cout << " Invalid Balanced Factor\n"; }
    }
    bti_t::const_iterator i = bti.begin();
    bti_t::const_iterator e = bti.end();
    cout << "all:\n";
    while (i != e)
    {
        const int& n = *i;
        cout << n << "\n";
        ++i;
    }
    cout << "height=" << bti.height() << "\n";
    cout << "balanced?=" << bti.balanced() << "\n";
    cout << "valid_bf=" << bti.valid_bf() << "\n";
    bti_print(bti);
    return true;
}

static bool test2()
{
    cout << "== " << __func__ << " ==\n";
    for (int r = -1; r <= 11; ++r)
    {
        bti_t bti;
        for (int k = 0; k < 10; ++k)
        {
            int n = (7*k) % 10;
            bti.insert(n);
        }
        bti_t::const_iterator w = bti.find(r);
        bti_t::const_iterator e = bti.end();
        bool found = w != e;
        cout << "r=" << r << ", found?=" << found << "\n";
        if (found)
        {
            cout << "remove: " << r << "\n";
            bti.remove(r);
            bti_print(bti);
        }
    }
    return true;
}

static bool test_vec(set_bti_t& btis, const vi_t& a, bool debug=false)
{
    bool ok;
    bti_t bti;
    for (auto i = a.begin(), e = a.end(); i != e; ++i)
    {
        int k = *i;
        if (debug && ((e - i) == 1))
        {
            bti_print(bti);
        }
        bti.insert(k);
    }
    ok = bti_ok(bti);
    if (ok)
    {
        btis.insert(std::move(bti));
    }
    else
    {
        const char *sep = "";
        cout << "a={";
        for (auto i = a.begin(), e = a.end(); i != e; ++i)
        {
            cout << sep << *i;
            sep = ", ";
        }
        cout << "}\n";
        bti_print(bti);
    }
    return ok;
}

static bool test_removal_special()
{
    static const int g[] = {0, 1, 2, 4, 5, 6, 3};
    bti_t bti;

    for (int k = 0; k < 7; ++k) { bti.insert(g[k]); }
    bool ok = bti_ok(bti);
    if (ok)
    {
        bti_print(bti);
        bti.remove(5);
        bti_print(bti);
        ok = bti_ok(bti);
    }
    return ok;
}

static bool test_removals(int n, const vvi_t& generators)
{
    cout << "== " << __func__ << "(" << n << ") ==\n";
    bool ok = true;
    for (auto gi = generators.begin(), ge = generators.end(); ok && (gi != ge);
        ++gi)
    {
        const vi_t& g = *gi;

        vi_t a;
        iota(a, n);
        for (int r = 0; ok && (r < n); ++r)
        {
            bti_t bti;
            for (auto i = g.begin(), e = g.end(); i != e; ++i)
            {
                int k = *i;
                bti.insert(k);
            }
            bti.remove(r);
            ok = bti_ok(bti);
        }
    }
    return ok;
}

static bool test_permute(int n)
{
    cout << "== " << __func__ << "(" << n << ") ==\n";
    vi_t a;
    iota(a, n);

    set_bti_t btis;
    vvi_t generators;
    bool ok = true, more = true;
    unsigned n_perms = 0;
    unsigned btis_size = 0;
    while (ok && more)
    {
        ++n_perms;
        ok = test_vec(btis, a);
        if (ok)
        {
            unsigned btis_size_new = btis.size();
            if (btis_size != btis_size_new)
            {
                btis_size = btis_size_new;
                generators.push_back(a);
            }
        }
        else
        {
            test_vec(btis, a, true); // for debugger
        }
        more = next_permutation(a.begin(), a.end());
    }
    cout << "n="<<n << ", #(permutations)="<< n_perms << 
        ", #(btis)="<< btis.size() << "\n";
    ok = ok && test_removals(n, generators);

    return ok;
}

static bool test_permutations()
{
    cout << "== " << __func__ << " ==\n";
    bool ok = true;
    for (int n = 1; ok && n < 10; ++n)
    {
        ok = test_permute(n);
    }
    return ok;
}

static bool test_remove_case1l()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    bti.insert(1);
    bti.insert(0);
    bti.remove(0);
    return bti_ok(bti);
}

static bool test_remove_case1r()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    bti.insert(0);
    bti.insert(1);
    bti.remove(1);
    return bti_ok(bti);
}

static bool test_remove_case2(int rn)
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    bti.insert(1);
    bti.insert(0);
    bti.insert(2);
    bti.remove(rn);
    return bti_ok(bti);
}

static bool test_remove_case3(int ngg, bool rev)
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    vi_t inserts;
    inserts.push_back(0);
    for (auto i = inserts.begin(), e = inserts.end(); i != e; ++i)
    {
        bti.insert(*i);
    }
    return bti_ok(bti);
}

int main(int argc, char **argv)
{
    int rc = 0;
    bool ok = true;

    ok = ok && test_removal_special();
    ok = ok && test_permutations();
    ok = ok && test2();
    ok = ok && test_remove_case1l();
    ok = ok && test_remove_case1r();
    ok = ok && test_remove_case2(0);
    ok = ok && test_remove_case2(2);
    ok = ok && test_remove_case3(0, false);
    ok = ok && test0();
    ok = ok && test1();
    rc = ok ? 0 : 1;
    return rc;
}
