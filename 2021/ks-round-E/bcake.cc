// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class Cut
{
 public:
    typedef enum {Right, Left, Down, Up} edir_t;
    Cut(u_t _i=0, u_t _j=0, edir_t _dir=Right, u_t _len=0) :
        i(_i), j(_j), dir(_dir), len(_len) {}
    u_t i, j;
    edir_t dir;
    u_t len;
};
typedef vector<Cut> vcut_t;

class Bcake
{
 public:
    Bcake(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void print_cake(ostream& os=cerr) const;
    u_t n_cuts_for(u_t sz) const { return (sz + K - 1)/K; }
    void left_precuts();
    u_t R, C, K;
    u_t r1, c1, r2, c2;
    u_t solution;
    vcut_t cuts;
};

Bcake::Bcake(istream& fi)
{
    fi >> R >> C >> K;
    fi >> r1 >> c1 >> r2 >> c2;
}

void Bcake::solve_naive()
{
    // first_cut;
    const u_t h_deli = r2 + 1 - r1;
    const u_t w_deli = c2 + 1 - c1;
    const u_t left = c1 - 1;
    const u_t right = C - c2;
    const u_t top = r1 - 1;
    const u_t bottom  = R - r2;
    const u_t min_lr = min(left, right);
    const u_t min_tb = min(top, bottom);
    const u_t min_lrtb = min(min_lr, min_tb);
    const u_t max_lrtb = max(max(left, right), max(top, bottom));
    u_t pre_cuts = 0;

    if (dbg_flags & 0x1) { 
        cerr << "lrtb: min=" << min_lrtb << ", max=" << max_lrtb << '\n'; }
    if (max_lrtb > 0)
    {
        if (min_lrtb == left)
        {
            u_t h_pre_cuts = n_cuts_for(c2);
            pre_cuts += (top > 0 ? h_pre_cuts : 0);
            pre_cuts += (bottom > 0 ? w_deli : 0);
            pre_cuts += (left > 0 ? h_deli : 0);
            pre_cuts += (right > 0 ? h_deli : 0);
            if (dbg_flags & 0x1) { left_precuts(); }
        }
        else if (min_lrtb == right)
        {
            u_t h_pre_cuts = n_cuts_for(C + 1 - c1);
            pre_cuts += (top > 0 ? h_pre_cuts : 0);
            pre_cuts += (bottom > 0 ? w_deli : 0);
            pre_cuts += (left > 0 ? h_deli : 0);
            pre_cuts += (right > 0 ? h_deli : 0);
        }
        else if (min_lrtb == top)
        {
            u_t v_pre_cuts = n_cuts_for(c2);
            pre_cuts += 2 * v_pre_cuts;
            pre_cuts += 2 * w_deli;
        }
        else // (min_lrtb == top))
        {
            u_t v_pre_cuts = n_cuts_for(R + 1 - c1);
            pre_cuts += 2 * v_pre_cuts;
            pre_cuts += 2 * w_deli;
        }
    }
    u_t h_cuts = (h_deli - 1)*(w_deli + K - 1)/K;
    u_t v_cuts = (w_deli - 1)*(h_deli + K - 1)/K;
    solution = pre_cuts + h_cuts + v_cuts;
}

void Bcake::solve()
{
    if (dbg_flags & 0x1) { print_cake(); }
    solve_naive();
}

void Bcake::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void Bcake::left_precuts()
{
    const u_t left = c1 - 1;
    const u_t right = C - c2;
    const u_t top = r1 - 1;
    const u_t bottom  = R - r2;

    if (top > 0)
    {
        for (u_t j = 0; j + K <= c2; j += K)
        {
            Cut cut(r1 - 2, j, Cut::Right, min(K, r2 - j));
            cuts.push_back(cut);
            if (dbg_flags & 0x2) { print_cake(cerr); }
        }
    }
    if (right > 0)
    {
        for (u_t i = r1 - 1, j = c2 - 1; i + K <= r2; i += K)
        {
            Cut cut(i, c2 - 1, Cut::Down, min(K, c2 - j));
            cuts.push_back(cut);
            if (dbg_flags & 0x2) { print_cake(cerr); }
        }
    }
    if (bottom > 0)
    {
        for (u_t j = c2 - 1; j >= K; j -= K)
        {
            Cut  cut(r2 - 1, j, Cut::Left, min(K, j));
            cuts.push_back(cut);
            if (dbg_flags & 0x2) { print_cake(cerr); }
        }
    }
    if (left > 0)
    {
        for (u_t i = r2 - 1; i >= K; i -= K)
        {
            Cut cut(i, c1 - 1, Cut::Up, min(K, i));
            cuts.push_back(cut);
            if (dbg_flags & 0x2) { print_cake(cerr); }
        }
    }
}

void Bcake::print_cake(ostream& os) const
{
    vector<string> rows;
    rows.reserve(2*R - 1);
    string row_sep, row_cake;
    for (u_t i = 0; i < C; ++i)
    {
        row_sep.push_back('-');
        row_sep.push_back('+');
        row_cake.push_back('.');
        row_cake.push_back('|');
    }
    row_sep.pop_back();
    row_cake.pop_back();
    for (u_t r = 0; r < R; ++r)
    {
        rows.push_back(row_cake);
        rows.push_back(row_sep);
    }
    rows.pop_back();
    for (u_t r = r1-1; r <= r2-1; ++r)
    {
        string& row = rows[2*r];
        for (u_t c = c1-1; c <= c2-1; ++c)
        {
            row[2*c] = '*';
        }
    }

    for (const Cut& cut: cuts)
    {
        int dr = 0, dc = 0;
        char c = '?';
        u_t i = 2*cut.i, j = 2*cut.j;
        switch (cut.dir)
        {
         case Cut::Right:
            dc = 1;
            c = '>';
            ++i;
            break;
         case Cut::Left:
            dc = -1;
            c = '<';
            ++i;
            break;
         case Cut::Down:
            dr = 1;
            c = 'v';
            ++j;
            break;
         case Cut::Up:
            dr = -1;
            c = '^';
            ++j;
            break;
        }
        for (u_t len = 0; len < 2*cut.len; i += dr, j += dc, ++len)
        {
            rows[i][j] = c;
        }
    }

    os << "\nprecuts: " << cuts.size() << '\n';
    for (const string& row: rows)
    {
        os << row << '\n';
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Bcake::*psolve)() =
        (naive ? &Bcake::solve_naive : &Bcake::solve);
    if (solve_ver == 1) { psolve = &Bcake::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bcake bcake(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bcake.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bcake.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
