#include "matrix.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    unsigned m = argc > 1 ? strtoul(argv[1], 0, 0) : 2;
    unsigned n = argc > 2 ? strtoul(argv[1], 0, 0) : 3;

    Matrix<unsigned> mtx(m, n);
    for (unsigned i = 0, v = 0; i < m; ++i)
    {
        for (unsigned j = 0; j < n; ++j, ++v)
        {
            mtx.put(i, j, v);
        }
    }

    for (unsigned i = 0; i < m; ++i)
    {
        const char *sep = "";
        for (unsigned j = 0; j < n; ++j)
        {
            unsigned v = mtx.get(i, j);
            std:: cout << sep << v;
            sep = "  ";
        }
        std::cout << '\n';
    }

    return 0;
}
