#include "kdsegtree.h"
#include <iostream>
#include <cstdlib>

template <int dim>
int rand_test(u_t maxval, u_t n_segs, u_t n_pts)
{
    VMinMaxD<dim> segs;
    while (segs.size() < n_segs)
    {
        AU2D<dim> seg;
        for (u_t d = 0; d != dim; ++d)
        {
            for (u_t zo: {0, 1})
            {
                seg[d][zo] = rand() % (maxval + 1);
            }
            u_t v0 = rand() % maxval;
            seg[d][0] = v0;
            seg[d][1] = v0 + (rand() % (maxval + 1 - v0));
        }
        segs.push_back(seg);
    }
    VD<dim> pts;
    while (pts.size() < n_pts)
    {
        AUD<dim> pt;
        for (u_t d = 0; d != dim; ++d)
        {
            pt[d] = rand() % (maxval + 1);
        }
        pts.push_back(pt);
    }
    int rc = test<dim>(segs, pts);
    return rc;
}

static int rand_main(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    u_t dim = stoi(argv[ai++]);
    u_t n_tests = stoi(argv[ai++]);
    u_t maxval = stoi(argv[ai++]);
    u_t n_segs = stoi(argv[ai++]);
    u_t n_pts = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti != n_tests); ++ti)
    {
    switch (dim)
    {
     case 1:
        rc = rand_test<1>(maxval, n_segs, n_pts);
        break;
     case 2:
        rc = rand_test<2>(maxval, n_segs, n_pts);
        break;
     default:
        cerr << "Unsupported dim=" << dim << '\n';
    }

    }
    return rc;
}

static void usage(const string& prog)
{
    const string indent(prog.size() + 2, ' ');
    cerr << prog << " # ....\n" <<
        indent << "<dim> s <b1> <e1> <b2> <e2> ... p <x1> <x2> ...\n" <<
        indent << "or\n" <<
        indent << "rand <dim> <#tests> <maxval> <#segs> <#points>\n";
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc == 1)
    {
        usage(argv[0]);
        rc = 1;
    }
    else if (string(argv[1]) == string("rand"))
    {
        cerr << "rand: not yet\n";
        rc = rand_main(argc - 2, argv + 2);
    }
    else
    {
        rc = dim_specific_main(argc - 1, argv + 1);
    }
    cerr << "Test " << (rc == 0 ? "successfull" : "failed") << '\n';
    return rc;
}
