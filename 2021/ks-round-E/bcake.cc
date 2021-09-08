// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <sstream>
#include <iostream>
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
    Bcake(u_t _R, u_t _C, u_t _K, u_t _r1, u_t _c1, u_t _r2, u_t _c2) :
        R(_R), C(_C), K(_K),
        r1(_r1), c1(_c1), r2(_r2), c2(_c2),
        solution(0)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
    string str() const;
 private:
    void print_cake(ostream& os=cerr) const;
    u_t n_cuts_for(u_t sz) const { return (sz + K - 1)/K; }
    void left_precuts();
    ull_t inner_cuts(ull_t a, ull_t b) const;
    u_t R, C, K;
    u_t r1, c1, r2, c2;
    ull_t solution;
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
    const ull_t h_deli_cuts = n_cuts_for(h_deli);
    const ull_t w_deli_cuts = n_cuts_for(w_deli);
    const u_t left = c1 - 1;
    const u_t right = C - c2;
    const u_t top = r1 - 1;
    const u_t bottom  = R - r2;
    const u_t min_lr = min(left, right);
    const u_t min_tb = min(top, bottom);
    const u_t max_lr = max(left, right);
    const u_t max_tb = max(top, bottom);
    const u_t min_lrtb = min(min_lr, min_tb);
    const u_t max_lrtb = max(max_lr, max_tb);
    ull_t pre_cuts = 0;

    if (dbg_flags & 0x1) { 
        cerr << "lrtb: min=" << min_lrtb << ", max=" << max_lrtb << '\n'; }
    if (max_lrtb > 0)
    {
        ull_t pre_cuts_lr = 0, pre_cuts_tb = 0;

        u_t h_pre_cuts = n_cuts_for(min_lr == left ? c2 : C + 1 - c1);
        pre_cuts_lr += (top > 0 ? h_pre_cuts : 0);
        pre_cuts_lr += (bottom > 0 ? w_deli_cuts : 0);
        pre_cuts_lr += (left > 0 ? h_deli_cuts : 0);
        pre_cuts_lr += (right > 0 ? h_deli_cuts : 0);
        if ((min_lr == left) && (dbg_flags & 0x1)) { left_precuts(); }

        u_t v_pre_cuts = n_cuts_for(min_tb == top ? r2 : R + 1 - r1);
        pre_cuts_tb += (min_lr > 0 ? v_pre_cuts : 0);
        pre_cuts_tb += (max_lr > 0 ? h_deli_cuts : 0);
        pre_cuts_tb += (top > 0 ? w_deli_cuts : 0);
        pre_cuts_tb += (bottom > 0 ? w_deli_cuts : 0);

        pre_cuts = min(pre_cuts_lr, pre_cuts_tb);
    }
    ull_t hv_cuts = inner_cuts(h_deli, w_deli);
    ull_t vh_cuts = inner_cuts(w_deli, h_deli);
    solution = pre_cuts + min(hv_cuts, vh_cuts);
}

ull_t Bcake::inner_cuts(ull_t a, ull_t b) const
{
    // think  a rows,  b columns
    ull_t n_cuts = 0;
    ull_t na = (a + K - 1)/K;
    ull_t nb = (b + K - 1)/K;
    ull_t na_major = (a - 1)/K;
    n_cuts += na_major * nb; // major horizontal lines
    ull_t n_single_covered = na_major * b;
    ull_t pending = (a - 1)*b - n_single_covered;
    n_cuts += (b - 1) * na; // ALL verticals lines
    n_cuts += pending; // singles
    return n_cuts;
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

string Bcake::str() const
{
    ostringstream os;
    os << "{R=" << R << ", C="<<C << ", K-"<<K <<
        ", r1="<<r1 << ", c1="<<c1 << ", r2="<<r2 << ", c2="<<c2 << "}";
    return os.str() ;
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

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t max_rc = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t R = rand() % max_rc + 1;
        u_t C = rand() % max_rc + 1;
        u_t K = rand() % max_rc + 1;
        u_t r1 = rand() % R + 1;
        u_t c1 = rand() % C + 1;
        u_t r2 = r1 + (r1 < R ? rand() % (R - r1) : 0);
        u_t c2 = c1 + (c1 < C ? rand() % (C - c1) : 0);
        
        Bcake bk0(R, C, K, r1, c1, r2, c2);
        cerr << "Bk0: " << bk0.str() << "... solution: "; cerr.flush();
        bk0.solve();
        u_t solution = bk0.get_solution();
        cerr << solution << '\n';

        for (u_t mask = 0x1; (rc == 0) && (mask < 2*2*2); ++mask)
        {
            u_t aR = R, aC = C, ar1 = r1, ac1 = c1, ar2 = r2, ac2 = c2;
            if (mask & 0x1)
            {
                ar1 = R + 1 - r2;
                ar2 = R + 1 - r1;
            }
            if (mask & 0x2)
            {
                ac1 = C + 1 - c2;
                ac2 = C + 1 - c1;
            }
            if (mask & 0x4)
            {
                swap(aR, aC);
                swap(ar1, ac1);
                swap(ar2, ac2);
            }
            Bcake bk(aR, aC, K, ar1, ac1, ar2, ac2);
            bk.solve();
            u_t a_solution = bk0.get_solution();
            if (a_solution != solution)
            {
                cerr << "Inconsistent: bk: " << bk.str() << 
                    " a_solution=" << a_solution << 
                    " != solution = " << solution << '\n';
                rc = 1;
            }
        }
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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
