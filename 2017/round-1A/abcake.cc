// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <boost/numeric/ublas/matrix.hpp>

using namespace std;

// typedef mpz_class mpzc_t;
// typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
typedef pair<unsigned, unsigned> uu_t;
typedef vector<uu_t> vuu_t;
typedef boost::numeric::ublas::matrix<char> mtx_c;

static unsigned dbg_flags;

enum { MaxGrid = 25 };

class Kid
{
 public:
    Kid(unsigned vi=0, unsigned vj=0): 
        i(vi), j(vj), 
        rmin(vi), rmax(vi), 
        cmin(vj), cmax(vj)
    {}
    void single()
    {
         rmin =  rmax = i;
         cmin =  cmax = j;
    }
    void add(unsigned vi, unsigned vj)
    {
        if (rmin > vi)
        {
            rmin = vi;
        }
        if (rmax < vi)
        {
            rmax = vi;
        }
        if (cmin > vj)
        {
            cmin = vj;
        }
        if (cmax < vj)
        {
            cmax = vj;
        }
    }
    unsigned size() const
    {
        unsigned nr = (rmax - rmin) + 1;
        unsigned nc = (cmax - cmin) + 1;
        return nr * nc;
    }
    unsigned i, j; 
    unsigned rmin, rmax, cmin, cmax;
};
typedef map<char, Kid> c2kid_t;

class Rect
{
 public:
    Rect(unsigned vrb, unsigned vre, unsigned vcb, unsigned vce) :
        rb(vrb),
        re(vre),
        cb(vcb),
        ce(vce)
        {}
   unsigned rb, re, cb, ce;
};

class ABCake
{
 public:
    ABCake(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void print_grid(ostream &fo) const;
    bool fill(unsigned ei);
    void kids_set();
    bool kids_ok() const;
    bool disjoint(const Kid &k0, const Kid &k1) const;
    void fill_grid();
    unsigned first_row_kid(unsigned ri) const;
    vuu_t empty;
    unsigned r, c;
    mtx_c grid;
    c2kid_t kids;
    c2kid_t kids_big;
    unsigned check_count;
};

ABCake::ABCake(istream& fi)
{
    fi >> r >> c;
    grid = mtx_c(r, c);
    for (unsigned ri = 0; ri < r; ++ri)
    {
        string s;
        fi >> s;
        for (unsigned ci = 0; ci < c; ++ci)
        {
            grid(ri, ci) = s[ci];
        }
    }
}

void ABCake:: print_grid(ostream& fo) const
{
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            fo << grid(ri, ci);
        }
        fo << "\n";
    }
}

void ABCake::kids_set()
{
    kids_big = kids;
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            char ckid = grid(ri, ci);
            if (ckid == '?')
            {
                cerr << "error\n";
            }
            else
            {
                Kid &kid = kids_big[ckid];
                if (kid.rmin > ri)
                {
                    kid.rmin = ri;
                }
                if (kid.rmax < ri)
                {
                    kid.rmax = ri;
                }
                if (kid.cmin > ci)
                {
                    kid.cmin = ci;
                }
                if (kid.cmax < ci)
                {
                    kid.cmax = ci;
                }
            }
        }
    }
}

bool ABCake::disjoint(const Kid &kid0, const Kid &kid1) const
{
    bool ret = 
        (kid0.rmax < kid1.rmin) ||
        (kid1.rmax < kid0.rmin) ||
        (kid0.cmax < kid1.cmin) ||
        (kid1.cmax < kid0.cmin);
    return ret;
}

bool ABCake::kids_ok() const
{
    bool ok = true;
    
    for (c2kid_t::const_iterator i = kids_big.begin(), e = kids_big.end(); 
        (i != e) && ok; ++i)
    {
        const Kid &kid0 = (*i).second;
        for (c2kid_t::const_iterator i1 = kids_big.begin(); 
             (i1 != e) && ok; ++i1)
        {
            if (i != i1)
            {
                const Kid &kid1 = (*i1).second;
                ok = disjoint(kid0, kid1);
            }
        }
    }
    if (dbg_flags & 0x4)
    {
        cerr << "check_count="<<check_count << ", ok="<<ok << "\n";
        print_grid(cerr);
    }
    return ok;
}

bool ABCake::fill(unsigned ei)
{
    bool ret = false;
    if (ei == empty.size())
    {
        kids_set();
        ++check_count;
        ret = kids_ok(); 
    }
    else
    {
        uu_t rc = empty[ei];
        unsigned ri = rc.first;
        unsigned cj = rc.second;
        for (c2kid_t::const_iterator i = kids.begin(), e = kids.end(); 
            (i != e) && !ret; ++i)
        {
            char ckid = (*i).first;
            grid(ri, cj) = ckid;
            if (dbg_flags & 0x8)
            {
                cerr << "ri="<<ri << ", cj="<<cj << ":\n";
                print_grid(cerr);
            }
            ret = fill(ei + 1);
        }
        if (!ret)
        {
            grid(ri, cj) = '?';
        }
    }
    return ret;
}

void ABCake::solve_naive()
{
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            char ckid = grid(ri, ci);
            if (ckid == '?')
            {
                empty.push_back(uu_t(ri, ci));
            }
            else
            {
                c2kid_t::value_type v(ckid, Kid(ri, ci));
                kids.insert(kids.end(), v);
            }
        }
    }
    check_count = 0;
    bool filled = fill(0);
    if (!filled)
    {
        cerr << "failure\n";
        exit(1);
    }
}

void ABCake::fill_grid()
{
    
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            grid(ri, ci) = '?';
        }
    }
    for (c2kid_t::const_iterator i = kids.begin(), e = kids.end(); i != e; ++i)
    {
        const c2kid_t::value_type &v = (*i);
        char ckid = v.first;
        const Kid &kid = (*i).second;
        for (unsigned ri = kid.rmin; ri <= kid.rmax; ++ri)
        {
            for (unsigned cj = kid.cmin; cj <= kid.cmax; ++cj)
            {
                grid(ri, cj) = ckid;
            }
        }
    }

}

unsigned ABCake::first_row_kid(unsigned ri) const
{
    unsigned ci;
    for (ci = 0; ci < c && grid(ri, ci) == '?'; ++ci) {}
    return ci;
}


void ABCake::solve()
{
    if (dbg_flags & 0x1) { print_grid(cerr); }

    unsigned ri0;
    // skip undef rows
    for (ri0 = 0; ri0 < r && first_row_kid(ri0) == c; ++ri0) {};

    for (unsigned ri = ri0; ri < r; ++ri)
    {
        unsigned ci0 = first_row_kid(ri);
        if (ci0 == c)
        {
            for (unsigned ci = 0; ci < c; ++ci)
            {
                grid(ri, ci) = grid(ri - 1, ci);
            }
        }
        else
        {
            for (unsigned ci = 0; ci < c; )
            {
                char ckid = grid(ri, ci0);
                for (; ci < c && grid(ri, ci) == '?'; ++ci)
                {
                    grid(ri, ci) = ckid;
                }
                if (ci < c)
                {
                    ci0 = ci;
                    ++ci;
                }
            }            
        }  
    }

    for (unsigned ri = 0; ri < ri0; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            grid(ri, ci) = grid(ri0, ci);
        }
    }
}

void ABCake::print_solution(ostream &fo) const
{
    fo << "\n";
    print_grid(fo);
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (ABCake::*psolve)() =
        (naive ? &ABCake::solve_naive : &ABCake::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        ABCake problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
