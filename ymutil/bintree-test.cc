#include "bintree.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm>

using namespace std;

typedef BinTree<int> bti_t;

static void bti_check_parents(bti_t &t)
{
    vector<int> v;
    for (auto i = t.begin(), e = t.end(); i != e; ++i)
    {
        auto node = i.node();
        for (unsigned ci = 0; ci != 2; ++ci)
        {
            auto child = node->child[ci];
            if (child && child->parent != node)
            {
                cerr << "Bad parent/child relation\n";
            }
        }
    }
}

static void bti_check_order(bti_t &t)
{
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
    }
}

static void bti_print(bti_t &t, unsigned level=0)
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

void test0()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    bti.insert(7);
    bti_t::iterator i = bti.begin();
    bti_t::iterator e = bti.end();
    while (i != e)
    {
        int& n = *i;
        cout << n << "\n";
        ++i;
    }
    cout << "height=" << bti.height() << ", bf-valid=" << bti.valid_bf() << "\n";
}


static void test1()
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
    bti_t::iterator i = bti.begin();
    bti_t::iterator e = bti.end();
    cout << "all:\n";
    while (i != e)
    {
        int& n = *i;
        cout << n << "\n";
        ++i;
    }
    cout << "height=" << bti.height() << "\n";
    cout << "balanced?=" << bti.balanced() << "\n";
    cout << "valid_bf=" << bti.valid_bf() << "\n";
    bti_print(bti);
}

static void test2()
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
        bti_t::iterator w = bti.find(r);
        bool found = w != bti.end();
        cout << "r=" << r << ", found?=" << found << "\n";
        if (found)
        {
            cout << "remove: " << r << "\n";
            bti.remove(r);
            bti_print(bti);
        }
    }
}

int main(int argc, char **argv)
{
    int rc = 0;
    test0();
    test1();
    test2();
    return rc;
}
