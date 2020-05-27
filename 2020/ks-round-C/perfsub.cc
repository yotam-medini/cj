// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

vi_t squares;
void init_squares()
{
    int sq = 0;
    squares.reserve(10000);
    for (int k = 0; sq < 10000000; ++k)
    {
        sq = k*k;
        squares.push_back(sq);
    }
}

int iroot(int n)
{
    int ret = 0;
    if (n > 0)
    {
        int high = n + 1;
        while (ret + 1 < high)
        {
            int mid = (ret + high)/2;
            int sq = mid*mid;
            if (sq <= n)
            {
                ret = mid;
            }
            else
            {
                high = mid;
            }
        }
    }
    return ret;
}

bool is_square(int n)
{
    int r = iroot(n);
    bool ret = (r*r == n);
    return ret;
}

class PerfectSubArray
{
 public:
    PerfectSubArray(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vi_t a;
    ull_t solution;
};

PerfectSubArray::PerfectSubArray(istream& fi) : solution(0)
{
    fi >> n;
    copy_n(istream_iterator<int>(fi), n, back_inserter(a));
}

void PerfectSubArray::solve_naive()
{
    for (u_t b = 0; b < n; ++b)
    {
        for (u_t e = b + 1; e <= n; ++e)
        {
            int s = accumulate(a.begin() + b, a.begin() + e, 0);
            if (is_square(s))
            {
                ++solution;
            }
        }
    }
}

void PerfectSubArray::solve()
{
    typedef map<int, u_t> sum2count_t;
    sum2count_t sum2count;
    // sum2count.insert(sum2count.end(), sum2count_t::value_type(0, 1));
    int sum = 0;
    int sum_min = 0;
    int sum_max = a[0];
    int diff_max = max(0, sum_max);

    for (int x: a)
    {
        sum += x;
        if (sum_min > sum)
        {
            sum_min = sum;
            diff_max = sum_max - sum_min;
        }
        if (sum_max < sum)
        {
            sum_max = sum;
            diff_max = sum_max - sum_min;
        }
        for (int r = 0, sq = 0; sq <= diff_max; ++r, sq = r*r)
        {
            int presum = sum - sq;
            if (presum == 0)
            {
                ++solution;
            }
            sum2count_t::const_iterator iter = sum2count.find(presum);
            if (iter != sum2count.end())
            {
                solution += iter->second;
            }
        }
        auto er = sum2count.equal_range(sum);
        if (er.first == er.second)
        {
            sum2count.insert(er.first, sum2count_t::value_type(sum, 1));
        }
        else
        {
            ++(er.first->second);
        }
    }
}

void PerfectSubArray::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (PerfectSubArray::*psolve)() =
        (naive ? &PerfectSubArray::solve_naive : &PerfectSubArray::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    init_squares();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PerfectSubArray perfsub(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (perfsub.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        perfsub.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
