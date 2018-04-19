#include "lineq.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

int main(int argc, char **argv)
{
    int rc = 1;
    int ai = 1;
    unsigned n = strtoul(argv[ai++], 0, 0);
    lineq_mtx_t a{n, n};
    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = 0; j < n; ++j)
        {
            lineq_scalar_t aij = strtod(argv[ai++], 0);
            a.put(i, j, aij);
        }
    }
    vscalar_t b;
    for (unsigned i = 0; i < n; ++i)
    {
        lineq_scalar_t bi = strtod(argv[ai++], 0);
        b.push_back(bi);
    }
    vscalar_t x;
    bool solved = lineq_solve(x, a, b);
    if (solved)
    {
        const char *sep = "";
        for (auto xi: x)
        {
            cout << sep << xi;
            sep = " ";
        }
        cout << "\n";
        vscalar_t ax;
        bool ok = matvec_mult(ax, a, x);
        if (ok)
        {
            lineq_scalar_t error = 0;
            for (unsigned i = 0; i < n; ++i)
            {
                lineq_scalar_t erri = fabs(ax[i] - b[i]);
                if (error < erri) { error = erri; }
            }
            cout << "error: " << error << "\n";
            if (error < lineq_eps)
            {
                rc = 0;
            }
        }
    }
    return rc;
}

