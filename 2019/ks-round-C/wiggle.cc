// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <list>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;
typedef list<u_t> listu_t;
typedef listu_t::iterator listui_t;
typedef listu_t::reverse_iterator listuri_t;
typedef vector<listu_t> vlistu_t;

static unsigned dbg_flags;

static listu_t dum_list;
static listui_t dum_iter = dum_list.end();
static listuri_t dum_riter = dum_list.rend();

class Square
{
 public:
   Square(listui_t vri=dum_iter, listui_t vci=dum_iter,
       listuri_t vrri=dum_riter, listuri_t vrci=dum_riter) : 
       ri(vri), ci(vci), rri(vrri), rci(vrci)
       {}
   listui_t ri, ci;
   listuri_t rri, rci;
};
typedef vector<Square> vsqr_t;
typedef vector<vsqr_t> vvsqr_t;

class Wiggle
{
 public:
    Wiggle(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t find_empty(const vb_t& v, u_t b, int step) const;
    void init_squares();
    u_t sqr_find_empty(Square *psqr, listu_t &l, bool is_row, bool fwd);
    u_t n, r, c, sr, sc;
    u_t orig_sr, orig_sc;
    string moves;
    u_t er, ec;
    vvb_t rows;
    vvb_t cols;

    vlistu_t lrows;
    vlistu_t lcols;
    vvsqr_t squares;
};

Wiggle::Wiggle(istream& fi) : er(0), ec(0)
{
    fi >> n >> r >> c >> sr >> sc;
    --sr;  --sc; // Dijkstra style

    orig_sr = sr;
    orig_sc = sc;
    sr = 102; sc = 102;

    fi >> moves;
    vb_t empty_row(vb_t::size_type(210), false);
    vb_t empty_col(vb_t::size_type(2100), false);
    rows = vvb_t(vvb_t::size_type(210), empty_row);
    cols = vvb_t(vvb_t::size_type(210), empty_col);
}

void Wiggle::solve_naive()
{
    rows[sr][sc] = true;
    cols[sc][sr] = true;
    er = sr; ec = sc;
    for (u_t ci = 0; ci < n; ++ci)
    {
        const char move = moves[ci];
        switch (move)
        {
         case 'N':
            er = find_empty(cols[ec], er, -1);
            break;
         case 'S':
            er = find_empty(cols[ec], er, +1);
            break;
         case 'W':
            ec = find_empty(rows[er], ec, -1);
            break;
         case 'E':
            ec = find_empty(rows[er], ec, +1);
            break;
        }
        rows[er][ec] = true;
        cols[ec][er] = true;
    }
}

void Wiggle::solve()
{
#if 1
    solve_naive();
#else
    init_squares();
    er = sr; ec = sc;
    Square *psqr = &squares[er][ec];
    for (u_t ci = 0; ci < n; ++ci)
    {
        listu_t &lrow = lrows[er];
        listu_t &lcol = lcols[ec];
        const char move = moves[ci];
        switch (move)
        {
         case 'N':
            er = sqr_find_empty(psqr, lcol, false, false);
            break;
         case 'S':
            er = sqr_find_empty(psqr, lcol, false, true);
            break;
         case 'W':
            ec = sqr_find_empty(psqr, lrow, true, false);
            break;
         case 'E':
            ec = sqr_find_empty(psqr, lrow, true, true);
            break;
        }
        lrow.erase(psqr->ri);
        lcol.erase(psqr->ci);
        psqr = &squares[er][ec];
    }
#endif
}

u_t Wiggle::find_empty(const vb_t& v, u_t b, int step) const
{
    for (b = b + step; v[b]; b += step) {}
    return b;
}

u_t Wiggle::sqr_find_empty(Square *psqr, listu_t &l, bool is_row, bool fwd)
{
    u_t ret = ~0;
    Square &sqr = *psqr;
    if (fwd)
    {
        listui_t i = (is_row ? sqr.ri : sqr.ci), i1 = i;
        ++i1;
        ret = *i1;
        // l.erase(i);
    }
    else
    {
        listuri_t ri = (is_row ? sqr.rri : sqr.rci), ri1 = ri;
        ++ri1;
        ret = *ri1;
        // l.erase(i);
    }
    return ret;
}

void Wiggle::init_squares()
{
    vsqr_t row = vsqr_t(vsqr_t::size_type(c), Square());
    squares = vvsqr_t(vvb_t::size_type(r), row);
    lrows = vlistu_t(vlistu_t::size_type(r), listu_t());
    lcols = vlistu_t(vlistu_t::size_type(c), listu_t());
    for (u_t i = 0; i < r; ++i)
    {
        listu_t &lrow = lrows[i];
        u_t j;
        for (j = 0; j < c; ++j)
        {
            lrow.push_back(j);
        }
        j = 0;
        for (listui_t iter = lrow.begin(); iter != lrow.end(); ++iter, ++j)
        {
            squares[i][j].ri = iter;
        }
        j = c;
        for (listuri_t riter = lrow.rbegin(); riter != lrow.rend(); ++riter)
        {
            --j;
            squares[i][j].rri = riter;
        }
    }
    for (u_t j = 0; j < c; ++j)
    {
        listu_t &lcol = lcols[j];
        u_t i;
        for (i = 0; i < r; ++i)
        {
            lcol.push_back(i);
        }
        i = 0;
        for (listui_t iter = lcol.begin(); iter != lcol.end(); ++iter, ++i)
        {
            squares[i][j].ci = iter;
        }
        i = r;
        for (listuri_t riter = lcol.rbegin(); riter != lcol.rend(); ++riter)
        {
            --i;
            squares[i][j].rci = riter;
        }
    }
}

void Wiggle::print_solution(ostream &fo) const
{
    // fo << ' ' << er + 1 << ' ' << ec + 1; 
    fo << ' ' << (er + orig_sr - 102) + 1 << ' ' << (ec + orig_sc - 102) + 1; 
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Wiggle::*psolve)() =
        (naive ? &Wiggle::solve_naive : &Wiggle::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Wiggle wiggle(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (wiggle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        wiggle.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
