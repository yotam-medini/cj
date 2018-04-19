#include "whmatrix.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    unsigned w = argc > 1 ? strtoul(argv[1], 0, 0) : 2;
    unsigned h = argc > 2 ? strtoul(argv[1], 0, 0) : 3;

    WHMatrix<unsigned> mtx(w, h);
    for (unsigned x = 0, v = 0; x < w; ++x)
    {
        for (unsigned y = 0; y < h; ++y, ++v)
        {
            mtx.put(x, y, v);
        }
    }

    for (unsigned x = 0; x < w; ++x)
    {
        const char *sep = "";
        for (unsigned y = 0; y < h; ++y)
        {
            unsigned v = mtx.get(x, y);
            std:: cout << sep << v;
            sep = "  ";
        }
        std::cout << '\n';
    }

    return 0;
}
