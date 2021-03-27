// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

bool subsetsum_rec(vu_t& subset, const vu_t& a, u_t sz, u_t sum)
{
    bool ret = true;
    if (sum > 0)
    {
        ret = false;
        if (sz > 0)
        {
           u_t last = a[sz - 1];
           if (last <= sum)
           {
               subset.push_back(last);
               ret = subsetsum_rec(subset, a, sz - 1, sum - last);
               if (!ret)
               {
                   subset.pop_back();
               }
           }
           if (!ret)
           {
               ret = subsetsum_rec(subset, a, sz - 1, sum);
           }
        }
    }
    return ret;
}

bool subsetsum(vu_t& subset, const vu_t& a, u_t sum)
{
    vu_t sa(a);
    sort(sa.begin(), sa.end());
    subset.clear();
    bool ret = subsetsum_rec(subset, sa, a.size(), sum);
    return ret;
}

class RevEng
{
 public:
    RevEng(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t& get_solution() const { return solution; }
    bool possible() const { return !solution.empty(); }
    u_t solution_cost() const { return cost(solution); }
 private:
    u_t cost(const vu_t& a) const;
    void reverse_engine(const vu_t& sub_costs);
    u_t N, C;
    vu_t solution;
};

RevEng::RevEng(istream& fi)
{
    fi >> N >> C;
}

void RevEng::solve_naive()
{
    vu_t perm;
    permutation_first(perm, N);
    for (bool more = true; more && (solution.empty() || (dbg_flags & 0x1));
        more = permutation_next(perm))
    {
        u_t pcost = cost(perm);
        if (pcost == C)
        {
            solution.reserve(N);
            solution.clear();
            for (u_t x: perm)
            {
                solution.push_back(x + 1);
                if (dbg_flags & 0x2) { cerr << ' ' << x + 1; }
            }
            if (dbg_flags & 0x2) { cerr << '\n'; }
        }
    }
}

void RevEng::solve()
{
    if ((N - 1 <= C) && (C <= ((N + 2)*(N - 1))/2))
    {
        vu_t all_costs; all_costs.reserve(N - 1);
        vu_t sub_costs(size_t(N - 1), 1);
        u_t pending = C - (N - 1);
        for (u_t i = 0; i < N - 1; ++i)
        {
            u_t add = min(N - 1 - i, pending);
            sub_costs[i] += add;
            pending -= add;
        }
#if 0
        for (u_t sc = 1; sc <= N - 1; ++sc)
        {
            all_costs.push_back(sc);
        }
        vu_t sub_costs;
        bool possible = subsetsum(sub_costs, all_costs, C);
        if (possible)
#endif
        {
            sort(sub_costs.begin(), sub_costs.end());
            reverse_engine(sub_costs);
        }
    }
}

u_t RevEng::cost(const vu_t& a) const
{
    u_t ret = 0;
    vu_t s(a);
    for (u_t i = 0; i < N - 1; ++i)
    {
        vu_t::const_iterator iter = min_element(s.begin() + i, s.end());
        u_t j = iter - s.begin();
        for (u_t low = i, high = j; low < high; ++low, --high)
        {
            swap(s[low], s[high]);
        }
        u_t ij_cost = j + 1 - i;
        ret += ij_cost;
    }
    return ret;
}

void RevEng::reverse_engine(const vu_t& sub_costs)
{
    // typedef pair<u_t, u_t> uu_t;
    // typedef vector<uu_t> vuu_t;
    solution.clear();
    solution.reserve(N);
    vu_t pos; pos.reserve(N);
    for (u_t x = 0; x < N; ++x)
    {
        pos.push_back(x);
        // solution.push_back(x + 1);
    }
    // vuu_t swaps;
    const u_t nsc = sub_costs.size();
    for (u_t i = 0; i < nsc; ++i)
    {
        // u_t sub_cost = sub_costs[nsc - 1 - i];
        u_t sub_cost = sub_costs[i];
        // reverse [nsc - i - 1, .... + sub_costs]
        u_t low = nsc - i - 1, high = low + sub_cost - 1;
        for ( ; low < high; ++low, --high)
        {
            swap(pos[low], pos[high]);
        }
    }
    solution = vu_t(size_t(N), 0);
    for (u_t i = 0; i < N; ++i)
    {
        // solution[pos[i]] = i + 1;
        solution[i] = pos[i] + 1;
    }
    if (dbg_flags & 0x4)
    {
        u_t cost_check = solution_cost();
        if (cost_check != C)
        {
            cerr << "Failure: cost_check=" << cost_check << " != C="<<C << '\n';
        }
    }
}

void RevEng::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        for (u_t x: solution)
        {
            fo << ' ' << x;
        }
    }
}

static int real_main(int argc, char** argv)
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

    void (RevEng::*psolve)() =
        (naive ? &RevEng::solve_naive : &RevEng::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        RevEng reveng(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (reveng.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        reveng.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test(u_t N, u_t C)
{
    int rc = 0;
    dbg_flags |= 0x4;
    const char* fn_in = "reveng-auto.in";
    {
        ofstream f(fn_in);
        f << "1\n" << N << ' ' << C << '\n';
        f.close();
    }
    bool possible[2];
    for (int inaive = 0; inaive < 2; ++inaive)
    {
        if ((inaive == 1) || (N < 10))
        {
            ifstream f(fn_in);
            int dumt;
            f >> dumt;
            RevEng p(f);
            if (inaive == 0)
            {
                p.solve_naive();
            }
            else
            {
                p.solve();
            }
            possible[inaive] = p.possible();
            cerr << "N="<<N << ",  C="<<C << ", naive?="<<(inaive==0) <<
                ", possible=" << possible[inaive] << '\n';
            if (possible[inaive])
            {
                u_t cost = p.solution_cost();
                if (cost != C)
                {
                    cerr << "Failed inaive=" << inaive <<
                        ", cost=" << cost << " != " <<  C << '\n';
                    rc = 1;
                }
            }
        }
    }
    if ((rc == 0) && (N < 10) && (possible[0] != possible[1]))
    {
        cerr << "Inconsisyent possible: naive="<<possible[0] <<
            ", non-naive=" << possible[1] << '\n';
        rc = 1;
    }
    return rc;
}

static int test_main(int argc, char** argv)
{
    int rc = 0;
    int ai = 1; // test
    const u_t Nmin = stoi(argv[++ai]);
    const u_t Nmax = stoi(argv[++ai]);
    const u_t Cmin = stoi(argv[++ai]);
    const u_t Cmax = stoi(argv[++ai]);
    const u_t ntests = (Nmax + 1 - Nmin) * (Cmax + 1 - Cmin);
    u_t tested = 0;
    for (u_t N = Nmin; (rc == 0) && (N <= Nmax); ++N)
    {
        for (u_t C = Cmin; (rc == 0) && (C <= Cmax); ++C)
        {
            cerr << "tested: " << tested << '/' << ntests << '\n';
            rc = test(N, C);
            ++tested;
        }
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if ((argc > 1) && (string(argv[1]) == string("test")))
    {
        rc = test_main(argc, argv);
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
