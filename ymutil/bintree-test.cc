#include "bintree.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>

using namespace std;

typedef BinTree<int> bti_t;

void bti_print(bti_t &t, unsigned level=0)
{
    typedef pair<unsigned, unsigned> uu_t;
    typedef vector<uu_t> vuu_t;
    vuu_t hd;
    for (auto i = t.begin(), e = t.end(); i != e; ++i)
    {
        unsigned depth = i.node()->depth();
        unsigned d = *i;
        hd.push_back(uu_t(depth, d));
    }
    for (unsigned depth = 0; depth <= t.height(); ++depth)
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
    cout << "height=" << bti.height() << "\n";
}


void test1()
{
    cout << "== " << __func__ << " ==\n";
    bti_t bti;
    for (int k = 0; k < 10; ++k)
    {
        int n = (7*k) % 10;
        cout << "insert(" << n << ")\n";
        bti.insert((7*k) % 10);
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
    bti_print(bti);
}

int main(int argc, char **argv)
{
    int rc = 0;
    test0();
    test1();
    return rc;
}
