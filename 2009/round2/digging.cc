// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/March/03

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include <vector>
#include <utility>
#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
static unsigned dbg_flags;

class Digging
{
 public:
    Digging(istream& fi);
    ~Digging() { delete [] _cells; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef array<unsigned, 4> a2_t;
    typedef array<unsigned, 4> a4_t;
    typedef vector<a2_t> va2_t;
    // typedef map<unsigned, unsigned> u2u_t;
    typedef map<a4_t, unsigned> memo_t;
    
    const bool &cell(unsigned x, unsigned y) const 
        { return _cells[xy2i(x, y)]; }
    bool &cell(unsigned x, unsigned y) { return _cells[xy2i(x, y)]; }
    unsigned xy_solve(unsigned x, unsigned y);
    unsigned dig_segment(unsigned xdb, unsigned xde, unsigned y);
    unsigned xy2i(unsigned x, unsigned y) const { return r*x + y; }
    unsigned jump_down(unsigned x, unsigned y, unsigned prefall);
    unsigned cave_size(unsigned x, unsigned y) const;
    unsigned r, c, f;
    unsigned infinity;
    bool *_cells;
    va2_t dig_stack;
    memo_t memo; // Must be changed to reflect segment-dig
    unsigned solution;
};

Digging::Digging(istream& fi) : r(0), c(0), f(0), _cells(0), solution(0)
{
    fi >> r >> c >> f;
    _cells = new bool[r * c];
    solution = infinity = r * c + 1;
    for (unsigned y = 0; y < r; ++y)
    {
        string s;
        fi >> s;
        for (unsigned x = 0; x < c; ++x)
        {
            cell(x, y) = (s[x] == '#');
        }
    }
    dig_stack.push_back(a2_t({0, c}));
}

void Digging::solve_naive()
{
    solve();
}

void Digging::solve()
{
#if 0
    unsigned cs = cave_size(0, 0);
    if (cs < f + 1)
    {
        solution = xy_solve(0, cs - 1);
    }
#endif
    solution = jump_down(0, 0, 0);
}

static void minby(unsigned &var, unsigned val)
{
    if (var > val)
    {
        var = val;
    }
}

unsigned Digging::xy_solve(unsigned x, unsigned y)
{
    unsigned ret = infinity;
    const a2_t &seg = dig_stack.back();
    memo_t::key_type key = {x, y, seg[0], seg[1]};
    auto er = memo.equal_range(key);
    if (er.first != er.second)
    {
        ret = er.first->second;
    }
    else
    {
        if (y == r - 1)
        {
            ret = 0;
        }
        else
        {
            // find under-segment
            unsigned xb = x, xe = x + 1, xs, y1 = y + 1;
            for (xs = x - 1; (xb > 0) && !cell(xs, y) && cell(xs, y1);
                xb = xs--) {}
            for (; (xe < c) && !cell(xe, y) && cell(xe, y1); ++xe) {}

            if ((xb > 0) && !cell(xb - 1, y)) // fall left
            {
                minby(ret, jump_down(xb - 1, y1, 1));
            }
            if ((xe > c) && !cell(xe, y)) // fall right
            {
                minby(ret, jump_down(xe, y1, 1));
            }

            unsigned xde_limit = xe - 1;
            for (unsigned xdb = xb; xdb < xe; ++xdb)
            {
                for (unsigned xde = xb + 1; xde <= xde_limit; ++xde)
                {
                    minby(ret, dig_segment(xdb, xde, y1));
                }
                xde_limit = xe;
            }
        }
        memo_t::value_type v(key, ret);
        memo.insert(er.first, v);
    }
    return ret;
}

unsigned Digging::dig_segment(unsigned xdb, unsigned xde, unsigned y)
{
    unsigned ret = infinity;
    unsigned n_dig = xde - xdb;
    for (unsigned x = xdb; x < xde; ++x)
    {
        cell(x, y) = false;
    }
    dig_stack.push_back(a2_t({xdb, xde}));
    minby(ret, jump_down(xdb, y, 0) + n_dig);
    minby(ret, jump_down(xde - 1, y, 0) + n_dig);
    dig_stack.pop_back();
    for (unsigned x = xdb; x < xde; ++x)
    {
        cell(x, y) = true;
    }
    return ret;
}

unsigned Digging::jump_down(unsigned x, unsigned y, unsigned prefall)
{
    unsigned cs = cave_size(x, y) + prefall;
    unsigned ret = (0 < cs && cs <= f) ? xy_solve(x, y + cs - 1) : infinity;
    return ret;
}

unsigned Digging::cave_size(unsigned x, unsigned y) const
{
    unsigned ret = 0;
    while ((y + ret < r) && !cell(x, y + ret))
    {
        ++ret;
    }
    return ret;
}

void Digging::print_solution(ostream &fo) const
{
    if (solution < infinity)
    {
        fo << " Yes " << solution;
    }
    else
    {
        fo << " No";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }
    
    int ai_in = ai;
    int ai_out = ai + 1;
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

    unsigned n_cases;
    *pfi >> n_cases;

    void (Digging::*psolve)() =
        (naive ? &Digging::solve_naive : &Digging::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Digging digging(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (digging.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        digging.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
