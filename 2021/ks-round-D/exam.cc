// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Exam
{
 public:
    Exam(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vull_t& get_solution() const { return solution; }
 private:
    ul_t N, M;
    vaull2_t ABs;
    vull_t S;
    vull_t solution;
};

Exam::Exam(istream& fi)
{
    fi >> N >> M;
    ABs.reserve(N);
    S.reserve(M);
    while (ABs.size() < N)
    {
        aull2_t ab;
        fi >> ab[0] >> ab[1];
        ABs.push_back(ab);
    }
    copy_n(istream_iterator<ull_t>(fi), M, back_inserter(S));
}

void Exam::solve_naive()
{
    ull_t max_exam = 0;
    for (const aull2_t& ab: ABs)
    {
        max_exam = max(max_exam, ab[1]);
    }
    if (dbg_flags & 0x1) { cerr << "max_exam=" << max_exam << '\n'; }
    vector<bool> exams(size_t(max_exam + 1), false);
    for (const aull2_t& ab: ABs)
    {
        for (ull_t i = ab[0]; i <= ab[1]; ++i)
        {
            exams[i] = true;
        }
    }
    solution.reserve(M);
    for (ull_t si = 0; si < M; ++si)
    {
        const ull_t skill = S[si];
        ull_t exam = ull_t(-1); // undef
        for (ull_t step = 0; (exam == ull_t(-1)); ++step)
        {
            // if (skill - step >= 0)
            if ((skill >= step) && exams[skill - step])
            {
                exam = skill - step;
                exams[exam] = false;
            }
            else if ((skill + step <= max_exam) && exams[skill + step])
            {
                exam = skill + step;
                exams[exam] = false;
            }
        }
        solution.push_back(exam);
    }
}

void Exam::solve()
{
    solve_naive();
}

void Exam::print_solution(ostream &fo) const
{
    for (ull_t x: solution)
    {
        fo << ' ' << x;
    }
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

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
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
        rc = test_case(argc, argv);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
