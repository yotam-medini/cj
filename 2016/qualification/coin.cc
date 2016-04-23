// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

#include "get_primes.h"

using namespace std;

typedef mpz_class mpzc_t;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;
static ul_t primes_until = 0x1000;

static mpzc_t iroot(mpzc_t n)
{
    mpzc_t ret = 0;
    if (n > 0)
    {
        mpzc_t r = 1;
        while ((ret = (r + (n / r)) / 2) != r)
        {
            r = ret;
        }
    }
    return ret;
}

static mpzc_t ull2mpz(ull_t n)
{
    static mpzc_t two_p32("0x100000000");
    unsigned long u32high = (n >> 32) & 0xffffffff;
    unsigned long u32low = (n & 0xffffffff);
    mpzc_t ret(u32high);

    // ret <<= 32;
    // ret = (ret << 32);
    ret *= two_p32;

    ret += u32low;
    return ret;
}

static string sbin(ull_t n)
{
    static const string szero("0");
    static const string sone("1");
    string ret("");
    while (n > 0)
    {
        unsigned digit = n % 2;
        n /= 2;
        ret = string(digit == 1 ? sone : szero) + ret;
    }
    if (ret == string(""))
    {
        ret = string("0");
    }
    return ret;
}

class Jam
{
 public:
    Jam(unsigned v = 0) : n(v) { fill_n(divisors, 11, 0); }
    ull_t n;
    ul_t divisors[11];
};

class Coin
{
 public:
    Coin(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void check(ull_t n);
    mpzc_t bin_to_base(ull_t n_bin, mpzc_t base);
    unsigned N;
    unsigned J;
    vector<Jam> solution;
};

Coin::Coin(istream& fi)
{
    fi >> N >> J;
}

void Coin::solve()
{
    ull_t n = 1;
    n <<= (N - 1);
    ++n;
    while (solution.size() < J)
    {
        check(n);
        n += 2;
    }
}

void Coin::check(ull_t n_bin)
{
// cerr << "check("<<n_bin << "= 0b" << sbin(n_bin) << "\n";
    static const ul_t ul_max = ~(ul_t(0));
    static const mpzc_t mpz_ul_max = ull2mpz(ul_max);
    mpzc_t nb10 = bin_to_base(n_bin, 10);
    mpzc_t root = iroot(nb10);
    ul_t root_ul = root.get_ui();
    const vul_t &primes = get_primes(min(root_ul, primes_until));
    Jam jam(n_bin);
    bool is_prime = false;
    for (unsigned base = 2; (base <= 10) && !is_prime; ++base)
    {
        mpzc_t nb = bin_to_base(n_bin, base);
        ul_t p;
        for (auto pi = primes.begin();
            jam.divisors[base] == 0 && pi != primes.end() &&
            (p = *pi) && p*p < nb; ++pi) 
        {
            if (nb % p == 0)
            {
                jam.divisors[base] = p;
if (dbg_flags & 0x1) {
 cerr << "base="<< base << ", nb="<<nb << ", p="<< p << "\n"; }
            }
        }
        is_prime = jam.divisors[base] == 0;
    }
    if (!is_prime)
    {
        solution.push_back(jam);
if (dbg_flags & 0x2) { cerr << "solution[-1]=" << jam.n << " size=" << 
 solution.size() << "\n"; }
    }
}

mpzc_t Coin::bin_to_base(ull_t n_bin, mpzc_t base)
{
    mpzc_t ret = 0;
    ull_t bit = 1;
    mpzc_t power = 1;
    for (unsigned pos = 0; pos < N; ++pos, power *= base)
    {
        if (n_bin & (bit << pos))
        {
            ret += power;
        }
    }
    return ret;
}

void Coin::print_solution(ostream &fo)
{
    fo << "\n";
    for (auto i = solution.begin(), e = solution.end(); i != e; ++i)
    {
        const Jam& jam = *i;
        fo << sbin(jam.n);
        for (unsigned base = 2; base <= 10; ++base)
        {
            fo << ' ' << jam.divisors[base];
        }
        fo << "\n";
    }
}

int main(int argc, char ** argv)
{
// cerr << "sizeof(ull_t)=" << sizeof(ull_t) << "\n";
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    if (argc > 4) { primes_until = strtoul(argv[4], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Coin coin(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        coin.solve();
        fout << "Case #"<< ci+1 << ":";
        coin.print_solution(fout);
        // fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
