// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef long long ll_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;


template <ull_t mod>
class ModInt
{
 public:
    typedef ModInt<mod> self_t;
    ModInt(ll_t _n=0) : n((_n >= 0 ? _n : mod - (-_n) % mod) % mod) {}
    static constexpr std::uint32_t get_mod() { return mod; }
    ull_t get() const { return n; }
    static self_t add(const self_t& a, const self_t& b)
    {
        return self_t(a.n + b.n);
    }
    static self_t sub(const self_t& a, const self_t& b)
    {
        return self_t((a.n + mod) - b.n);
    }
    static self_t mul(const self_t& a, const self_t& b)
    {
        return self_t(a.n * b.n);
    }
    static self_t div(const self_t& a, const self_t& b)
    {
        return mul(a, b.inv());
    }
    static bool eq(const self_t& a, const self_t& b)
    {
        return a.n == b.n;
    }
    self_t pow(ull_t p) const
    {
        self_t ret = 1;
        self_t m = *this;;
        while (p)
        {
            if (p & 0x1)
            {
                ret = ret * m;
            }
            m = m * m;
            p /= 2;
        }
        return ret;
    }
    self_t inv() const
    {
        return pow(mod - 2); // since n^{mod - 1} == 1
    }
    self_t operator+=(const self_t& other)
    {
        n = ((*this) + other).get();
        return *this;
    }
 private:
    ull_t n;
};

template <ull_t mod>
ostream& operator<<(ostream& os, const ModInt<mod>& a)
{
    return os << a.get();
}

template <ull_t mod> ModInt<mod>
operator+(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::add(a, b);
}

template <ull_t mod> ModInt<mod>
operator-(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::sub(a, b);
}

template <ull_t mod> ModInt<mod>
operator*(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::mul(a, b);
}

template <ull_t mod> ModInt<mod>
operator/(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::div(a, b);
}

template <ull_t mod> bool
operator==(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::eq(a, b);
}

template <ull_t mod> bool
operator!=(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return !(a == b);
}

typedef ModInt<1000000007> gmodint_t;
typedef vector<gmodint_t> vgmodint_t;
typedef vector<vgmodint_t> vvgmodint_t;
typedef vector<vvgmodint_t> vvvgmodint_t;

gmodint_t gmodint_factorial(u_t n)
{
    static vector<gmodint_t> fact_cache;
    if (fact_cache.size() <= n)
    {
        fact_cache.reserve(n + 1);
        if (fact_cache.empty())
        {
            fact_cache.push_back(gmodint_t(1));
        }
        u_t k;
        while ((k = fact_cache.size()) <= n)
        {
            gmodint_t next = gmodint_t(k) * fact_cache.back();
            fact_cache.push_back(next);
        }
    }
    return fact_cache[n];
};

static unsigned dbg_flags;

class PaliDel
{
 public:
    PaliDel(istream& fi);
    PaliDel(const string& _S) : N(_S.size()), S(_S) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution.get(); }
 private:
    bool is_palindrome(const string& s) const;
    gmodint_t n_sz_combs(u_t sz) const
    {
        gmodint_t nc = gmodint_factorial(N - sz)*gmodint_factorial(sz);
        return nc;
    }
    u_t N;
    string S;
    gmodint_t solution;
};

PaliDel::PaliDel(istream& fi)
{
    fi >> N >> S;
}

void PaliDel::solve_naive()
{
    vu_t perm;
    while (perm.size() < N) { perm.push_back(perm.size()); }
    ull_t n_perms = 0, n_pali = 0;
    vgmodint_t sz_pali_count(N, gmodint_t(0));
    for (bool more = true; more;
        more = next_permutation(perm.begin(), perm.end()))
    {
        if (dbg_flags & 0x2) { cerr << "perm:";
            for (u_t i: perm) { cerr << ' ' << i; } cerr << '\n'; }
        ull_t active_mask = (1ull << N) - 1;
        for (u_t i = 0; i < N; ++i)
        {
            string sdel;
            active_mask &= ~(1ull << perm[i]); // delete bit
            for (u_t ci = 0; ci < N; ++ci)
            {
                if (active_mask & (1ull << ci))
                {
                    sdel.push_back(S[ci]);
                }
            }
            if (is_palindrome(sdel))
            {
                ++n_pali;
                sz_pali_count[sdel.size()] += gmodint_t(1);
                if (dbg_flags & 0x4) { cerr << sdel << " is Palindrome\n"; }
            }
        }
        ++n_perms;
    }
    if (dbg_flags & 0x1) {
        for (u_t sz = 0; sz < N; ++sz) {
            if (sz_pali_count[sz] != gmodint_t(0)) {
                cerr << "#("<<sz<<")=" << sz_pali_count[sz] << '\n'; } }
        cerr << "n_pali="<<n_pali << ", n_perms="<<n_perms << '\n'; }
    solution = gmodint_t(n_pali) / gmodint_t(n_perms);
}

void PaliDel::solve()
{
    const gmodint_t factN = gmodint_factorial(N);
    gmodint_t n_pali_ways;
    for (u_t even_odd = 0; even_odd < 2; ++even_odd)
    {
        u_t sz = even_odd;
        // [l][r] used iff l<r
        vvgmodint_t pali_inside_count(N, vgmodint_t(N, 0)); 
        for (u_t l = 0; l < N; ++l)
        {
            for (u_t r = l + 1; r < N; ++r)
            {
                pali_inside_count[l][r] = gmodint_t(sz == 0 ? 1 : (r - 1) - l);
            }
        }
        gmodint_t npw = n_sz_combs(sz) * gmodint_t(sz == 0 ? 1 : N);
        if (dbg_flags & 0x2) { cerr << "sz="<<sz << ", npw="<<npw << '\n'; }
        n_pali_ways += npw;
        for (sz += 2; sz < N; sz += 2)
        {
            vvgmodint_t pali_inside_count_next(N, vgmodint_t(N, 0));
            gmodint_t n_sz_pali;
            for (u_t rad = sz; rad <= N; ++rad)
            {
                for (u_t l = 0, r = rad - 1; r < N; ++l, ++r)
                {
                    gmodint_t n_inside;
                    if (S[l] == S[r])
                    {
                        n_inside = pali_inside_count[l][r];
                        n_sz_pali += n_inside;
                    }
                    if ((l > 0) && (r + 1 < N))
                    {
                        pali_inside_count_next[l - 1][r + 1] = n_inside
                            + pali_inside_count_next[l][r + 1]
                            + pali_inside_count_next[l - 1][r] 
                            - pali_inside_count_next[l][r];
                    }
                }
            }
            npw = n_sz_combs(sz) * n_sz_pali;
            if (dbg_flags & 0x2) { cerr << "sz="<<sz <<
                ", n_sz_pali="<<n_sz_pali << ", npw="<<npw << '\n'; }
            n_pali_ways += npw;
            swap(pali_inside_count, pali_inside_count_next);
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "n_pali_ways="<<n_pali_ways << ", factN="<<factN << '\n'; }
    solution = n_pali_ways / factN;
}

bool PaliDel::is_palindrome(const string& s) const
{
    bool isp = true;
    for (int i = 0, j = s.size() - 1; isp && (i < j); ++i, --j)
    {
        isp = s[i] == s[j];
    }
    return isp;
}

void PaliDel::print_solution(ostream &fo) const
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (PaliDel::*psolve)() =
        (naive ? &PaliDel::solve_naive : &PaliDel::solve);
    if (solve_ver == 1) { psolve = &PaliDel::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PaliDel paliDel(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (paliDel.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        paliDel.print_solution(fout);
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

static void save_case(const char* fn, const string& S)
{
    ofstream f(fn);
    f << "1\n" << S.size() << '\n' << S << '\n';
    f.close();
}

static int test_case(const string& S)
{
    int rc = 0;
    ull_t solution(-1), solution_naive(-1);
    const u_t N = S.size();
    bool small = (N <= 10);
    if (dbg_flags & 0x100) { save_case("palidel-curr.in", S); }
    if (small)
    {
        PaliDel p(S);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        PaliDel p(S);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("palidel-fail.in", S);
    }
    if (rc == 0) { 
        cerr << "N=" << N << " ";
        if (S.size() < 0x10) { 
            cerr << S; 
        } else {
            cerr << S.substr(0, 0x10) << " ...";
        }
        cerr << (small ? " (small) " : " (large) ") << " --> " <<
            solution << '\n';
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Cmax=" << Cmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        string S; S.reserve(N);
        while (S.size() < N)
        {
            S.push_back('a' + (rand() % Cmax));
        }
        rc = test_case(S);
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
