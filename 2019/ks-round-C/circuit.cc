// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class Circuit
{
 public:
    Circuit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t r, c, k;
    u_t solution;
    vvu_t board;
    vvu_t r_eq_count;
    vvu_t c_eq_count;
};

void maximize(u_t &v, u_t by)
{
    if (v < by)
    {
        v = by;
    }
}

void minimize(u_t &v, u_t by)
{
    if (v > by)
    {
        v = by;
    }
}

Circuit::Circuit(istream& fi) : solution(1)
{
    fi >> r >> c >> k;
    board.reserve(r);
    for (u_t i = 0; i < r; ++i)
    {
        vu_t row;
        row.reserve(c);
        for (u_t j = 0; j < c; ++j)
        {
            u_t x;
            fi >> x;
            row.push_back(x);
        }
        board.push_back(row);
    }
}

void Circuit::solve_naive()
{
    vu_t row_z(vu_t::size_type(c), 0);
    r_eq_count = vvu_t(vvu_t::size_type(r), row_z;
    c_eq_count = vvu_t(vvu_t::size_type(r), row_z);
    for (u_t j = 0; j < c; ++j)
    {
        u_t i = r - 1;
        while (i > 1)
        {
            --i;
            if (board[i - 1][j] == board[i][j])
            {
                c_eq_count[i - 1][j] = c_eq_count[i][j] + 1;
            }
        }
    }
    for (u_t i = 0; i < r; ++i)
    {
        u_t j = c - 1;
        while (j > 1)
        {
            --j;
            if (board[i][j - 1] == board[i][j])
            {
                r_eq_count[i][j - 1] = r_eq_count[i][j] + 1;
            }
        }
    }

    for (u_t i = 0; i < r; ++i)
    {
        for (u_t j = 0; j < c; ++j)
        {
            u_t candid = 0;
            u_t m = c_eq_count[i][j];
            maximize(solution, 1*m);
            for (u_t w = 1; w <= r_eq_count[i][j]; ++w)
            {
                minimize(m, c_eq_count[i][j + w];
                maximize(solution, (w + 1)*m;
            }
        }
    }
}

void Circuit::solve()
{
    solve_naive();
}

void Circuit::print_solution(ostream &fo) const
{
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

    void (Circuit::*psolve)() =
        (naive ? &Circuit::solve_naive : &Circuit::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Circuit circuit(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (circuit.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        circuit.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
