// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>
#include <cstdlib>

#include <cstdlib>

using namespace std;

static unsigned dbg_flags;

enum { MaxSz = 100 };

typedef set<int> seti_t;

class Step
{
 public:
   Step(char vt='.', unsigned vx=0, unsigned vy=0) : t(vt), x(vx), y(vy) {}
   char t;
   unsigned x, y;
};
typedef vector<Step> vstep_t;

class Fashion
{
 public:
    Fashion(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void print_grid() const;
    void init();
    bool may_o_parallel(unsigned x, unsigned y) const;
    bool may_o_diags(unsigned x, unsigned y) const;
    bool may_o(unsigned x, unsigned y) const;
    bool may_plus(unsigned x, unsigned y) const;
    bool may_x(unsigned x, unsigned y) const;
    void add_o();
    void upgrade_o();
    void add_plus();
    void add_x();
    void set_txy(char t, unsigned x, unsigned y);
    void add_others(char t, unsigned x, unsigned y);
    unsigned n, m;
    char grid[MaxSz][MaxSz];
    seti_t o_rows, o_cols, o_diag_sw_ne, o_diag_nw_se;
    vstep_t steps;
};

Fashion::Fashion(istream& fi)
{
    for (unsigned x = 0; x < MaxSz; ++x)
    {
        for (unsigned y = 0; y < MaxSz; ++y)
        {
            grid[x][y] = '.';
        }
    }
    fi >> n >> m;
    for (unsigned p = 0; p < m; ++p)
    {
        string st;
        unsigned r, c;
        fi >> st >> r >> c;
        char t = st[0];
        grid[c - 1][r - 1] = t;
    }
}

void Fashion::print_grid() const
{
    for (unsigned y = n; y > 0;)
    {
        --y;
        for (unsigned x = 0; x < n; ++x)
        {
            cerr << grid[x][y];
        }
        cerr << "\n";
    }
    cerr << "\n";
}

void Fashion::add_others(char t, unsigned x, unsigned y)
{
    if ((t == 'x') || (t == 'o'))
    {
        o_cols.insert(x);
        o_rows.insert(y);
    }
    if ((t == '+') || (t == 'o'))
    {
        o_diag_sw_ne.insert(x - y);
        o_diag_nw_se.insert(x + y);
    }
}

void Fashion::init()
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            add_others(t, x, y);
        }
    }
}

bool Fashion::may_o_parallel(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && o_cols.find(x) == o_cols.end();
    ret = ret && o_rows.find(y) == o_rows.end();
    return ret;
}

bool Fashion::may_o_diags(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && o_diag_sw_ne.find(x - y) == o_diag_sw_ne.end();
    ret = ret && o_diag_nw_se.find(x + y) == o_diag_nw_se.end();
    return ret;
}

bool Fashion::may_o(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && may_o_parallel(x, y);
    ret = ret && may_o_diags(x, y);
    return ret;
}

bool Fashion::may_plus(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && o_diag_sw_ne.find(x - y) == o_diag_sw_ne.end();
    ret = ret && o_diag_nw_se.find(x + y) == o_diag_nw_se.end();
    return ret;
}

bool Fashion::may_x(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && o_cols.find(x) == o_cols.end();
    ret = ret && o_rows.find(y) == o_rows.end();
    return ret;
}

void Fashion::set_txy(char t, unsigned x, unsigned y)
{
    grid[x][y] = t;
    steps.push_back(Step(t, x, y));
    add_others(t, x, y);
}

void Fashion::add_o()
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            if ((t == '.') && may_o(x, y))
            {
                set_txy('o', x, y);
            }
        }
    }
}

void Fashion::upgrade_o()
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            if ((t == '+') || (t == 'x'))
            {
                bool may = (t == '+') && may_o_parallel(x, y);
                may = may || ((t == '-') && may_o_diags(x, y));
                if (may)
                {
                    set_txy('o', x, y);
                }
            }
        }
    }
}

void Fashion::add_plus()
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            if ((t == '.') && may_plus(x, y))
            {
                set_txy('+', x, y);
            }
        }
    }
}

void Fashion::add_x()
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            if ((t == '.') && may_x(x, y))
            {
                set_txy('x', x, y);
            }
        }
    }
}

void Fashion::solve_naive()
{
    if (dbg_flags) { print_grid(); }
    init();
    const char *qenv = getenv("FASHUINQ") ? : "aupx";
    for (unsigned qi = 0; qi < 4; ++qi)
    {
        char qc = qenv[qi];
        switch (qc)
        {
         case 'a': add_o(); break;
         case 'u': upgrade_o(); break;
         case 'p': add_plus(); break;
         case 'x': add_x(); break;
         default: 
            cerr << "Bad qc="<<qc << ", in qenv="<<qenv << "\n";
            exit(1); 
        }
    }
    if (dbg_flags) { print_grid(); }
}

void Fashion::solve()
{
    solve_naive();
}

void Fashion::print_solution(ostream &fo) const
{
    unsigned score = 0;
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            char t = grid[x][y];
            if ((t == '+') || (t == 'x'))
            {
                score += 1;
            }
            else if (t == 'o')
            {
                score += 2;
            }
        }
    }
    unsigned n_steps = steps.size();
    fo << ' ' << score << ' ' << n_steps << "\n";
    for (unsigned si = 0; si < n_steps; ++si)
    {
        const Step &step = steps[si];
        fo << step.t << ' ' << step.y + 1 << ' ' << step.x + 1 << "\n";
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

    void (Fashion::*psolve)() =
        (naive ? &Fashion::solve_naive : &Fashion::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        Fashion problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
