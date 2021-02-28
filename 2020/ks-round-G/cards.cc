// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
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
typedef vector<ull_t> vull_t;
typedef vector<double> vd_t;
typedef vector<vd_t> vvd_t;

static unsigned dbg_flags;

class Cards
{
 public:
    Cards(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    const vd_t& get_factors(u_t k);
    double naive_expect(const vull_t& sub) const;
    u_t n;
    vull_t cards;
    double solution;
};

Cards::Cards(istream& fi) : solution(0)
{
    fi >> n;
    cards.reserve(n);
    copy_n(istream_iterator<ull_t>(fi), n, back_inserter(cards));
}

void Cards::solve_naive()
{
    solution = naive_expect(cards);
}

double Cards::naive_expect(const vull_t& sub) const
{
    double ret = 0.;
    const u_t sz = sub.size();
    if (sz > 1)
    {
        double d = sz - 1;
        double total = 0;
        for (u_t i0 = 0, i1 = 1; i1 < sz; i0 = i1++)
        {
            ull_t anew = sub[i0] + sub[i1];
            total += anew;
            vull_t subsub;
            subsub.insert(subsub.end(), sub.begin(), sub.begin() + i0);
            subsub.push_back(anew);
            subsub.insert(subsub.end(), sub.begin() + i1 + 1, sub.end());
            total += naive_expect(subsub);
        }
        ret = total / d;
    }
    return ret;
}

void Cards::solve()
{
    const vd_t& fn = get_factors(n);
    for (u_t i = 0; i < n; ++i)
    {
        solution += fn[i] * cards[i];
    }
}

const vd_t& Cards::get_factors(u_t k)
{
    static vvd_t factors;
    if (factors.size() <= k)
    {
        if (factors.empty())
        {
            vd_t zo;
            zo.push_back(0.);
            factors.push_back(zo); // [0] = {0.}
            zo[0] = 1;
            factors.push_back(zo); // [1] = {1.}
            zo.push_back(1.);
            factors.push_back(zo); // [2] = {1., 1.}
        }
        while (factors.size() <= k)
        {
            double total = 0.;
            u_t l = factors.size();
            vd_t f; f.reserve(l);
            for (u_t j = 0; j < (l + 1)/2; ++j)
            {
                total = (j == 0 ? 1. : j * factors[l - 1][j - 1] + 2.);;
                total += (l - j - 1) * factors[l - 1][j];
                double q = total / double(l - 1);
                f.push_back(q);
            }
            for (u_t j = f.size(); j < l; ++j)
            {
                f.push_back(f[l - j - 1]); // mirror - palindrome
            }
            factors.push_back(f);
        }
    }
    return factors[k];
}

void Cards::print_solution(ostream &fo) const
{
    ostringstream os;
    os.precision(8);
    os.setf(ios::fixed);
    os << solution;
    string s = os.str();
    while (s.back() == '0')
    {
        s.pop_back();
    }
    fo << ' ' << s;
}

int main(int argc, char ** argv)
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Cards::*psolve)() =
        (naive ? &Cards::solve_naive : &Cards::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cards cards(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cards.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cards.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
