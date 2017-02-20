#include "bintree.h"
#include <iostream>

using namespace std;

typedef BinTree<int> bti_t;

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
}

int main(int argc, char **argv)
{
    int rc = 0;
    test0();
    test1();
    return rc;
}
