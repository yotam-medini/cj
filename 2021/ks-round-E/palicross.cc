// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

class Palicross
{
 public:
    Palicross(istream& fi);
    Palicross(size_t _N, size_t _M, const vs_t& rows) :
        N(_N), M(_M), R(rows), n_filled(0) {}
    void solve_naive();
    void solve();
    void solve_smart();
    void print_solution(ostream&) const;
    const vs_t& get_solution(u_t& nf) const
    {
        nf = n_filled;
        return R;
    }
 private:
    size_t N, M;
    vs_t R;
    size_t n_filled;
};

Palicross::Palicross(istream& fi) : n_filled(0)
{
    fi >> N >> M;
    R.reserve(N);
    copy_n(istream_iterator<string>(fi), N, back_inserter(R));
}

static bool dot_fill(char& c0, char& c1)
{
    bool fill = false;
    if ((c0 != '.') && (c1 == '.'))
    {
        fill = true;
        c1 = c0;
    }
    if ((c0 == '.') && (c1 != '.'))
    {
        fill = true;
        c0 = c1;
    }
    return fill;
}

void Palicross::solve_naive()
{
    bool filling = true;
    
    while (filling)
    {
        filling = false;
        for (size_t r = 0; r < N; ++r)
        {
            for (size_t c = 0; c < M; )
            {
                for ( ; (c < M) && (R[r][c] == '#'); ++c) {}
                const size_t cb = c;
                for ( ; (c < M) && (R[r][c] != '#'); ++c) {}
                const size_t ce = c;
                for (size_t i = cb, j = ce - 1; i < j; ++i, --j)
                {
                    if (dot_fill(R[r][i], R[r][j]))
                    {
                        filling = true;
                        ++n_filled;
                    }
                }
            }
        }
        for (size_t c = 0; c < M; ++c)
        {
            for (size_t r = 0; r < N; )
            {
                for ( ; (r < N) && (R[r][c] == '#'); ++r) {}
                const size_t rb = r;
                for ( ; (r < N) && (R[r][c] != '#'); ++r) {}
                const size_t re = r;
                for (size_t i = rb, j = re - 1; i < j; ++i, --j)
                {
                    if (dot_fill(R[i][c], R[j][c]))
                    {
                        filling = true;
                        ++n_filled;
                    }
                }
            }
        }
    }
}

static bool dot_fill_who(u_t& who, char& c0, char& c1)
{
    bool fill = false;
    if ((c0 != '.') && (c1 == '.'))
    {
        fill = true;
        c1 = c0;
        who = 1;
    }
    if ((c0 == '.') && (c1 != '.'))
    {
        fill = true;
        c0 = c1;
        who = 0;
    }
    return fill;
}

void Palicross::solve()
{
    solve_smart();
}

void Palicross::solve_smart()
{
    bool filling = true;
    setu_t rows, cols;
    for (u_t r = 0; r < N; ++r)
    {
        rows.insert(rows.end(), r);
    }
    for (u_t c = 0; c < M; ++c)
    {
        cols.insert(cols.end(), c);
    }
    u_t n_loops = 0;
    while (filling)
    {
        setu_t rows_next, cols_next;
        if (n_loops == 0)
        {
            cols_next = cols;
        }
        ++n_loops;
        filling = false;
        u_t who = 2;
        for (u_t r: rows)
        {
            for (size_t c = 0; c < M; )
            {
                for ( ; (c < M) && (R[r][c] == '#'); ++c) {}
                const size_t cb = c;
                for ( ; (c < M) && (R[r][c] != '#'); ++c) {}
                const size_t ce = c;
                for (size_t i = cb, j = ce - 1; i < j; ++i, --j)
                {
                    if (dot_fill_who(who, R[r][i], R[r][j]))
                    {
                        filling = true;
                        ++n_filled;
                        cols_next.insert(cols_next.end(), who == 0 ? i : j);
                    }
                }
            }
        }
        swap(cols, cols_next);
        for (size_t c: cols)
        {
            for (size_t r = 0; r < N; )
            {
                for ( ; (r < N) && (R[r][c] == '#'); ++r) {}
                const size_t rb = r;
                for ( ; (r < N) && (R[r][c] != '#'); ++r) {}
                const size_t re = r;
                for (size_t i = rb, j = re - 1; i < j; ++i, --j)
                {
                    if (dot_fill_who(who, R[i][c], R[j][c]))
                    {
                        filling = true;
                        ++n_filled;
                        rows_next.insert(rows_next.end(), who == 0 ? i : j);
                    }
                }
            }
        }
        swap(rows, rows_next);
    }
    if (dbg_flags & 0x1) { cerr << "#loops: " << n_loops << '\n'; }
}

void Palicross::print_solution(ostream &fo) const
{
    fo << ' ' << n_filled;
    for (const string row: R)
    {
        fo << '\n' << row;
    }
}

void pali_complete(vs_t& rows, u_t i, u_t j)
{
    const char c = rows[i][j];
    const int N = rows.size(), M = rows[0].size();

    // horizontal
    int l, r;;
    for (l = j; (l >= 0) && (rows[i][l] != '#'); --l) {}
    ++l;
    for (r = j; (r < M) && (rows[i][r] != '#'); ++r) {}
    --r;
    u_t jnew = r - (j - l);
    if (rows[i][jnew] == '.')
    {
        rows[i][jnew] = c;
        pali_complete(rows, i, jnew);
    }

    // vertocal
    int t, b;
    for (t = i; (t >= 0) && (rows[t][j] != '#'); --t) {}
    ++t;
    for (b = i; (b < N) && (rows[b][j] != '#'); ++b) {}
    --b;
    u_t inew = b - (i - t);
    if (rows[inew][j] == '.')
    {
        rows[inew][j] = c;
        pali_complete(rows, inew, j);
    }
}

void gen_palicross(vs_t& rows, u_t N, u_t M, u_t nc, u_t sharps)
{
    rows.clear();
    string dots(M, '.');
    rows = vs_t(N, dots);
    u_t max_del = N * M - sharps;
    u_t ndel = (max_del == 0 ? 0 : rand() % max_del);
    while (sharps > 0)
    {
        u_t i = rand() % N;
        u_t j = rand() % M;
        if (rows[i][j] == '.')
        {
            rows[i][j] = '#';
            --sharps;
        }
    }
    for (u_t i = 0; i < N ; ++i)
    {
        for (u_t j = 0; j < M; ++j)
        {
            if (rows[i][j] == '.')
            {
                rows[i][j] = 'A' + (rand() % nc);
                pali_complete(rows, i, j);
            }
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "complete, pre deletions:\n";
        for (const string& row: rows)
        {
            cerr << row << '\n';
        }
    }
    while (ndel > 0)
    {
        u_t i = rand() % N;
        u_t j = rand() % M;
        if ((rows[i][j] != '.') && (rows[i][j] != '#'))
        {
            rows[i][j] = '.';
            --ndel;
        }
    }
    if (dbg_flags & 0x2) {
        cerr << "After deletions:\n";
        for (const string& row: rows)
        {
            cerr << row << '\n';
        }
    }
}

int test(int argc, char ** argv)
{
    int rc = 0; 
    int ai = 0; 
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_max = strtoul(argv[ai++], 0, 0);
    u_t m_max = strtoul(argv[ai++], 0, 0);
    u_t c_max = strtoul(argv[ai++], 0, 0);
    u_t sharp_max = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = max<u_t>(1, rand() % (n_max + 1));
        u_t M = max<u_t>(1, rand() % (m_max + 1));
        u_t nc = max<u_t>(1, rand() % (c_max + 1));
        u_t sharps = min<u_t>(N * M, rand() % (sharp_max + 1));
        vs_t rows;
        gen_palicross(rows, N, M, nc, sharps);
        Palicross real(N, M, rows);
        real.solve();
        u_t n_filled;
        const vs_t& solution = real.get_solution(n_filled);
        Palicross naive(N, M, rows);
        real.solve_smart();
        u_t n_filled_naive;
        const vs_t& solution_naive = real.get_solution(n_filled_naive);
        if ((n_filled != n_filled_naive) || (solution != solution_naive))
        {
            rc = 1;
            cerr << "Inconsistent: original:\n1\n" << N << ' ' << M << '\n';
            for (const string& row: rows)
            {
                cerr << row << '\n';
            }
            cerr << "naive:";
            naive.print_solution(cerr);
            cerr << "\nreal:";
            real.print_solution(cerr);
            cerr << '\n';
        }
        else if (dbg_flags & 0x4)
        {
            real.print_solution(cerr); 
            cerr << '\n';
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
            exit(rc);
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

    void (Palicross::*psolve)() =
        (naive ? &Palicross::solve_naive : &Palicross::solve);
    if (solve_ver == 1) { psolve = &Palicross::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Palicross palicross(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palicross.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palicross.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
