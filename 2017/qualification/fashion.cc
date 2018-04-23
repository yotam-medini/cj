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

enum style_t {S_O=0, S_P, S_X, S_E}; // other, plus, X, empty
typedef vector<style_t> vstyle_t;

class XY
{
 public:
    XY(unsigned vx=0, unsigned vy=0) : x(vx), y(vy) {}
    unsigned x, y;
};
bool operator==(const XY &p0, const XY &p1)
{
    bool ret = ((p0.x == p1.x) && (p0.y == p1.y));
    return ret;
}
bool operator<(const XY &p0, const XY &p1)
{
    bool ret = (p0.x < p1.x) || ((p0.x == p1.x) && (p0.y < p1.y));
    return ret;
}
typedef set<XY> set_xy_t;

class Step
{
 public:
   Step(const XY& vxy=XY(), style_t vt=S_E) : xy(vxy), t(vt) {}
   XY xy;
   style_t t;
};
bool operator<(const Step &p0, const Step &p1)
{
    bool ret = (p0.xy < p1.xy) || ((p0.xy == p1.xy) && (p0.t < p1.t));
    return ret;
}
typedef set<Step> set_step_t;
typedef vector<Step> vstep_t;

class Line
{
 public:
    Line(unsigned no=0, unsigned np=0, unsigned nx=0) : 
        n{no, np, nx} 
        {}
    unsigned op_count() const { return n[S_O] + n[S_P]; }
    unsigned ox_count() const { return n[S_O] + n[S_X]; }
    bool op_none() const { return (n[S_O] == 0) && (n[S_P] == 0); }
    bool ox_none() const { return (n[S_O] == 0) && (n[S_X] == 0); }
    unsigned n[3];
};

class LineParallel : public Line
{
 public:
    LineParallel(unsigned no=0, unsigned np=0, unsigned nx=0) : 
        Line(no, np, nx)
        {}
    bool may_upgrade() const
    {
        bool ret = (ox_count() == 0) || (n[S_P] > 1);
        return ret;
    }
};

class LineDiag : public Line
{
 public:
    LineDiag(unsigned no=0, unsigned np=0, unsigned nx=0) : 
        Line(no, np, nx)
        {}
    bool may_upgrade() const
    {
        bool ret = (op_count() == 0) || (n[S_X] > 1);
        return ret;
    }
};

typedef vector<LineParallel> vlinep_t;
typedef vector<LineDiag> vlined_t;

class Fashion
{
 public:
    Fashion(istream& fi);
    void solve();
    void solve_perm();
    void print_solution(ostream&) const;
 private:
    typedef vstep_t::const_iterator vstep_cit_t;
    typedef set_step_t::const_iterator set_step_cit_t;
    static style_t c2style(char c)
    {
        style_t ret = 
           (c == 'o')
           ? S_O
           : ((c == '+')
               ? S_P
               : ((c == 'x')
                   ? S_X
                   : S_E
                 )
             );
        return ret;
    }
    static char style2c(style_t t)
    {
        int ti = int(t);
        char ret = "o+x."[ti];
        return ret;
    }
    void print_grid(const vstyle_t &g) const;
    void init();
    void init_steps_fwd(vstep_t&);
    void init_steps_sub_fwd(vstep_t&, vstep_cit_t b, vstep_cit_t e);
    bool may_o_parallel(unsigned x, unsigned y) const;
    bool may_o_diags(unsigned x, unsigned y) const;
    bool may_o(unsigned x, unsigned y) const;
    bool may_upgrade(unsigned x, unsigned y) const;
    bool may_plus(unsigned x, unsigned y) const;
    bool may_x(unsigned x, unsigned y) const;
    void set_txy(style_t t, unsigned x, unsigned y);
    void set_back_txy(style_t t, unsigned x, unsigned y);
    void advance(const vstep_t &steps_fwd);
    unsigned xy2i(unsigned x, unsigned y) const { return n*x + y; }
    unsigned dxy2ine(unsigned x, unsigned y) const { return (n + x) - (y + 1); }
    unsigned dxy2ise(unsigned x, unsigned y) const { return x + y; }
    unsigned compute_score() const;
    unsigned n, m;
    vstyle_t grid0;
    vstyle_t grid;
    vstyle_t grid_best;
    vlinep_t rows, cols;
    vlined_t diags_ne, diags_se;
    
    vstep_t steps;
    vstep_t steps_best;
    unsigned score;
    unsigned score_best;
};

Fashion::Fashion(istream& fi) : score(0), score_best(0)
{
    fi >> n >> m;
    grid0 = vstyle_t(vstyle_t::size_type(n * n), S_E);
    cols = rows = vlinep_t(vlinep_t::size_type(n), LineParallel());
    diags_ne = diags_se = vlined_t(vlined_t::size_type(2*n - 1), LineDiag());
    for (unsigned p = 0; p < m; ++p)
    {
        string st;
        unsigned r, c;
        fi >> st >> r >> c;
        char ct = st[0];
        grid0[xy2i(c - 1, r - 1)] = c2style(ct);
    }
    
}

void Fashion::print_grid(const vstyle_t& g) const
{
    for (unsigned y = n; y > 0;)
    {
        --y;
        for (unsigned x = 0; x < n; ++x)
        {
            cerr << style2c(g[xy2i(x, y)]);
        }
        cerr << "\n";
    }
    cerr << "\n";
}

bool Fashion::may_o_parallel(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && cols[x].ox_none();
    ret = ret && rows[y].ox_none();
    return ret;
}

bool Fashion::may_o_diags(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && diags_ne[dxy2ine(x, y)].op_none();
    ret = ret && diags_se[dxy2ise(x, y)].op_none();
    return ret;
}

bool Fashion::may_o(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && may_o_parallel(x, y);
    ret = ret && may_o_diags(x, y);
    return ret;
}

bool Fashion::may_upgrade(unsigned x, unsigned y) const
{
    bool ret = true;
    style_t t = grid[xy2i(x, y)];
    if (t == S_P)
    {
        ret = ret && cols[x].may_upgrade();
        ret = ret && rows[y].may_upgrade();
    }
    else // t == S_X
    {
        ret = ret && diags_ne[dxy2ine(x, y)].may_upgrade();
        ret = ret && diags_se[dxy2ise(x, y)].may_upgrade();
    }
    return ret;
}

bool Fashion::may_plus(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && diags_ne[dxy2ine(x, y)].op_none();
    ret = ret && diags_se[dxy2ise(x, y)].op_none();
    return ret;
}

bool Fashion::may_x(unsigned x, unsigned y) const
{
    bool ret = true;
    ret = ret && cols[x].ox_none();
    ret = ret && rows[y].ox_none();
    return ret;
}

void Fashion::set_txy(style_t t, unsigned x, unsigned y)
{
    if (int(t) >= 3) { 
      cerr << "fuck " << __func__ << "\n"; exit(1);}
    unsigned i = xy2i(x, y);
    style_t old = grid[i];
    grid[i] = t;
    if (dbg_flags & 0x4) { print_grid(grid); }
    score += ((old == S_E) && (t == S_O) ? 2 : 1);
    steps.push_back(Step(XY(x, y), t));
    if (old != S_E)
    {
        --(cols[x].n[old]);
        --(rows[y].n[old]);
        --(diags_ne[dxy2ine(x, y)].n[old]);
        --(diags_se[dxy2ise(x, y)].n[old]);
    }
    ++(cols[x].n[t]);
    ++(rows[y].n[t]);
    ++(diags_ne[dxy2ine(x, y)].n[t]);
    ++(diags_se[dxy2ise(x, y)].n[t]);
}

void Fashion::set_back_txy(style_t t, unsigned x, unsigned y)
{
    if (int(t) > 3) { 
      cerr << "fuck " << __func__ << "\n"; exit(1); }
    unsigned i = xy2i(x, y);
    style_t undo = grid[i];
    if (int(undo) > 3) { 
      cerr << "fuck " << __func__ << "\n"; exit(1); }
    grid[i] = t;
    steps.pop_back();
    if (t != S_E)
    {
        ++(cols[x].n[t]);
        ++(rows[y].n[t]);
        ++(diags_ne[dxy2ine(x, y)].n[t]);
        ++(diags_se[dxy2ise(x, y)].n[t]);
    }
    --(cols[x].n[undo]);
    --(rows[y].n[undo]);
    --(diags_ne[dxy2ine(x, y)].n[undo]);
    --(diags_se[dxy2ise(x, y)].n[undo]);
}

void Fashion::init()
{
    score = 0;
    grid = vstyle_t(vstyle_t::size_type(n * n), S_E);
    cols = rows = vlinep_t(vlinep_t::size_type(n), LineParallel());
    diags_ne = diags_se = vlined_t(vlined_t::size_type(2*n - 1), LineDiag());
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            style_t t = grid0[xy2i(x, y)];
            if (t != S_E)
            {
                set_txy(grid0[xy2i(x, y)], x, y);
            }
        }
    }
    steps.clear();
}

void Fashion::init_steps_fwd(vstep_t& steps_fwd)
{
    for (unsigned x = 0; x < n; ++x)
    {
        for (unsigned y = 0; y < n; ++y)
        {
            style_t t = grid[xy2i(x, y)];
            if (t != S_O)
            {
                const XY xy(x, y);
                if (t == S_E)
                {
                    if (may_o(x, y))
                    {
                        steps_fwd.push_back(Step(xy, S_O));
                    }
                    if (may_plus(x, y))
                    {
                        steps_fwd.push_back(Step(xy, S_P));
                    }
                    if (may_x(x, y))
                    {
                        steps_fwd.push_back(Step(xy, S_X));
                    }
                }
                else
                {
                    if (may_upgrade(x, y))
                    {
                        steps_fwd.push_back(Step(xy, S_O));
                    }
                }
            }
        }
    }
}

void Fashion::init_steps_sub_fwd(
    vstep_t& steps_fwd, vstep_cit_t b, vstep_cit_t e)
{
    for (vstep_cit_t i = b; i != e; ++i)
    {
        const Step &step = *i;
        const XY &xy = step.xy;
        unsigned x = xy.x;
        unsigned y = xy.y;
        style_t t = grid[xy2i(x, y)];
        if (t != S_O)
        {
            bool add = false;
            if (t == S_E)
            {
                if (step.t == S_O)
                {
                    add = may_o(x, y);
                }
                else if (step.t == S_P)
                {
                    add = may_plus(x, y);
                }
                else // step.t == S_X
                {
                    add = may_x(x, y);
                }
            }
            else if (step.t == S_O)
            {
                add = may_upgrade(x, y);
            }
            if (add)
            {
                steps_fwd.push_back(step);
            }
        }
    }
}

void Fashion::advance(const vstep_t& steps_fwd)
{
    if (steps_fwd.empty())
    {
        if (score_best < score)
        {
            score_best = score;
            steps_best = steps;
            grid_best = grid;
            if (dbg_flags & 0x2) 
            { 
                cerr << "score_best="<<score_best<<"\n";
                print_grid(grid_best);
            }
        }
    }
    else
    {
        for (vstep_cit_t i = steps_fwd.begin(), i1 = i, e = steps_fwd.end();
            i != e; i = i1)
        {
            ++i1;
            unsigned score_save = score;
            const Step &step = *i;
            style_t told = grid[xy2i(step.xy.x, step.xy.y)];
            set_txy(step.t, step.xy.x, step.xy.y);
            vstep_t sub_fwd;
            init_steps_sub_fwd(sub_fwd, i1, e);
            advance(sub_fwd);
            set_back_txy(told, step.xy.x, step.xy.y);
            score = score_save;
        }
    }
}

void Fashion::solve()
{
    init();
    vstep_t steps_fwd;
    init_steps_fwd(steps_fwd);
    score = compute_score();
    advance(steps_fwd);
    if (dbg_flags & 0x1) { print_grid(grid_best); }
}

unsigned Fashion::compute_score() const
{
    unsigned score = 0;
    for (style_t t : grid)
    {
        if ((t == S_P) || (t == S_X))
        {
            score += 1;
        }
        else if (t == S_O)
        {
            score += 2;
        }
    }
    return score;
}

void Fashion::print_solution(ostream &fo) const
{
    // unsigned score = compute_score();
    unsigned n_steps = steps_best.size();
    fo << ' ' << score_best << ' ' << n_steps << "\n";
    for (unsigned si = 0; si < n_steps; ++si)
    {
        const Step &step = steps_best[si];
        char c = style2c(step.t);
        fo << c << ' ' << step.xy.y + 1 << ' ' << step.xy.x + 1 << "\n";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");


    unsigned n_opts = 0;
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

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        Fashion problem(*pfi);
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
