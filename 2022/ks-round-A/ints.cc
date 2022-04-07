// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <numeric>
#include <unordered_map>
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
typedef array<u_t, 3> au3_t;
typedef array<u_t, 4> au4_t;
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
        if ((dbg_flags & 0x1) && ((n & (n - 1)) == 0))
        {
            cerr << __func__ << " " << n << "/" << Nmax << '\n';
        }
        u_t sum_digits = accumulate(digits.begin(), digits.end(), 0);
        u_t prod_digits = accumulate(digits.begin(), digits.end(), 1ull,
            multiplies<u_t>());
        if (prod_digits % sum_digits == 0)
        {
            ++n_interesting;
            if ((dbg_flags & 0x2) && (n <= 100)) {
                cerr << "Interesting: " << n << '\n'; }
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

void get_digits(vu_t& digits, ull_t n)
{
    digits.clear();
    while (n > 0)
    {
        digits.push_back(n % 10);
        n /= 10;
    }
    reverse(digits.begin(), digits.end());
}

template <std::size_t N>
class Hash_AUN {
 public:
    typedef array<u_t, N> auN_t;
    size_t operator()(const auN_t& a) const
    {
        ull_t n = a[0];
        for (u_t i = 1; i < N; ++i)
        {
            n ^= (ull_t(a[i]) << (30/N));
        }
        return hash_uint(n);
    }
 private:
    hash<ull_t> hash_uint;
};
typedef Hash_AUN<3> Hash_AUN3_t;
typedef Hash_AUN<4> Hash_AUN4_t;

class P2357
{
 public:
    P2357(u_t _n, const au4_t& _powers) : n(_n), powers(_powers) {}
    void digit_mult(u_t digit);
    static const P2357 p2357_digit[10];
    u_t n;
 private:
    static const u_t caps_power[];
    static const u_t low_primes[];
    au4_t powers;
};
const u_t P2357::caps_power[] = {6, 4, 2, 2};
const u_t P2357::low_primes[] = {2, 3, 5, 7};
const P2357 P2357::p2357_digit[10] =
{              //  2  3  5  7
    P2357(0, au4_t{0, 0, 0, 0}),
    P2357(1, au4_t{0, 0, 0, 0}),
    P2357(2, au4_t{1, 0, 0, 0}),
    P2357(3, au4_t{0, 1, 0, 0}),
    P2357(4, au4_t{2, 0, 0, 0}),
    P2357(5, au4_t{0, 0, 1, 0}),
    P2357(6, au4_t{1, 1, 0, 0}),
    P2357(7, au4_t{0, 0, 0, 1}),
    P2357(8, au4_t{3, 0, 0, 0}),
    P2357(9, au4_t{0, 2, 0, 0})
};


void P2357::digit_mult(u_t digit)
{
    if (digit == 0)
    {
        n = 0;
        powers = au4_t{0, 0, 0, 0};
    }
    else
    {
        for (u_t i = 0; i < 4; ++i)
        {
            const u_t cap_power = caps_power[i];
            const u_t low_prime = low_primes[i];
            const u_t pp = p2357_digit[digit].powers[i];
            for (u_t p = 0; (p < pp) && (powers[i] < cap_power); ++p)
            {
                ++powers[i];
                n *= low_prime;
            }
        }
   }
}

class Method
{
 public:
    Method() {};
    virtual ~Method() {}
    virtual ull_t interesting_upto(ull_t N) = 0;
};

class Method1 : public Method
{
 public:
    Method1() {}
    ull_t interesting_upto(ull_t N);
 private:
    ull_t f1(u_t L, const P2357& P, u_t S);
    ull_t f1(u_t L, u_t P, u_t S) { return f1(L, P2357::p2357_digit[P], S); }
    ull_t interesting_with_n_digits(u_t n_digits);
    ull_t interesting_with_prefix(
        const vu_t& digits,
        const P2357& P,
        u_t S,
        u_t d_index);
};

ull_t Method1::interesting_upto(ull_t N)
{
    ull_t count = 0;
    if (N > 0)
    {
        --N; // upto nut not including
        vu_t digits;
        get_digits(digits, N);
        const u_t n_digits = digits.size();
        for (u_t nd = 1; nd < n_digits; ++nd)
        {
            count += interesting_with_n_digits(nd);
        }
        count += interesting_with_prefix(digits, P2357::p2357_digit[1], 0, 0);
    }
    if (dbg_flags & 0x1) { cerr <<__func__<<" N="<<N<< ", count="<<count<<'\n'; }
    return count;
}

ull_t Method1::interesting_with_n_digits(u_t L)
{
    ull_t count = 0;
    for (u_t digit = 1; digit <= 9; ++digit)
    {
        count += f1(L - 1, P2357::p2357_digit[digit], digit);
    }
    return count;
}

ull_t Method1::interesting_with_prefix(
    const vu_t& digits,
    const P2357& P,
    u_t S,
    u_t d_index)
{
    ull_t count = 0;
    if (d_index == digits.size())
    {
        count = ((S > 0) && (P.n % S == 0) ? 1 : 0);
    }
    else
    {
        const u_t digit_limit = digits[d_index];
        const u_t L = digits.size() - d_index - 1;
        for (u_t digit = (d_index == 0 ? 1 : 0); digit < digit_limit; ++digit)
        {
            P2357 dP(P);
            dP.digit_mult(digit);
            count += f1(L, dP, S + digit);
        }
        P2357 dP(P);
        const u_t digit = digits[d_index];
        dP.digit_mult(digit);
        count += interesting_with_prefix(digits, dP, S + digit, d_index + 1);
    }
    return count;
}

ull_t Method1::f1(u_t L, const P2357& P, u_t S)
{
    ull_t ret = 0;
    if (L == 0)
    {
        ret = (P.n % S == 0) ? 1 : 0;
    }
    else
    {
        typedef unordered_map<au3_t, ull_t, Hash_AUN3_t> memo_t;
        static memo_t memo;
        memo_t::key_type key{L, P.n, S};
        memo_t::iterator iter = memo.find(key);
        if (iter == memo.end())
        {
            for (u_t digit = 0; digit <= 9; ++digit)
            {
                P2357 dP(P);
                dP.digit_mult(digit);
                ret += f1(L - 1, dP, S + digit);
            }
            iter = memo.insert(memo.end(), memo_t::value_type(key, ret));
        }
        else
        {
            ret = iter->second;
        }
    }
    return ret;
}

class Method2 : public Method
{
 public:
    Method2() {}
    ull_t interesting_upto(ull_t N);
 private:
    ull_t interesting_with_n_digits(u_t S_target, u_t L);
    ull_t interesting_with_prefix(
        u_t S_target,
        ull_t N,
        const vu_t& digits,
        ull_t P,
        u_t S,
        u_t d_index,
        bool first_digit);
    ull_t f2(u_t L, u_t P, u_t S, u_t S_target);
};

ull_t Method2::interesting_upto(ull_t N)
{
    ull_t count = 0;
    if (N > 0)
    {
        --N; // upto nut not including
        vu_t digits;
        get_digits(digits, N);
        const u_t n_digits = digits.size();
        for (u_t S_target = 1; S_target <= 9*n_digits; ++S_target)
        {
            ull_t st_count = 0;
            for (u_t L = 1; L < n_digits; ++L)
            {
                st_count += interesting_with_n_digits(S_target, L);
            }
            st_count += interesting_with_prefix(S_target, N, digits, 1, 0, 0,
                true);
            if (dbg_flags & 0x2) { cerr << "S_target="<<S_target <<
                ", st_count="<<st_count << '\n'; }
            count += st_count;
        }
    }
    if (dbg_flags & 0x1) { cerr <<__func__<<" N="<<N<< ", count="<<count<<'\n'; }
    return count;
}

ull_t Method2::interesting_with_n_digits(u_t S_target, u_t L)
{
    ull_t count = 0;
    for (u_t digit = 1; digit <= 9; ++digit)
    {
        count += f2(L - 1, digit % S_target, digit, S_target);
    }
    return count;
}

ull_t Method2::interesting_with_prefix(
    u_t S_target,
    ull_t N,
    const vu_t& digits,
    ull_t P,
    u_t S,
    u_t d_index,
    bool first_digit)
{
    ull_t count = 0;
    const u_t n_digits = digits.size();
    if (d_index == n_digits)
    {
        if ((P % S == 0) && (S == S_target))
        {
            count = 1;
        }
    }
    else
    {
        const u_t curr_digit = digits[d_index];
        for (u_t digit = (first_digit ? 1 : 0); digit < curr_digit; ++digit)
        {
            u_t Pd = (P * digit) % S_target;
            count += f2(n_digits - d_index - 1, Pd, S + digit, S_target);
        }
        count += interesting_with_prefix(
            S_target, 
            N, digits,
            (P * curr_digit) % S_target,
            S + curr_digit,
            d_index + 1,
            false);
    }
    return count;
}

ull_t Method2::f2(u_t L, u_t P, u_t S, u_t S_target) 
{ 
    ull_t ret = 0;
    if (L == 0)
    {
        if ((S == S_target) && (P % S_target == 0))
        {
            ret = 1;
        }
    }
    else
    {
        typedef unordered_map<au4_t, ull_t, Hash_AUN4_t> memo_t;
        static memo_t memo;
        memo_t::key_type key{L, P, S, S_target};
        memo_t::iterator iter = memo.find(key);
        if (iter == memo.end())
        {
            for (u_t digit = 0; digit <= 9; ++digit)
            {
                u_t dP = (P * digit) % S_target;
                ret += f2(L - 1, dP, S + digit, S_target);
            }
            iter = memo.insert(memo.end(), memo_t::value_type(key, ret));
        }
        else
        {
            ret = iter->second;
        }
    }
    return ret;
}

static void compute_upto(
    Method& method,
    const ull_t Nmax,
    const set_ull_t& saved_pts,
    ull_to_ull_t& upto_interesting)
{
    for (ull_t pt: saved_pts)
    {
        upto_interesting[pt] = method.interesting_upto(pt);
    }
}

static void solve(Method& method, u_t n_cases, istream& fi, ostream& fo)
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
    compute_upto(method, Nmax, saved_pts, upto_interesting);
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
    Method1 method1;
    Method2 method2;
    Method* pmethod = &method1;
    int rc = 0, ai;
    
    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-method1"))
        {
            pmethod = &method1;
        }
        else if (opt == string("-method2"))
        {
            pmethod = &method2;
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
        solve(*pmethod, n_cases, *pfi, fout);
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
