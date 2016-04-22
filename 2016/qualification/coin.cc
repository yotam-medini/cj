// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

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
    Jam(unsigned v = 0) : n(v) {}
    ull_t n;
    ull_t divisors[11];
};

class Coin
{
 public:
    Coin(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void check(ull_t n);
    ull_t bin_to_base(ull_t n_bin, unsigned base);
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
    Jam jam(n_bin);
    unsigned base = 2;
    ull_t d = 3;
    while ((d * d < n_bin) && (n_bin % d != 0))
    {
        ++d;
    }
// cerr << "n_bin"<<n_bin << ", base="<<base << ", d?="<<d << "\n";
    jam.divisors[base] = d;
    bool prime = (n_bin % d != 0);
    for (unsigned base = 3; (base <= 10) && !prime; ++base)
    {
        ull_t nb = bin_to_base(n_bin, base);
        d = 2;
        while ((d * d < nb) && (nb % d != 0))
        {
            ++d;
        }
        prime = (nb % d != 0);
        if (!prime)
        {
// cerr << "n_bin"<<n_bin << ", base="<<base << ", d="<<d << "\n";
            jam.divisors[base] = d;
        }
    }
    if (!prime)
    {
        solution.push_back(jam);
    }
}

ull_t Coin::bin_to_base(ull_t n_bin, unsigned base)
{
    ull_t ret = 0;
    ull_t bit = 1;
    ull_t power = 1;
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
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

