#include "interval_tree.h"
#include <iostream>
#include <string>

using namespace std;

typedef IntervalTree<int, int> intree_t;
typedef intree_t::interval_t interval_t;
typedef intree_t::node_ptr_t node_ptr_t;

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
            os << "\n";
        }
    }
}

static void it_print(const intree_t &intree, ostream &os=cout)
{
    const node_ptr_t root = intree.get_root();
    it_subpr(root, os, "");
}

int main(int argc, char **argv)
{
    IntervalTree<int, int> intree;
    intree.insert(2, 3);
    intree.insert(0, 1);
    intree.insert(4, 5);
    it_print(intree);

    return 0;
}
