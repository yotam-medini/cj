// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

class Students
{
 public:
    Students(istream& fi);
    Students(const vi_t& _R) : N(_R.size()), R(_R) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vi_t& get_solution() const { return solution; }
 private:
    u_t N;
    vi_t R;
    vi_t solution;
};

Students::Students(istream& fi)
{
    fi >> N;
    copy_n(istream_iterator<int>(fi), N, back_inserter(R));
}

void Students::solve_naive()
{
    for (const int r: R)
    {
        int mentor = -1;
        u_t n_self = 0;
        for (int m: R)
        {
            if (m == r)
            {
                ++n_self;
            }
            else if ((m <= 2*r) && (mentor < m))
            {
                mentor = m;
            }
        }
        if ((n_self > 1) && (mentor < r))
        {
            mentor = r;
        }
        solution.push_back(mentor);
    }
}

void Students::solve()
{
    vi_t r_sorted(R);
    sort(r_sorted.begin(), r_sorted.end());
    for (int r: R)
    {
        int i = lower_bound(r_sorted.begin(), r_sorted.end(), 2*r) -
            r_sorted.begin();
        if (i == int(N))
        {
            --i;
        }
        while ((i > 0) && (r_sorted[i] > 2*r))
        {
            --i;
        }
        if ((i >= 0) && (r_sorted[i] == r))
        {
            --i;
        }
        int mentor = (i >= 0 ? r_sorted[i] : -1);
        solution.push_back(mentor);
    }
}

void Students::print_solution(ostream &fo) const
{
    for (int m: solution)
    {
        fo << ' ' << m;
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

    void (Students::*psolve)() =
        (naive ? &Students::solve_naive : &Students::solve);
    if (solve_ver == 1) { psolve = &Students::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Students students(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (students.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        students.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vi_t& R)
{
    int rc = 0;
    vi_t solution, solution_naive;
    const u_t N = R.size();
    bool small = (N <= 10);
    if (small)
    {
        Students p{R};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Students p{R};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution= ";
        copy(solution.begin(), solution.end(),
            ostream_iterator<int>(cerr, " "));
        cerr << " != naive = ";
        copy(solution_naive.begin(), solution_naive.end(),
            ostream_iterator<int>(cerr, " "));
        cerr << '\n';
        ofstream f("students-fail.in");
        f << "1\n" << N << '\n';
        const char *sep = "";
        for (int r: R)
        { 
            f << sep << r; sep = " ";
        }
        f << '\n';
        f.close();
    }
    if ((rc == 0) && (N < 0x10))
    {
        copy(R.begin(), R.end(),
            ostream_iterator<int>(cerr, " "));
        cerr << " ==> ";
        copy(solution.begin(), solution.end(),
            ostream_iterator<int>(cerr, " "));
        cerr << '\n';
    }
    return rc;
}

void tuple_next(vi_t& t, const vi_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] <= bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 1);
        }
    }
    if (!done)
    {
        t.clear();
    }
}

static int test_small(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    // u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t N_min = strtoul(argv[ai++], 0, 0);
    u_t N_max = strtoul(argv[ai++], 0, 0);
    u_t R_max = strtoul(argv[ai++], 0, 0);
    ull_t rn = 1;
    ull_t n_tests = 0;
    for (u_t n = 1; n < N_min; ++n)
    {
        rn *= R_max;
    }
    for (u_t n = N_min; n <= N_max; ++n)
    {
        rn *= R_max;
        n_tests += rn;
    }
    ull_t ti = 0;
    for (u_t N = N_min; (rc == 0) && (N <= N_max); ++N)
    {
        const vi_t bound(N, R_max);
        for (vi_t R(N, 1); (rc == 0) && !R.empty(); ++ti, tuple_next(R, bound))
        {
            cerr << "Tested: " << ti << '/' << n_tests << '\n';
            rc = test_case(R);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_small(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
