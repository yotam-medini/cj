#include "kdsegtree.h"
#include <iostream>
#include <cstdlib>

template <int dim>
int specific_main(u_t argc, char **argv)
{
    u_t ai = 0;
    int rc = 1;
    if (string(argv[0]) == string("l"))
    {
        ++ai;
        VMinMaxD<dim> leaves, usegs;
        const string ss("s"), sp("p");
        while ((string(argv[ai]) != ss) && (string(argv[ai]) != sp))
        {
            AU2D<dim> seg;
            for (u_t d = 0; d != dim; ++d)
            {
                for (u_t zo = 0; zo != 2; ++zo, ++ai)
                {
                    seg[d][zo] = stoi(argv[ai]);
                }
            }
            leaves.push_back(seg);
        }
        if (string(argv[ai]) == ss)
        {
            ++ai; // skip "s"
            while (string(argv[ai]) != sp)
            {
                AU2D<dim> seg;
                for (u_t d = 0; d != dim; ++d)
                {
                    for (u_t zo = 0; zo != 2; ++zo, ++ai)
                    {
                        seg[d][zo] = stoi(argv[ai]);
                    }
                }
                usegs.push_back(seg);
            }
        }
        ++ai; // skip "p"
        VD<dim> pts;
        while (ai < argc)
        {
            AUD<dim> pt;
            for (u_t i = 0; i != dim; ++i, ++ai)
            {
                pt[i] = stoi(argv[ai]);
            }
            pts.push_back(pt);
        }
        rc = test<dim>(leaves, pts, usegs);
    }
    return rc;
}

int dim_specific_main(int argc, char **argv)
{
    u_t dim = stoi(argv[0]);
    int rc = 0;
    switch (dim)
    {
     case 1:
        rc = specific_main<1>(argc - 1, argv + 1);
        break;
     case 2:
        rc = specific_main<2>(argc - 1, argv + 1);
        break;
     default:
        cerr << "Unsupported dim=" << dim << '\n';
    }
    return rc;
}

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
    cerr << prog << " [-debug] # ....\n" <<
        indent << "<dim> l <b1> <e1> <b2> <e2> ... p <x1> <x2> ...\n" <<
        indent << "or\n" <<
        indent << "rand <dim> <#tests> <maxval> <#leaves> <#points>\n";
}

int main(int argc, char **argv)
{
    int rc = 0;
    int pai = 0;
    if ((argc > 2) && ((string(argv[1]) == "-debug")))
    {
        kd_debug = true;
        pai = 1;
    }
    if (argc == 1)
    {
        usage(argv[0]);
        rc = 1;
    }
    else if (string(argv[1]) == string("rand"))
    {
        
        rc = rand_main(argc - (2 + pai), argv + (2 + pai));
    }
    else
    {
        rc = dim_specific_main(argc - (1 + pai), argv + (1 + pai));
    }
    cerr << "Test " << (rc == 0 ? "successfull" : "failed") << '\n';
    return rc;
}
