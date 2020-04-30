#include "bipartite_match.h"
#include <iostream>
#include <cstdlib>
using namespace std;

static void usage(const char *pn)
{
    cerr << "Usage: " << pn << " <l0 r0> <l1 r1> ...\n";
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && ((argc -1) % 2) == 0) ? 0 : 1;
    if (rc == 0)
    {
        lr_edges_t edges;
        for (int ai = 1; ai < argc; ai += 2)
        {
            unsigned l = strtoul(argv[ai + 0], 0, 0);
            unsigned r = strtoul(argv[ai + 1], 0, 0);
            au2_t e{l, r};
            edges.insert(e);
        }
        lr_edges_t match;
        int n = bipartitee_max_match(match, edges);
        if (n >= 0)
        {
            cout << "matched: " << n << "\n";
            for (const auto e: match)
            {
                cout << "  " << e[0] << " " << e[1] << "\n";
            }
        }
        else
        {
            cerr << "bipartitee_max_match failed\n";
            rc = 1;
        }
    }
    if (rc) { usage(argv[0]); }
    return rc;
}
