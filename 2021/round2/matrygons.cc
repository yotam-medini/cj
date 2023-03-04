// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef map<u_t, u_t> u_to_u_t;
typedef vector<u_to_u_t> v_u_to_u_t;

static unsigned dbg_flags;

static u_t N_MAX = 1000;
// static u_t N_MAX = 1000000;

static vu_t primes;

static void set_primes()
{
    vector<bool> sieve(N_MAX + 1, true);
    primes.clear();
    for (u_t n = 2; n <= N_MAX; ++n)
    {
         if (sieve[n])
         {
             primes.push_back(n);
             for (u_t k = n*n; k <= N_MAX; k += n)
             {
                 sieve[k] = false;
             }
         }
    }
}

class NumPrimes
{
 public:
    NumPrimes(u_t _n=1, const vau2_t& pp = vau2_t()) :
        n(_n), primes_powers(pp) {}
    NumPrimes(u_t prime, const NumPrimes& base);
    u_t n;
    vau2_t primes_powers;
};

NumPrimes::NumPrimes(u_t prime, const NumPrimes& base) : n(prime * base.n)
{
    const vau2_t& pps = base.primes_powers;
    primes_powers.reserve(pps.size() + 1);
    u_t i = 0;
    for (; (i < pps.size()) && (pps[i][0] < prime); ++i)
    {
        primes_powers.push_back(pps[i]);
    }
    if (i == pps.size())
    {
        primes_powers.push_back(au2_t{prime, 1});
    }
    else
    {
        if (pps[i][0] == prime)
        {
            primes_powers.push_back(au2_t{prime, pps[i][1] + 1});
            ++i;
        }
        else // prime < pps[i][0]
        {
            primes_powers.push_back(au2_t{prime, 1});
        }
        for ( ; i < pps.size(); ++i)
        {
            primes_powers.push_back(pps[i]);
        }
    }
}

bool operator<(const NumPrimes& np0, const NumPrimes& np1)
{
    return np0.n < np1.n;
}
bool operator>(const NumPrimes& np0, const NumPrimes& np1)
{
    return np0.n > np1.n;
}

typedef vector<NumPrimes> vnumprimes_t;

class Matrygons
{
 public:
    // Matrygons(istream& fi);
    Matrygons(u_t _N) : N(_N), solution(0) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t best_under(u_t n, u_t target);
    void set_solution_map();
    void maximize_by(u_to_u_t& m, const u_to_u_t& by);
    u_t N;
    u_t solution;
    static vu_t solution_map;
};
vu_t Matrygons::solution_map;

void Matrygons::solve_naive()
{
    for (u_t p = 3; p <= N; ++p)
    {
        u_t np = best_under(p, N);
        if (solution < np)
        {
            solution = np;
        }
    }
}

u_t Matrygons::best_under(u_t n, u_t target)
{
    u_t best = (target == n ? 1 : 0);
    if ((3 <= n) && (n < target))
    {
        target -= n;
        for (u_t d = 3; 2*d <= n; ++d)
        {
            if (n % d == 0)
            {
                u_t np = best_under(d, target);
                if ((np > 0) && (best < np + 1))
                {
                    best = np + 1;
                }
            }     
        }
    }
    return best;
}

void Matrygons::solve()
{
    if (primes.empty())
    {
        set_primes();
    }
    if (solution_map.empty())
    {
        set_solution_map();
    }
    solution = solution_map[N];
}

void Matrygons::set_solution_map()
{
    vector<bool> numbers_used(N_MAX + 1, false);
    solution_map = vu_t(N_MAX + 1, 1);
    vnumprimes_t heap;
    vau2_t ppower1;
    ppower1.push_back(au2_t{2, 1});
    for (u_t pi = 1; pi < primes.size(); ++pi)
    {
        const u_t prime = primes[pi];
        ppower1[0][0] = prime;
        heap.push_back(NumPrimes(prime, ppower1));
        numbers_used[prime] = true;
    }
    const greater<NumPrimes> gt;
    make_heap(heap.begin(), heap.end(), gt);

    const u_t n_primes = primes.size();
    v_u_to_u_t num_target_sum_to_count(N_MAX + 1, u_to_u_t());
    while (!heap.empty())
    {
        const NumPrimes curr = heap.front(); // copy
        const vau2_t& pps = curr.primes_powers;
        u_to_u_t sum2count;
        sum2count.insert(sum2count.end(), u_to_u_t::value_type(curr.n, 1));
        if ((pps.size() != 1) || (pps[0][1] != 1))
        {
            // not a trivial prime.
            for (const au2_t& pp: pps)
            {
                const u_t d = curr.n / pp[0];
                const u_to_u_t& sub_t2n = num_target_sum_to_count[d];
                maximize_by(sum2count, sub_t2n);
                u_t target;
                for (u_to_u_t::const_iterator siter = sub_t2n.begin();
                    (siter != sub_t2n.end()) && 
                        ((target = (curr.n + siter->first)) <= N_MAX);
                    ++siter)
                {
                    const u_t count1 = siter->second + 1;
                    u_to_u_t::iterator iter = sum2count.find(target);
                    u_to_u_t::value_type new_val{target, count1};
                    if (iter == sum2count.end())
                    {
                        iter = sum2count.insert(iter, new_val);
                    }
                    else if (iter->second < count1)
                    {
                        iter->second = count1;
                    }
                    if (solution_map[target] < count1)
                    {
                        solution_map[target] = count1;
                    }                    
                }
            }
        }
        num_target_sum_to_count[curr.n] = sum2count;
        for (u_t pi = 0; (pi < n_primes) && (curr.n * primes[pi] <= N_MAX);
            ++pi)
        {
            const u_t prime = primes[pi];
            u_t new_number = prime * curr.n;
            if (!numbers_used[new_number])
            {
                heap.push_back(NumPrimes(prime, curr));
                push_heap(heap.begin(), heap.end(), gt);
                numbers_used[new_number] = true;
            }
        }
        pop_heap(heap.begin(), heap.end(), gt);
        heap.pop_back();
    }
}

void Matrygons::maximize_by(u_to_u_t& m, const u_to_u_t& by)
{
    for (const u_to_u_t::value_type& kv: by)
    {
        const u_t key = kv.first, val = kv.second;
        u_to_u_t::iterator iter = m.find(key);
        if (iter == m.end())
        {
            m.insert(iter, kv);
        }
        else if (iter->second < val)
        {
            iter->second = val;
        }
    }
}

void Matrygons::print_solution(ostream &fo) const
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

    void (Matrygons::*psolve)() =
        (naive ? &Matrygons::solve_naive : &Matrygons::solve);
    if (solve_ver == 1) { psolve = &Matrygons::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    vu_t inputs;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        u_t n;
        (*pfi) >> n;
        inputs.push_back(n);
    }
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Matrygons matrygons(inputs[ci]);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (matrygons.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        matrygons.print_solution(fout);
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

static void save_case(const char* fn, u_t N)
{
    ofstream f(fn);
    f << "1\n" << N << '\n';
    f.close();
}

static int test_case(u_t N)
{
    int rc = rand_range(0, 1);
    u_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("matrygons-curr.in", N); }
    if (small)
    {
        Matrygons p{N};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Matrygons p{N};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("matrygons-fail.in", N);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
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
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const u_t n_tests = Nmax + 1 - Nmin;
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (u_t ti = 0, N = Nmin; (rc == 0) && (ti < n_tests); ++ti, ++N)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(N);
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
