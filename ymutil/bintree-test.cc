#include "bintree.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    int rc = 0;
    typedef BinTree<int> bti_t;
    bti_t bti;
    bti.insert(7);
    bti_t::iterator i = bti.begin();
    bti_t::iterator e = bti.end();
    while (i != e)
    {
        int& n = *i;
        cout << n << "\n";
    }
    return rc;
}
