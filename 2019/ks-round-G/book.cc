// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Book
{
 public:
    Book(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t n, m, q;
    vull_t p;
    vull_t r;
    ull_t solution;
};

Book::Book(istream& fi) : solution(0)
{
    fi >> n >> m >> q;
    p.reserve(m);
    r.reserve(q);
    ull_t x;
    for (ull_t i = 0; i < m; ++i)
    {
        fi >> x;
        p.push_back(x);
    }
    for (ull_t i = 0; i < q; ++i)
    {
        fi >> x;
        r.push_back(x);
    }
}

void Book::solve_naive()
{
    for (ull_t ri = 0; ri < q; ++ri)
    {
        ull_t dp = r[ri];
        for (ull_t page = dp; page <= n; page += dp)
        {
            if (!binary_search(p.begin(), p.end(), page))
            {
                ++solution;
            }
            else
            {
                if (dbg_flags & 0x1) {
                    cerr << "Skip dp="<<dp << ", page="<<page << '\n'; }
            }
        }
    }
}

void Book::solve()
{
    sort(r.begin(), r.end());
    for (ull_t ri = 0; ri < q; ++ri)
    {
        ull_t dp = r[ri];
        ull_t nread = n / dp;
        solution += nread;
    }
    for (ull_t ti = 0; ti < m; ++ti)
    {
        ull_t t = p[ti];
        for (ull_t divisor = 1; divisor * divisor <= t; ++divisor)
        {
            if (t % divisor == 0)
            {
                auto er = equal_range(r.begin(), r.end(), divisor);
                solution -= (er.second - er.first);
                ull_t odiv = t / divisor;
                if (odiv != divisor)
                {
                    er = equal_range(r.begin(), r.end(), odiv);
                    solution -= (er.second - er.first);
                }
            }
        }
    }
}

#if 0
void Book::solve()
{
    for (ull_t ri = 0; ri < q; ++ri)
    {
        ull_t dp = r[ri];
        ull_t nread = n / dp;
        for (ull_t ti = 0; ti < m; ++ti)
        {
            ull_t t = p[ti];
            if (t % dp == 0)
            {
                --nread;
            }
        }
        solution += nread;
    }
}
#endif

void Book::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Book::*psolve)() =
        (naive ? &Book::solve_naive : &Book::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Book book(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (book.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        book.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
