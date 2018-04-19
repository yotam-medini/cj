#include "lineq.h"
#include <cmath>
#include <limits>
#include <utility>
#include <iostream>
#include <string>

using namespace std;

const lineq_scalar_t lineq_eps = numeric_limits<float>::epsilon();
static const bool lineq_debug = false;

static void output_vector(ostream &o, const vscalar_t &v, const string &t="")
{
    if (t.size() > 0) { o << t << " "; }
    const char *sep = "";
    for (auto x: v)
    {
        o << sep << x;
        sep = "  ";
    }
    o << "\n";
}

static void output_matrix(ostream &o, const lineq_mtx_t &mtx, const string &t="")
{
    if (t.size() > 0) { o << t << "\n"; }
    for (unsigned i = 0; i < mtx.m; ++i)
    {
        const char *sep = "";
        for (unsigned j = 0; j < mtx.n; ++j)
        {
            lineq_scalar_t v = mtx.get(i, j);
            o << sep << v;
            sep = "  ";
        }
        o << '\n';
    }
}

bool matvec_mult(vscalar_t &result, const lineq_mtx_t &m, const vscalar_t &v)
{
    bool ok = (m.n == v.size());
    if (ok)
    {
        result.clear();
        result.reserve(m.m);
        for (unsigned i = 0; i < m.m; ++i)
        {
            lineq_scalar_t resi = 0;
            for (unsigned j = 0; j < m.n; ++j)
            {
                resi += m.get(i, j)*v[j];
            }
            result.push_back(resi);
        }
    }
    return ok;
}

static void eliminate(lineq_mtx_t &aa, vscalar_t &bb, unsigned i, unsigned k)
{
    lineq_scalar_t q = aa.get(k, i);
    lineq_scalar_t dq = 1./q;

    // Swap rows i,k and canonize new i-row
    lineq_scalar_t t = bb[k];
    bb[k] = bb[i];
    bb[i] = t*dq;
    for (unsigned j = i; j < aa.n; ++j)
    {
        t = aa.get(k, j);
        aa.put(k, j, aa.get(i, j));
        t *= dq;
        aa.put(i, j, t);
    }
    aa.put(i, i, 1); // cleaner one instead of q*dq

    lineq_scalar_t bbi = bb[i];
    for (unsigned low = i + 1; low < aa.m; ++low)
    {
        t = aa.get(low, i);
        if (fabs(t) > lineq_eps)
        {
            for (unsigned j = i + 1; j < aa.n; ++j)
            {
                 lineq_scalar_t v = aa.get(low, j);
                 v -= t * aa.get(i, j);
                 aa.put(low, j, v);
            }
        }
        aa.put(low, i, 0);
        bb[low] -= t*bbi;
    }
}

static void diagonalize(lineq_mtx_t &aa, vscalar_t &bb)
{
    if (lineq_debug) {
        output_matrix(cerr, aa, "before"); output_vector(cerr, bb, "before"); }
    for (unsigned j = aa.n; j > 0; )
    {
        --j;
        // zero i-column above j-row
        lineq_scalar_t bbj = bb[j];
        for (unsigned i = 0; i < j; ++i)
        {
            lineq_scalar_t q = aa.get(i, j);
            aa.put(i, j, 0);
            bb[i] -= q*bbj;
        }
    }
    if (lineq_debug) {
        output_matrix(cerr, aa, "after"); output_vector(cerr, bb, "after"); }
}

bool lineq_solve(vscalar_t &x, const lineq_mtx_t &a, const vscalar_t &b)
{
    bool ok = (a.m == a.n) && (a.m == b.size());
    if (ok)
    {
        lineq_mtx_t aa{a.m, a.n}; // copy
        vscalar_t bb(b);
        for (unsigned i = 0; i < a.m; ++i)
        {
            for (unsigned j = 0; j < a.n; ++j)
            {
                aa.put(i, j, a.get(i, j));
            }
        }

        vector<unsigned> perm;
        perm.reserve(a.m);
        for (unsigned i = 0; ok && (i < a.m); ++i)
        {
            perm.push_back(i);
        }
        for (unsigned i = 0; ok && (i < a.m); ++i)
        {
            if (lineq_debug) { cerr << "// i="<<i << "\n";
                output_matrix(cerr, aa); output_vector(cerr, bb, "bb"); }
            // find largest pivot
            unsigned imax = i;
            for (unsigned k = imax + 1; k < a.m; ++k)
            {
                if (fabs(aa.get(imax, i)) < fabs(aa.get(k, i)))
                {
                    imax = k;
                }
            }
            ok = fabs(aa.get(imax, i)) > lineq_eps;
            if (ok)
            {
                eliminate(aa, bb, i, imax);
                swap(perm[i], perm[imax]);
            }
        }
        if (ok)
        {
            diagonalize(aa, bb);
            vscalar_t::size_type sz = a.m;
            vscalar_t::value_type v = 0;
            x = bb;
        }
    }
    return ok;
}
