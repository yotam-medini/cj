// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
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
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

u_t gcd(u_t m, u_t n)
{
    while (n)
    {
        u_t r = m % n;
        m = n;
        n = r;
    }
    return m;
}

class Exam
{
 public:
    Exam(istream& fi);
    Exam(const vs_t& _A, const vu_t& _S) :
        N(_A.size()), Q(A[0].size()), A(_A), S(_S)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    void get_solution(string& s, string& e) const 
    {
        s = solution; e = expectation;
    }
 private:
    void solve_N1();
    void solve_N2();
    void solve_N3();
    static char ft_other(char c) { return c == 'F' ? 'T' : 'F'; }
    u_t N, Q;
    vs_t A;
    vu_t S;
    string solution;
    string expectation;
};

Exam::Exam(istream& fi)
{
    fi >> N >> Q;
    for (u_t i = 0; i < N; ++i)
    {
        string a;
        u_t score;
        fi >> a >> score;
        A.push_back(a);
        S.push_back(score);
    }
}

void Exam::solve_naive()
{
    vu_t possibles;
    for (u_t bits = 0; bits < (1u << Q); ++bits)
    {
        bool possible = true;
        for (u_t i = 0; possible && (i < N); ++i)
        {
            u_t n_same = 0;
            for (u_t j = 0; j < Q; ++j)
            {
                if (((bits & (1u << j)) != 0) == (A[i][j] == 'T'))
                {
                    ++n_same;
                }
            }
            possible = (n_same == S[i]);
        }
        if (possible)
        {
            possibles.push_back(bits);
        }
    }
    u_t best_bits;
    u_t best_mscore = 0;
    for (u_t bits = 0; bits < (1u << Q); ++bits)
    {
        u_t mscore = 0;
        for (u_t i = 0; i < Q; ++i)
        {
            u_t pbit = bits & (1u << i);
            for (const u_t ap: possibles)
            {
                if (pbit == (ap & (1u << i)))
                {
                    ++mscore;
                }
            }
        }
        if (best_mscore < mscore)
        {
            best_mscore = mscore;
            best_bits = bits;
        }
    }
    for (u_t i = 0; i < Q; ++i)
    {
        solution.push_back(best_bits & (1u << 1) ? 'T' : 'F');
    }
    const u_t g = gcd(best_mscore, possibles.size());
    {
        ostringstream oss;
        oss << (best_mscore / g) << '/' << (possibles.size() / g);
        expectation = oss.str();
    }
}

void Exam::solve()
{
    switch (N)
    {
     case 1:
        solve_N1();
        break;
     case 2:
        solve_N2();
        break;
     case 3:
        solve_N3();
        break;
    }
}

void Exam::solve_N1()
{
    u_t e = S[0];
    if (2*S[0] >= Q)
    {
        solution = A[0];
        ostringstream oss;
    }
    else
    {
        for (char c: A[0])
        {
            solution.push_back(ft_other(c));
        }
        e = Q - S[0];
    }
    ostringstream oss;
    oss << e << "/1";
    expectation = oss.str();
}

void Exam::solve_N2()
{
    u_t n_equql = 0;
    for (u_t i = 0; i < Q; ++i)
    {
        n_equql += (A[0][i] == A[1][i] ? 1 : 0);
    }
    const u_t n_not_equal = Q - n_equql;
    
    u_t score_in_equal = (S[0] + S[1] - n_not_equal)/2;
    u_t score_in_not_equal[2];
    for (u_t i: {0, 1})
    {
        score_in_not_equal[i] = S[i] - score_in_equal;
    }
    const u_t i_better = (S[0] < S[1] ? 1 : 0);
    const u_t score_not_equal_better = score_in_not_equal[i_better];
    for (u_t i = 0; i < Q; ++i)
    {
        const char c = A[i_better][i];
        char csol = '?';
        if (A[0][i] == A[1][i])
        {
            csol = (2*score_in_equal >= n_equql) ? c : ft_other(c);
        }
        else
        {
            csol = (2*score_not_equal_better <= n_not_equal) ? ft_other(c) : c;
        }
        solution.push_back(csol);
    }
    u_t n_expect = max(score_in_equal, n_equql - score_in_equal) +
        score_not_equal_better;
    ostringstream oss;
    oss << n_expect << "/1";
    expectation = oss.str();
}

void Exam::solve_N3()
{
}

void Exam::print_solution(ostream &fo) const
{
    fo << ' ' << solution << ' ' << expectation;
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

    void (Exam::*psolve)() =
        (naive ? &Exam::solve_naive : &Exam::solve);
    if (solve_ver == 1) { psolve = &Exam::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Exam exam(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (exam.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        exam.print_solution(fout);
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
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
