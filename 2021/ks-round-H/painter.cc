// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef unordered_map<char, u_t> c2u_t;

static unsigned dbg_flags;

class Painter
{
 public:
    Painter(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    enum {Red, Yellow, Blue};
    static void set_char2primary();
    static c2u_t char2primary;
    static u_t c2p(char c)
    {
        c2u_t::const_iterator iter = char2primary.find(c);
        if (iter == char2primary.end())
        {
            cerr << "table missing color: " << c << '\n';
        }
        u_t prims = iter->second;
        return prims;
    }
    u_t N;
    string P;
    u_t solution;
};
c2u_t Painter::char2primary;

Painter::Painter(istream& fi) : solution(0)
{
    fi >> N >> P;
}

void Painter::set_char2primary()
{
    if (char2primary.empty())
    {
        typedef c2u_t::value_type c2uv_t;
        const u_t B_Red = 1u << Red;
        const u_t B_Yellow = 1u << Yellow;
        const u_t B_Blue = 1u << Blue;

        char2primary.insert(char2primary.end(), c2uv_t{'U', 0});

        char2primary.insert(char2primary.end(), c2uv_t{'R', B_Red});
        char2primary.insert(char2primary.end(), c2uv_t{'Y', B_Yellow});
        char2primary.insert(char2primary.end(), c2uv_t{'B', B_Blue});

        char2primary.insert(char2primary.end(), c2uv_t{'O', B_Red | B_Yellow});
        char2primary.insert(char2primary.end(), c2uv_t{'P', B_Red | B_Blue});
        char2primary.insert(char2primary.end(), c2uv_t{'G', B_Yellow | B_Blue});

        char2primary.insert(char2primary.end(), c2uv_t{'A', 
                            B_Red | B_Yellow | B_Blue});
    }
}

void Painter::solve_naive()
{
    set_char2primary();
    for (u_t primary: {0, 1, 2})
    {
        const u_t bp = 1u << primary;
        for (u_t i = 0; i < N; )
        {
            for (; (i < N) && ((bp & c2p(P[i])) == 0); ++i) {}
            if (i < N)
            {
                ++solution;
                for (; (i < N) && ((bp & c2p(P[i])) != 0); ++i) {}
            }
        }
    }
}

void Painter::solve()
{
    solve_naive();
}

void Painter::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (Painter::*psolve)() =
        (naive ? &Painter::solve_naive : &Painter::solve);
    if (solve_ver == 1) { psolve = &Painter::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Painter painter(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (painter.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        painter.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
