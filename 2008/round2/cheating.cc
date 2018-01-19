// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<ul_t> vul_t;

class InterenalNode
{
 public:
    InterenalNode(bool a=false, bool c=false): gand(a), changable(c) {}
    bool gand;
    bool changable;
};

typedef vector<InterenalNode> vin_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

class Cheating
{
 public:
    Cheating(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool eval(unsigned root, unsigned change_bits) const;
    unsigned sub_solve(unsigned root) const;
    unsigned m; // also like infinity
    bool v;
    vin_t inodes;
    vb_t leaves;
    unsigned solution;
};

Cheating::Cheating(istream& fi) :
    m(0),
    v(false)
{
    unsigned u, u1;
    fi >> m >> u;
    v = (u == 1);
    unsigned n_inodes = (m - 1)/2;
    unsigned n_leaves = (m + 1)/2;
    inodes = vin_t(vin_t::size_type(n_inodes), false);
    leaves = vb_t(vb_t::size_type(n_leaves), false);
    for (unsigned i = 0; i < n_inodes; ++i)
    {
        fi >> u >> u1;
        inodes[i] = InterenalNode(u == 1, u1 == 1);
    }
    for (unsigned i = 0; i < n_leaves; ++i)
    {
        fi >> u;
        leaves[i] = (u == 1);
    }
    solution = m;
}

void Cheating::solve_naive()
{
    unsigned n_inodes = (m - 1)/2;
    unsigned bit_limit = 1u << n_inodes;
    for (unsigned bits = 0; bits < bit_limit; ++bits)
    {
        bool e = eval(0, bits);
        if (e == v)
        {
            unsigned n_changes = 0;
            for (unsigned ii = 0; ii < n_inodes; ++ii)
            {
                const InterenalNode& inode = inodes[ii];
                if (inode.changable && ((1ul << ii) & bits))
                {
                    ++n_changes;
                }                
            }
            if (solution > n_changes)
            {
                solution = n_changes;
            }
        }
    }
}

bool Cheating::eval(unsigned root, unsigned change_bits) const
{
    bool ret = false;
    unsigned n_inodes = inodes.size();
    if (root < n_inodes)
    {
        const InterenalNode& inode = inodes[root];
        unsigned rnode = 2*(root + 1);
        unsigned lnode = rnode - 1;
        bool l_val = eval(lnode, change_bits);
        bool r_val = eval(rnode, change_bits);
        bool g = inode.gand;
        if (inode.changable && ((1ul << root) & change_bits))
        {
            g = !g;
        }
        ret = (g ? (l_val && r_val) : (l_val || r_val));
    }
    else
    {
        ret = leaves[root - n_inodes];
    }
    return ret;
}

void Cheating::solve()
{
    solution = sub_solve(0);
}

unsigned Cheating::sub_solve(unsigned root) const
{
    unsigned ret = 0;
    unsigned n_inodes = inodes.size();
    if (root < n_inodes)
    {
        const InterenalNode& inode = inodes[root];
        unsigned rnode = 2*(root + 1);
        unsigned lnode = rnode - 1;
        unsigned l_solution = sub_solve(lnode);
        unsigned r_solution = sub_solve(rnode);
        if (v)
        {
            if (inode.gand)
            {
                ret = l_solution + r_solution;
                if (inode.changable)
                {
                    unsigned c_ret = min(l_solution, r_solution) + 1;
                    if (ret > c_ret)
                    {
                        ret = c_ret;
                    }
                }
            }
            else
            {
                ret = min(l_solution, r_solution);
            }
        }
        else // v == false
        {
            if (inode.gand)
            {
                ret = min(l_solution, r_solution);
            }
            else
            {
                ret = l_solution + r_solution;
                if (inode.changable)
                {
                    unsigned c_ret = min(l_solution, r_solution) + 1;
                    if (ret > c_ret)
                    {
                        ret = c_ret;
                    }
                }
            }
        }
    }
    else
    {
        unsigned leafi = root - n_inodes;
        bool leaf_value = leaves[leafi];
        ret = (leaf_value == v ? 0 : m);
    }
    return ret;
}

void Cheating::print_solution(ostream &fo) const
{
    if (solution >= m)
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << solution;
    }
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

    void (Cheating::*psolve)() =
        (naive ? &Cheating::solve_naive : &Cheating::solve);

    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cheating problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
