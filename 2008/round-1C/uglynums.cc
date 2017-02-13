// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;
typedef vector<unsigned> vu_t;
typedef vector<mpzc_t> vmpz_t;

class C210 // 2*3*5*7
{
 public:
    C210(): count{}, valid(false) {}
    mpzc_t count[210];
    bool valid;
};

static unsigned dbg_flags;


// 0 <= i < j < n.  [(0,1), ... (n-2,n-1)]  ==>  [0, n(n-1)/2)
static unsigned ijn_lt(unsigned i, unsigned j, unsigned n)
{
    unsigned ret = ((2*n - i - 3)*i)/2 + j - 1;
    return ret;
}

class UglyNums
{
 public:
    UglyNums(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool op_next();
    long compute() const;
    long next_num(unsigned& di) const;
    bool is_ugly(long n) const;
    void set_be_numbers();
    const C210& set_spectrum(unsigned b, unsigned e);
    vu_t digits;
    vu_t ops;
    mpzc_t solustion;
    vu_t be_numbers;
    vector<C210> be_spectrum;
};

UglyNums::UglyNums(istream& fi) : 
    solustion(0)
{
    string s;
    fi >> s;
    for (unsigned i = 0; i < s.size(); ++i)
    {
        unsigned d = s[i] - '0';
        digits.push_back(d);
    }
}

void UglyNums::solve_naive()
{
    ops = vu_t(digits.size(), 0);
    do
    {
        long n = compute();
        if (is_ugly(n))
        {
            ++solustion;
        }
    } while (op_next());
}

void UglyNums::print_solution(ostream &fo) const
{
    fo << " " << solustion;
}

long UglyNums::next_num(unsigned& di) const
{
    long ret = digits[di++];
    while (di < ops.size() && (ops[di] == 0))
    {
        ret = 10*ret + digits[di];
        ++di;
    }
    return ret;
}

long UglyNums::compute() const
{
    unsigned di = 0;
    long ret = next_num(di);

    while (di < ops.size())
    {
        bool plus = (ops[di] == 1);
        long nn = next_num(di);
        ret = (plus ? ret + nn : ret - nn);
    }

    return ret;
}

bool UglyNums::op_next()
{
    unsigned i;
    for (i = 1; (i < ops.size()) && ops[i] == 2; ++i) {}
    bool ret = (i < ops.size());
    if (ret)
    {
        ++(ops[i]);
        fill_n(ops.begin(), i, 0);
    }
    return ret;
}

void UglyNums::solve()
{
    const unsigned n = digits.size();
    const unsigned n_be = (n*(n + 1))/2;
    be_numbers.insert(be_numbers.end(), vu_t::size_type(n_be), unsigned(0));
    be_spectrum.insert(be_spectrum.end(), n_be, C210());
    set_be_numbers();
    const C210& spectrum = set_spectrum(0, n);
    for (unsigned k = 0; k < 210; ++k)
    {
        if (is_ugly(k))
        {
            solustion += spectrum.count[k];
        }
    }
}

void UglyNums::set_be_numbers()
{
    const unsigned n = digits.size();
    for (unsigned b = 0; b < n; ++b)
    {
        mpzc_t bignum(0);
        for (unsigned e = b, e1 = e + 1; e < n; e = e1++)
        {
            bignum = 10*bignum + digits[e];
            const unsigned bei = ijn_lt(b, e1, n + 1);
            mpzc_t bn_mod = bignum % 210;
            be_numbers[bei] = bn_mod.get_ui();
        }
    }
}

const C210& UglyNums::set_spectrum(unsigned b, unsigned e)
{
    const unsigned n = digits.size();
    const unsigned bei = ijn_lt(b, e, n + 1);
    C210& spectrum = be_spectrum[bei];
    if (!spectrum.valid)
    {
        unsigned w = be_numbers[bei];
        ++spectrum.count[w];
        for (unsigned c = b + 1; c < e; ++c)
        {
            const C210& bc_spectrum = set_spectrum(b, c);
            unsigned cei = ijn_lt(c, e, n + 1);
            unsigned ce = be_numbers[cei];
            for (unsigned k = 0; k < 210; ++k)
            {
                const mpzc_t& a = bc_spectrum.count[k];
                spectrum.count[(k + ce) % 210] += a;
                spectrum.count[(k + (210 - ce)) % 210] += a;
            }
        }
        spectrum.valid = true;
    }
    return spectrum;
}

bool UglyNums::is_ugly(long n) const
{
    if (n < 0) { n = -n; }
    bool ret = (n % 2 == 0);
    ret = ret || (n % 3 == 0);
    ret = ret || (n % 5 == 0);
    ret = ret || (n % 7 == 0);
    return ret;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (UglyNums::*psolve)() =
        (naive ? &UglyNums::solve_naive : &UglyNums::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        UglyNums problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
