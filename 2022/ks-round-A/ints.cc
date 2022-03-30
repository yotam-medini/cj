// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <numeric>
#include <unordered_set>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;
typedef unordered_map<ull_t, ull_t> ull_to_ull_t;
typedef set<ull_t> set_ull_t;
typedef unordered_set<ull_t> uset_ull_t;


static unsigned dbg_flags;

static vull_t n_ints_upto;

void compute_n_ints_upto()
{
    // const ull_t Nmax = 1000000ull * 1000000ull + 1;
    const ull_t Nmax = 100002;
    n_ints_upto.reserve(Nmax + 1);
    n_ints_upto.push_back(0); 
    n_ints_upto.push_back(1); 
    vu_t digits; digits.push_back(1);
    ull_t n_interesting = 1;
    for (ull_t n = 1; n <= Nmax; ++n)
    {
        if ((dbg_flags & 0x1) && (n & (n - 1)) == 0)
        {
            cerr << __func__ << " " << n << "/" << Nmax << '\n';
        }
        u_t sum_digits = accumulate(digits.begin(), digits.end(), 0);
        u_t prod_digits = accumulate(digits.begin(), digits.end(), 1ull, 
            multiplies<u_t>());
        if (prod_digits % sum_digits == 0)
        {
            ++n_interesting;
        }
        n_ints_upto.push_back(n_interesting);
        
        u_t di = 0;
        while ((di < digits.size()) && (digits[di] == 9))
        {
            digits[di] = 0;
            ++di;
        }
        if (di < digits.size())
        {
            ++digits[di];
        }
        else
        {
            digits.push_back(1);
        }
    }
}

class Ints
{
 public:
    Ints(istream& fi);
    void solve_naive();
    void print_solution(ostream&) const;
 private:
    ull_t A, B;
    ull_t solution;
};

Ints::Ints(istream& fi) : solution(0)
{
    fi >> A >> B;
}

void Ints::solve_naive()
{
    solution = n_ints_upto[B + 1] - n_ints_upto[A];
}

void Ints::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static void compute_upto(
    const ull_t Nmax, 
    const set_ull_t& saved_pts,
    ull_to_ull_t& upto_interesting)
{
    vu_t asaved_pts(saved_pts.begin(), saved_pts.end());
    asaved_pts.push_back(0); // dummy
    u_t inext_upto = 0;
    ull_t next_upto = asaved_pts[inext_upto];
    vu_t digits; digits.push_back(1);
    ull_t n_interesting = 0;
    for (ull_t n = 1; n <= Nmax + 1; ++n)
    {
        if ((dbg_flags & 0x1) && (n & (n - 1)) == 0)
        {
            cerr << __func__ << " " << n << "/" << Nmax + 1 << '\n';
        }
        if ((n == next_upto) != (saved_pts.find(n) != saved_pts.end()))
        {
            cerr << "n="<<n << " software error\n";
        }
        // if (saved_pts.find(n) != saved_pts.end())
        if (n == next_upto)
        {
             ull_to_ull_t::value_type v{n, n_interesting};
             upto_interesting.insert(upto_interesting.end(), v);
             next_upto = asaved_pts[++inext_upto];
        }
        u_t sum_digits = accumulate(digits.begin(), digits.end(), 0);
        u_t prod_digits = accumulate(digits.begin(), digits.end(), 1ull, 
            multiplies<u_t>());
        if (prod_digits % sum_digits == 0)
        {
            ++n_interesting;
        }
        u_t di = 0;
        while ((di < digits.size()) && (digits[di] == 9))
        {
            digits[di] = 0;
            ++di;
        }
        if (di < digits.size())
        {
            ++digits[di];
        }
        else
        {
            digits.push_back(1);
        }
    }
}

static void solve(u_t n_cases, istream& fi, ostream& fo)
{
    vaull2_t cases;
    set_ull_t saved_pts;
    ull_to_ull_t upto_interesting;
    ull_t Nmax = 1;
    for (u_t i = 0; i < n_cases; ++i)
    {
        ull_t A, B;
        fi >> A >> B;
        cases.push_back(aull2_t{A, B});
        saved_pts.insert(saved_pts.end(), A);
        saved_pts.insert(saved_pts.end(), B + 1);
        Nmax = max(Nmax, B + 1);
    }
    compute_upto(Nmax, saved_pts, upto_interesting);
    for (u_t i = 0; i < n_cases; ++i)
    {
        const aull2_t& ab = cases[i];
        const ull_t upto_a = upto_interesting[ab[0]];
        const ull_t upto_b = upto_interesting[ab[1] + 1];
        const ull_t result = upto_b - upto_a;
        fo << "Case #" << (i + 1) << ": " << result << '\n';
    }
}

static int real_main(int argc, char ** argv)
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

    if (naive)
    {
        compute_n_ints_upto();
    }
    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Ints::*psolve)() = &Ints::solve_naive;
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; naive && (ci < n_cases); ci++)
    {
        Ints ints(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ints.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ints.print_solution(fout);
        fout << "\n";
        fout.flush();
    }
    if (!naive)
    {
        solve(n_cases, *pfi, fout);
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
